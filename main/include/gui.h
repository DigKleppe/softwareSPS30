#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <lvgl.h>

#define NR_VALUES 10
void guiTask( void *);
void initMeasureScreen(void);


extern QueueHandle_t displayMssgBox;
extern QueueHandle_t displayReadyMssgBox;

typedef struct {
    float * values;
} displayMssg_t;
