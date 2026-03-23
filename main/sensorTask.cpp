
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "sps30_i2c.h"

#include "guiTask.h"
#include "i2c.h"

#include "CGIcommonScripts.h"
#include "averager.h"
#include "cgiScripts.h"
#include "sensorTask.h"
#include "wifiConnect.h"

#include "time.h"

#define SPS30_ADDR 0x69
#include <esp_err.h>
#include <esp_log.h>

#define CLKSPEED 50000

#define DAYLOGINTERVAL 5   // minutes
#define HOURLOGINTERVAL 10 // seconds

#define AVERAGES1 10								  // number of values to average first
#define AVERAGES2 ((DAYLOGINTERVAL * 60) / AVERAGES1) // number of values to average secondary for daylog
#define SKIPFIRSTSAMPLES 10;						  // skip from log

static const char *TAG = "sensorTask";

const char measLabelTxt[][20] = {{"PM1 µg/m³"}, {"PM2.5 µg/m³"}, {"PM4 µg/m³"}, {"PM10 µg/m³"}, {"typ grootte µm"}};

extern i2c_master_dev_handle_t SPS30_dev_handle;
extern int scriptState;
extern uint32_t timeStamp;

log_t lastVal;

esp_err_t SPS30AddDeviceToBus(i2c_master_bus_handle_t *bus_handle) {

	i2c_device_config_t dev_config = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = SPS30_ADDR,
		.scl_speed_hz = CLKSPEED,
	};
	return i2c_master_bus_add_device(*bus_handle, &dev_config, &SPS30_dev_handle);
}

Averager averager1[NR_MEASVALUES];
Averager averager2[NR_MEASVALUES];

Log hourLog(3600 / HOURLOGINTERVAL, sizeof(log_t));	   // 10 sec interval
Log dayLog((24 * 60) / DAYLOGINTERVAL, sizeof(log_t)); // 5 min interval

