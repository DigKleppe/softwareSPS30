/*
 * guiTask.c
 *
 *  Created on: Mar 2, 2021
 *      Author: dig
 *
 *      handles screens
 */

#include "guiTask.h"
#include "MainScreen.h"

const char measLabelTxt[][10] = {{"mc 1p0"}, {"mc 2p5"}, {"mc 4p0"}, {"mc 10p0"}, {"nc 0p5"},
								 {"nc 1p0"}, {"nc 2p5"}, {"nc 4p0"}, {"nc 10p0"}, {"typ size"}};

void guiTask(void *pvParameter);
// extern "C" {      if ( timer++ == 200) {

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "styles.h"

extern "C" void disp_wait_for_pending_transactions(void);

userState_t userState = USER_STATE_RUN;

QueueHandle_t displayMssgBox;
QueueHandle_t displayReadyMssgBox;
MainScreen *mainScreen;

extern char myIP[];
extern uint32_t upTime;

void showScreen(int idx) {
	switch (idx) {

	case 1:
		mainScreen->show();
		break;

	default:
		break;
	}
}

void guiTask(void *pvParameter) {
	displayMssg_t recDdisplayMssg;
	int dummy;

	displayMssgBox = xQueueCreate(5, sizeof(displayMssg_t));
	displayReadyMssgBox = xQueueCreate(1, sizeof(uint32_t));

	while (!displayReady)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	mainScreen = new MainScreen();
	vTaskDelay(50 / portTICK_PERIOD_MS);
	mainScreen->show();

	while (1) {
		if (xQueueReceive(displayMssgBox, &recDdisplayMssg, 1000) == pdTRUE) {
			if (pdTRUE == xSemaphoreTake(xGuiSemaphore, 100)) { // portMAX_DELAY)) {
				switch (recDdisplayMssg.displayItem) {
				case DISPLAY_ITEM_STATUSLINE:
					mainScreen->setStatusLine((const char *)recDdisplayMssg.str1);
					break;
				case DISPLAY_ITEM_MEASLINE:
					mainScreen->update(recDdisplayMssg.values);
					break;
				default:
					break;
				}
				disp_wait_for_pending_transactions();
				xSemaphoreGive(xGuiSemaphore);
				xQueueSend(displayReadyMssgBox, &dummy, 0);
			}
		}
	}
}

