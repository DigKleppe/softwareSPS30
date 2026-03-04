/*
 *
 *  Created on: Feb 8, 2018
 *      Author: dig
 */

#include "include/averager.h"
#include <stdlib.h>

#ifdef USE_FREERTOS
#include "freertos/FreeRTOS.h"
#endif

#include <esp_heap_caps.h>

Averager::Averager(uint32_t averages) {
	bufWriteIndex = 0;
	bufValues = 0;
	bufSize = averages;
#ifdef CONFIG_SPIRAM
	pBuffer = (int32_t *)heap_caps_malloc(bufSize * 4, MALLOC_CAP_SPIRAM);
#else
	pBuffer = (int32_t *)malloc(bufSize * 4);
#endif
}

void Averager::clear() {
	bufWriteIndex = 0;
	bufValues = 0;
}

int32_t *Averager::setAverages(uint32_t averages) {
	if (averages > 0) {
		bufSize = averages;
		bufWriteIndex = 0;
		bufValues = 0;
		free(pBuffer);
#ifdef CONFIG_SPIRAM
		pBuffer = (int32_t *)heap_caps_malloc(bufSize * 4, MALLOC_CAP_SPIRAM);
#else
		pBuffer = (int32_t *)malloc(bufSize * 4);
#endif
		return pBuffer;
	}
	return NULL;
}

// write cyclic buffer
int32_t Averager::write(int32_t value) {
	if (pBuffer == NULL) {
		return -1;
	} else {
		if (bufValues < bufSize)
			bufValues++;

		*(pBuffer + bufWriteIndex) = value;
		bufWriteIndex++;
		if (bufWriteIndex == bufSize)
			bufWriteIndex = 0;
	}
	return 0;
}

float Averager::average(void) {
	float result = 0;
	int64_t averageAccu = 0;
	highest = INT32_MIN;
	lowest = INT32_MAX;

	int32_t *p = (int32_t *)pBuffer;
	int32_t value;
	if (bufValues > 0) {
		for (int n = 0; n < bufValues; n++) {
			value = *p++;
			averageAccu += value;
			if (value > highest)
				highest = value;
			if (value < lowest)
				lowest = value;
		}
		if (bufValues > 2) {
			averageAccu -= lowest;
			averageAccu -= highest;
			result = (float)averageAccu / (bufValues - 2);
		} else
			result = (float)averageAccu / bufValues;
	}
	return result;
}

int32_t Averager::getHighest(void) {
	highest = INT32_MIN;
	int32_t *p = (int32_t *)pBuffer;
	int32_t value;
	if (bufValues > 0) {
		for (int n = 0; n < bufValues; n++) {
			value = *p++;
			if (value > highest)
				highest = value;
			if (value < lowest)
				lowest = value;
		}
	}
	return highest;
}

int32_t Averager::getLowest(void) {
	lowest = INT32_MAX;
	int32_t *p = (int32_t *)pBuffer;
	int32_t value;
	if (bufValues > 0) {
		for (int n = 0; n < bufValues; n++) {
			value = *p++;
			if (value < lowest)
				lowest = value;
		}
	}
	return lowest;
}
