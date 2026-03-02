/*
 * guiTask.h
 *
 *  Created on: Apr 17, 2021
 *      Author: dig
 */

#ifndef COMPONENTS_GUI_GUITASK_H_
#define COMPONENTS_GUI_GUITASK_H_
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "guiCommonTask.h"

extern QueueHandle_t displayMssgBox;
extern QueueHandle_t displayReadyMssgBox;

typedef enum {
	DISPLAY_ITEM_STATUSLINE, DISPLAY_ITEM_MEASLINE, DISPLAY_ITEM_MESSAGE, DISPLAY_ITEM_COLOR,  DISPLAY_ITEM_STOP
} displayItem_t;
typedef enum {
	USER_STATE_START, USER_STATE_RUN, USER_STATE_MENU, USER_STAT_DMM_MENU, USER_STATE_WIFI_MENU
} userState_t;
typedef struct {
	displayItem_t displayItem;
	float  *values;
} displayMssg_t;

extern "C" {

void guiTask(void *pvParameter);

}
#endif /* COMPONENTS_GUI_GUITASK_H_ */
