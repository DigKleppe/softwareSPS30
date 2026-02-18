
#include "log.h"

void sensorTask(void *parameters);
#define _ERRORVALUE         9999 
int getRTMeasValuesScript(char *pBuffer, int count);


int printLog(log_t *logToPrint, char *pBuffer, int idx);
int printLog(log_t *logToPrint, char *pBuffer); 