#include "Log.h"

#define NR_MEASVALUES 5 // only mass concetration and typ.size

void sensorTask(void *parameters);
#define _ERRORVALUE         9999 
int getRTMeasValuesScript(char *pBuffer, int count);
int getSensorInfoScript(char *pBuffer, int count);
int getDayLogsScript(char *pBuffer, int count);
int getHourLogsScript(char *pBuffer, int count);
int clearLogScript(char *pBuffer, int count) ;

// int printLog(log_t *logToPrint, char *pBuffer, int idx);
// int printLog(log_t *logToPrint, char *pBuffer);

typedef struct {
	uint32_t timeStamp;
	float values[5];
} log_t;
