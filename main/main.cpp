/*
 * SPDX-FileCopyrightText: 2022 atanisoft (github.com/atanisoft)
 *
 * SPDX-License-Identifier: MIT
 */

#include <driver/gpio.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "gui.h"
#include "i2c.h"
#include "sensorTask.h"


i2c_master_bus_handle_t bus_handle;

extern "C" void app_main() {

	int p = 0;
	i2c_master_bus_init(&bus_handle);

	xTaskCreatePinnedToCore(guiTask, "guiTask", 2* 4096, NULL, 0, NULL, 1);
	xTaskCreate(sensorTask, "sensorTask", 4096, NULL, 0, NULL);

	while (1) {
		vTaskDelay(100);
	}
}
