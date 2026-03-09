
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
#include "log.h"
#include "sensorTask.h"
// #include "settings.h"
// #include "udpServer.h"
#include "wifiConnect.h"

#define SPS30_ADDR 0x69
#include <esp_err.h>
#include <esp_log.h>

#define CLKSPEED 50000

#define LOGINTERVAL 5 // // minutes
#define AVERAGES 30	  // number of values to average

static const char *TAG = "sensorTask";

const char measLabelTxt[][10] = {{"mc 1p0"}, {"mc 2p5"}, {"mc 4p0"}, {"mc 10p0"}, {"nc 0p5"},
								 {"nc 1p0"}, {"nc 2p5"}, {"nc 4p0"}, {"nc 10p0"}, {"typ size"}};

extern i2c_master_dev_handle_t SPS30_dev_handle;
extern int scriptState;

log_t lastVal;

esp_err_t SPS30AddDeviceToBus(i2c_master_bus_handle_t *bus_handle) {

	i2c_device_config_t dev_config = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = SPS30_ADDR,
		.scl_speed_hz = CLKSPEED,
	};
	return i2c_master_bus_add_device(*bus_handle, &dev_config, &SPS30_dev_handle);
}



Averager averager[NR_MEASVALUES];

void sensorTask(void *parameters) {
	esp_err_t error = ESP_OK;
	uint32_t device_status = 0;
	displayMssg_t displayMssg;
	log_t logValue;
	int lastminute = -1;
	time_t now = 0;
	struct tm timeinfo;
	int logPrescaler = 1;
	float values[NR_MEASVALUES];
	float avGvalues[ NR_MEASVALUES];
	initLogBuffer(); // psRAM


	for (int n = 0; n < NR_MEASVALUES; n++)
		averager[n].setAverages(AVERAGES);

	do {	
		error = i2c_master_probe(bus_handle, SPS30_ADDR, 50);
		if ( error ) {
			ESP_LOGE(TAG, "SPS30 sensor not detected", error);
			vTaskDelay (1000/portTICK_PERIOD_MS);
		}
	} while( error);


	error = SPS30AddDeviceToBus(&bus_handle);
	if( error)
		ESP_LOGE(TAG, "Failed to add SPS30 to bus", error);
			
			
	sps30_stop_measurement();
	int8_t serial_number[32] = {0};
	int8_t product_type[8] = {0};
	do  {
		error = sps30_read_serial_number(serial_number, 32);
		if ( error != ESP_OK) {
			ESP_LOGE(TAG, "Failed to read serialnr %d", error);
			vTaskDelay (1000/portTICK_PERIOD_MS);
		}
	} while  ( error != ESP_OK);


	printf("serial_number: %s\n", serial_number);
	sps30_read_product_type(product_type, 8);
	printf("product_type: %s\n", product_type);
	//	sps30_start_measurement((sps30_output_format)(1280));
	sps30_start_measurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);
	vTaskDelay(1 / portTICK_PERIOD_MS);
	uint16_t data_ready_flag = 0;
	displayMssg.displayItem = DISPLAY_ITEM_MEASLINE;
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
		error = sps30_read_data_ready_flag(&data_ready_flag);
		if (error != NO_ERROR) {
			printf("error executing read_data_ready_flag(): %i\n", error);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}
		//	printf("data_ready_flag: %u\n", data_ready_flag);
		if (data_ready_flag) {
			// error = sps30_read_measurement_values_uint16(&mc_1p0, &mc_2p5, &mc_4p0, &mc_10p0, &nc_0p5, &nc_1p0, &nc_2p5, &nc_4p0, &nc_10p0,
			// 											 &typical_particle_size);
			//	error =
			// sps30_read_measurement_values_uint16(&values[0],&values[1],&values[2],&values[3],&values[4],&values[5],&values[6],&values[7],&values[8],&values[9]);
			error = sps30_read_measurement_values_float(&values[0], &values[1], &values[2], &values[3], &values[4], &values[5], &values[6],
														&values[7], &values[8], &values[9]);

			if (error != NO_ERROR) {
				printf("error executing read_measurement_values_uint16(): %i\n", error);
				continue;
			} else {
				for ( int n = 0; n < NR_MEASVALUES -1;n++) {  // average mc's
					averager[n].write( (int) 1000.0 * values[n]);
					avGvalues[n] =  averager[n].average()/ 1000;
				}
				averager[NR_MEASVALUES-1].write( (int) 1000.0 * values[9]);  // add typ. partical size to display and log
				avGvalues[NR_MEASVALUES-1] =  averager[NR_MEASVALUES-1].average()/ 1000;

				displayMssg.values = &avGvalues[0];
				if (displayMssgBox != NULL)
					xQueueSend(displayMssgBox, &displayMssg, 0);
				memcpy(lastVal.values, avGvalues, sizeof(logValue.values));
				lastVal.timeStamp = timeStamp;

				time(&now);
				localtime_r(&now, &timeinfo);
				if (lastminute == -1) {
					lastminute = timeinfo.tm_min;
				}
				if (lastminute != timeinfo.tm_min) {
					lastminute = timeinfo.tm_min; // every minute

					if (logPrescaler > 0) {
						logPrescaler--;
					} else {
						logPrescaler = LOGINTERVAL; // reset prescaler
						memcpy(logValue.values, avGvalues, sizeof(logValue.values));
						addToLog(logValue);
					}
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

		error = sps30_read_device_status_register(&device_status);
		if (error != NO_ERROR)
			printf("Error reading device status\n");
		// else {
		// 	if (device_status && (1 << 4))
		// 		printf("Error fan\n");
		// 	if (device_status && (1 << 5))
		// 		printf("Error laser\n");
		// 	if (device_status && (1 << 21))
		// 		printf("Error speed\n");
		// }
	}
}

// CGI stuff

// int printLog(log_t *logToPrint, char *pBuffer, int idx) {
// 	int len;
// 	len = sprintf(pBuffer, "%ld,", logToPrint->timeStamp);
// 	for (int n = 0; n < NR_VALUES; n++)
// 		len += sprintf(pBuffer + len, "%3.1f,", logToPrint->value[n]);
// 	return len;
// }

int printLog(log_t *logToPrint, char *pBuffer) {
	int len = 0;
	len = sprintf(pBuffer, "%ld,", logToPrint->timeStamp);
	for (int n = 0; n < NR_MEASVALUES; n++)
		len += sprintf(pBuffer + len, "%3.1f,", logToPrint->values[n]);
	len += sprintf(pBuffer + len, "\n");
	return len;
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
// not used 

const CGIdesc_t sensorInfoDescriptorTable[][7] = {{{measLabelTxt[0], &lastVal.values[0], FLT, 1},
												   {measLabelTxt[1], &lastVal.values[1], FLT, 1},
												   {measLabelTxt[2], &lastVal.values[2], FLT, 1},
												   {NULL, NULL, INT, 1}}};

int getSensorInfoScript(char *pBuffer, int count) {
	int len = 0;
	switch (scriptState) {
	case 0:
		scriptState++;
		len = sprintf(pBuffer, "Parameter, Waarde\n");
		for (int n = 0; n < 3; n++) {
			len += getCGItable(sensorInfoDescriptorTable[n], pBuffer + len, count);
		}
		return len;
	default:
		break;
	}
	return 0;
}
