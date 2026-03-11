/*
 * log.h
 *
 *  Created on: Nov 3, 2023
 *      Author: dig
 */

#ifndef MAIN_INCLUDE_LOG_H_
#define MAIN_INCLUDE_LOG_H_

#include <stdint.h>
#include <esp_err.h>

class Log {
public:
	Log(int _logValues, int _logTypeSize);
	int add( void * logValue);
	void * getLog(int idx);
	void clear();
	int getNrLogs();
	int getNrLogsToSend();
	void * readNext();

private:
	uint8_t *pLogBuffer;
	int maxValues;
	int logTypeSize;
	int actValues;
	int logRxIdx;
	int logTxIdx;
};


#endif /* MAIN_INCLUDE_LOG_H_ */