void sensorTask(void *parameters) {
	esp_err_t error = ESP_OK;
	uint32_t device_status = 0;
	displayMssg_t displayMssg;
	char str[20];
	log_t logValue;
	int lastMinute = -1;
	int lastSecond = -1;
	time_t now = 0;
	struct tm timeinfo;
	int logDayPrescaler = 1;
	int logHourPrescaler = 1;
	float values[NR_MEASVALUES];
	float avGvalues[NR_MEASVALUES];
	bool statusError;
	int skipFirstSamplesFromLog = SKIPFIRSTSAMPLES;

	for (int n = 0; n < NR_MEASVALUES; n++) {
		averager1[n].setAverages(AVERAGES1);
		averager2[n].setAverages(AVERAGES2);
	}

	do {
		error = i2c_master_probe(bus_handle, SPS30_ADDR, 50);
		if (error) {
			ESP_LOGE(TAG, "SPS30 sensor not detected", error);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	} while (error);

	error = SPS30AddDeviceToBus(&bus_handle);
	if (error)
		ESP_LOGE(TAG, "Failed to add SPS30 to bus", error);

	sps30_stop_measurement();
	int8_t serial_number[32] = {0};
	int8_t product_type[8] = {0};
	do {
		error = sps30_read_serial_number(serial_number, 32);
		if (error != ESP_OK) {
			ESP_LOGE(TAG, "Failed to read serialnr %d", error);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	} while (error != ESP_OK);

	ESP_LOGI(TAG, "SPS 30 serial_number: %s\n", serial_number);
	sps30_read_product_type(product_type, 8);
	ESP_LOGI(TAG, "SPS 30 product_type: %s\n", product_type);

	//	sps30_start_measurement((sps30_output_format)(1280));
	sps30_start_measurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);
	vTaskDelay(1 / portTICK_PERIOD_MS);
	uint16_t data_ready_flag = 0;
	displayMssg.str1 = str;
	// uint16_t mc_1p0 = 0;
	// uint16_t mc_2p5 = 0;
	// uint16_t mc_4p0 = 0;
	// uint16_t mc_10p0 = 0;
	// uint16_t nc_0p5 = 0;
	// uint16_t nc_1p0 = 0;
	// uint16_t nc_2p5 = 0;
	// uint16_t nc_4p0 = 0;
	// uint16_t nc_10p0 = 0;
	// uint16_t typical_particle_size = 0;
	//   uint16_t repetition = 0;
	//   for (repetition = 0; repetition < 50; repetition++) {
	while (1) {
		vTaskDelay(100 / portTICK_PERIOD_MS);
		statusError = false;
		error = sps30_read_device_status_register(&device_status);
		if (error != NO_ERROR) {
			strcpy(str, "Error reading SPS30");
			statusError = true;
		} else {
			if (device_status & (1 << 4)) {
				strcpy(str, "Error fan SPS30");
				statusError = true;
			}
			if (device_status & (1 << 5)) {
				strcpy(str, "Error laser SPS30");
				statusError = true;
			}
			if (device_status & (1 << 21)) {
				strcpy(str, "Error speed SPS30");
				statusError = true;
			}
		}

		if (statusError) {
			displayMssg.displayItem = DISPLAY_ITEM_STATUSLINE;
			if (displayMssgBox != NULL)
				xQueueSend(displayMssgBox, &displayMssg, 0);
		} else {

			displayMssg.displayItem = DISPLAY_ITEM_MEASLINE;

			error = sps30_read_data_ready_flag(&data_ready_flag);
			if (error != NO_ERROR) {
				printf("error executing read_data_ready_flag(): %i\n", error);
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				continue;
			}
			if (data_ready_flag) {
				error = sps30_read_measurement_values_float(&values[0], &values[1], &values[2], &values[3], &values[4], &values[5], &values[6],
															&values[7], &values[8], &values[9]);
				if (skipFirstSamplesFromLog)
					skipFirstSamplesFromLog--;
				else {
					if (error != NO_ERROR) {
						printf("error executing read_measurement_values_uint16(): %i\n", error);
						continue;
					} else {
						for (int n = 0; n < NR_MEASVALUES - 1; n++) { // average mc's
							averager1[n].write((int)1000.0 * values[n]);
							avGvalues[n] = averager1[n].average() / 1000;
						}
						averager1[NR_MEASVALUES - 1].write((int)1000.0 * values[9]); // add typ. partical size to display and log
						avGvalues[NR_MEASVALUES - 1] = averager1[NR_MEASVALUES - 1].average() / 1000;

						displayMssg.values = &avGvalues[0];
						if (displayMssgBox != NULL)
							xQueueSend(displayMssgBox, &displayMssg, 0);
						memcpy(lastVal.values, avGvalues, sizeof(logValue.values));
						lastVal.timeStamp = timeStamp;
					}

					time(&now);
					localtime_r(&now, &timeinfo);
					if (lastMinute == -1) {
						lastMinute = timeinfo.tm_min;
						lastSecond = timeinfo.tm_sec;
					}
					if (lastSecond != timeinfo.tm_sec) {
						lastSecond = timeinfo.tm_sec; // every second

						if (logHourPrescaler > 0)
							logHourPrescaler--;
						else {
							logHourPrescaler = HOURLOGINTERVAL;

							for (int n = 0; n < NR_MEASVALUES - 1; n++) {
								avGvalues[n] = averager1[n].average();
								averager2[n].write(avGvalues[n]); // second order for daylog
							}

							for (int n = 0; n < NR_MEASVALUES - 1; n++) {	  // average mc's
								avGvalues[n] = averager1[n].average() / 1000; // contains last samples
							}
							memcpy(logValue.values, avGvalues, sizeof(logValue.values));
							logValue.timeStamp = timeStamp;
							hourLog.add(&logValue);
							// for ( int n = 0;n< 100;n++) {
							//  			logValue.timeStamp = timeStamp++;
							//  			hourLog.add(&logValue);
							//  			dayLog.add(&logValue);
							// }
						}

						if (lastMinute != timeinfo.tm_min) {
							lastMinute = timeinfo.tm_min; // every minute

							if (logDayPrescaler > 0) {
								logDayPrescaler--;
							} else {
								logDayPrescaler = DAYLOGINTERVAL;			  // reset prescaler
								for (int n = 0; n < NR_MEASVALUES - 1; n++) { // average mc's
									avGvalues[n] = averager2[n].average() / 1000;
								}
								memcpy(logValue.values, avGvalues, sizeof(logValue.values));
								logValue.timeStamp = timeStamp;
								dayLog.add(&logValue);
							}
						}
						// printf("mc_1p0: %u ", mc_1p0);
						// printf("mc_2p5: %u ", mc_2p5);
						// printf("mc_4p0: %u ", mc_4p0);
						// printf("mc_10p0: %u ", mc_10p0);
						// printf("nc_0p5: %u ", nc_0p5);
						// printf("nc_1p0: %u ", nc_1p0);
						// printf("nc_2p5: %u ", nc_2p5);
						// printf("nc_4p0: %u ", nc_4p0);
						// printf("nc_10p0: %u ", nc_10p0);
						// printf("typical_particle_size: %u\n", typical_particle_size);
					}
				}
			}
		}
	}
}

// CGI stuff

int printLog(log_t *logToPrint, char *pBuffer) {
	int len = 0;
	len = sprintf(pBuffer, "%ld,", logToPrint->timeStamp);
	for (int n = 0; n < NR_MEASVALUES; n++)
		len += sprintf(pBuffer + len, "%3.1f,", logToPrint->values[n]);
	len += sprintf(pBuffer + len, "\n");
	return len;
}

// reads all available data from log
// issued as first request.

int getDayLogsScript(char *pBuffer, int count) {
	int left, len = 0;
	static int logsToSend = -1;
	if (logsToSend < 0)
		logsToSend = dayLog.getNrLogsToSend();

	if (logsToSend) {
		len = 0;
		do {
			len += printLog((log_t *)dayLog.readNext(), pBuffer + len);
			logsToSend--;
			left = count - len;
		} while (logsToSend && (left > (len + 50)));
	} else
		logsToSend = -1;
	return len;
}

int getHourLogsScript(char *pBuffer, int count) {
	int left, len = 0;
	static int logsToSend = -1;
	if (logsToSend < 0)
		logsToSend = hourLog.getNrLogsToSend();

	if (logsToSend) {
		len = 0;
		do {
			len += printLog((log_t *)hourLog.readNext(), pBuffer + len);
			logsToSend--;
			left = count - len;
		} while (logsToSend && (left > (len + 50)));
	} else
		logsToSend = -1;
	return len;
}

int clearLogScript(char *pBuffer, int count) {
	dayLog.clear();
	hourLog.clear();
	if (scriptState == 0) {
		strcpy(pBuffer, "OK");
		scriptState++;
		return 3;
	}
	return 0;
}

int getRTMeasValuesScript(char *pBuffer, int count) {
	int len = 0;
	switch (scriptState) {
	case 0:
		scriptState++;
		len += printLog(&lastVal, pBuffer);
		return len;
		break;
	default:
		break;
	}
	return 0;
}

const CGIdesc_t sensorInfoDescriptorTable[] = {{measLabelTxt[0], &lastVal.values[0], FLT, 1}, {measLabelTxt[1], &lastVal.values[1], FLT, 1},
											   {measLabelTxt[2], &lastVal.values[2], FLT, 1}, {measLabelTxt[3], &lastVal.values[3], FLT, 1},
											   {measLabelTxt[4], &lastVal.values[4], FLT, 1}, {NULL, NULL, INT, 0}};

int getSensorInfoScript(char *pBuffer, int count) {
	int len = 0;

	switch (scriptState) {
	case 0:
		scriptState++;
		len = sprintf(pBuffer, "Partikel, Waarde\n");
		len += getCGItable(sensorInfoDescriptorTable, pBuffer + len, count);
		return len;
		break;
	default:
		break;
	}
	return 0;
}
