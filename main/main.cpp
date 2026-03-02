/*
 * SPDX-FileCopyrightText: 2022 atanisoft (github.com/atanisoft)
 *
 * SPDX-License-Identifier: MIT
 */

#include <driver/gpio.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>

#include "nvs_flash.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "guiTask.h"
#include "i2c.h"
#include "sensorTask.h"

#include "settings.h"
#include "wifiConnect.h"

#include "guiTask.h"

TaskHandle_t guiCommonTaskh;
TaskHandle_t guiTaskh;

static const char *TAG = "main";

esp_err_t init_spiffs(void);
i2c_master_bus_handle_t bus_handle;
uint32_t timeStamp = 1; // global timestamp for logging

extern "C" void app_main() {

	esp_err_t err = init_spiffs();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(err));
		return;
	}

	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
		ESP_LOGI(TAG, "nvs flash erased");
	}
	ESP_ERROR_CHECK(err);

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	err = loadSettings();

	systemInfo.startUps++;
	strcpy(userSettings.moduleName, "FSM");

	saveSettings();

	wifiConnect();

	i2c_master_bus_init(&bus_handle);

	initGui();

	xTaskCreatePinnedToCore(guiCommonTask, "guicommon", 4096 * 2, NULL, 0, &guiCommonTaskh, 1);
	while (!displayReady)
		vTaskDelay(10 / portTICK_PERIOD_MS);

	xTaskCreatePinnedToCore(guiTask, "guiTask", 4096, NULL, 0, &guiTaskh, 1);
	xTaskCreate(sensorTask, "sensorTask", 2 * 4096, NULL, 0, NULL);

	while (1) {
		vTaskDelay(1000);
		timeStamp++;
	}
}
