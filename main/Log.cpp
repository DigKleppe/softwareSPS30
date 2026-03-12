/*
 * Log.cpp
 *
 *  Created on: Mar 11, 2026
 *      Author: dig
 */
#include "Log.h"
#include <cstdint>
#include <cstdlib>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "Log";

Log::Log(int _maxLogValues, int _logTypeSize) {
	maxValues = _maxLogValues;
	logTypeSize = _logTypeSize;

#ifdef CONFIG_SPIRAM
	pLogBuffer =(uint8_t *)  heap_caps_malloc(maxValues * _logTypeSize), MALLOC_CAP_SPIRAM);
#else
	pLogBuffer = (uint8_t *)malloc(maxValues * logTypeSize);
#endif
	if (pLogBuffer == 0)
		// ESP_LOGE(TAG, "No space for Logbuffer!");
		printf("No space for Logbuffer!");
	else {
		memset(pLogBuffer, 0, (maxValues * logTypeSize));
	}
	logRxIdx = 0;
	logTxIdx = 0;
	actValues = 0;
}

// esp_err_t Log::add(log_t logValue) {
int Log::add(void *logValue) {
	if (pLogBuffer == 0)
		return -1; //  ESP_ERR_NO_MEM;
	memcpy(&pLogBuffer[logTxIdx], logValue, logTypeSize);
	logTxIdx += logTypeSize;

	if (logTxIdx >= maxValues * logTypeSize)
		logTxIdx = 0;
	if (actValues < maxValues)
		actValues++;
	return 0; //  ESP_OK;
}

void Log::clear() {
	if (pLogBuffer == 0)
		return;
	logRxIdx = 0;
	logTxIdx = 0;
	actValues = 0;
	memset(pLogBuffer, 0, (maxValues * logTypeSize));
}

// search oldest log in  cyclic buffer
// returns nr of logs to send (read)

int Log::getNrLogsToSend() {
	if (pLogBuffer == 0)
		return -1;

	if (actValues < maxValues) // buffer filled from 0
		logRxIdx = 0;
	else
		logRxIdx = logTxIdx; // cyclic buffer , logTxidx points to oldest value

	ESP_LOGI(TAG, "logRxidx:%d , actvalues: %d", logRxIdx, actValues);

	return actValues;
}

void *Log::readNext() {
	void *p = &pLogBuffer[logRxIdx];
	logRxIdx += logTypeSize;
	if (logRxIdx >= maxValues * logTypeSize)
		logRxIdx = 0;
	return p;
}

/*
#include <cstdlib>
#include <stdio.h>
#include <cstdint>
#include <stdint.h>
#include "Log.h"

using namespace std;

uint32_t timeStamp = 1;

typedef struct {
	uint32_t timeStamp;
	float values[5];
} log1_t;

typedef struct {
	uint32_t timeStamp;
	float values[10];
} log2_t;

Log log1(10, sizeof(log1_t));
Log log_2(20, sizeof(log2_t));

log1_t logVal1;
log2_t logVal2;

int main() {
	int m;
	log1_t *rlogVal1;
	log2_t *RlogVal2;

	for (int n = 0; n < 5; n++) {
		logVal1.timeStamp = timeStamp;
		logVal2.timeStamp = timeStamp++;
		logVal1.values[n] = n * 10.0;
		logVal2.values[n] = n * 20.0;
		log1.add(&logVal1);
		log_2.add(&logVal2);
	}
	m = log1.getNrLogsToSend();
	for (int n = 0; n < m; n++) {
		rlogVal1 = (log1_t*) log1.readNext();
		printf("tS1: %d ", rlogVal1->timeStamp);
	}
	return 0;
}
*/