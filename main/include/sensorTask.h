
#include "log.h"

#define NR_MEASVALUES 5 // only mass concetration and typ.size

void sensorTask(void *parameters);
#define _ERRORVALUE         9999 
int getRTMeasValuesScript(char *pBuffer, int count);


int printLog(log_t *logToPrint, char *pBuffer, int idx);
int printLog(log_t *logToPrint, char *pBuffer);

