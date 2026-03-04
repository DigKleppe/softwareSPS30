/*
 * guiTask.c
 *
 *  Created on: Mar 2, 2021
 *      Author: dig
 */

#include "lvgl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "backGround.h"
#include "esp_err.h"

#include "esp_timer.h"

#include "../zhw990217__lvgl_esp32_drivers/lvgl_tft/disp_driver.h"

// #include "lvgl_helpers.h"

// fonts 0x5E 0xB0 0123456789 :-.,C%
// https://lvgl.io/tools/fontconverter
#define LV_TICK_PERIOD_MS 10 // 10

extern "C" {
/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;
static void lv_tick_task(void *arg);
lv_indev_drv_t indev_drv;
lv_indev_t *kb_indev;
lv_indev_data_t keydata;
volatile bool displayReady;

void initKeyboardDriver();
uint8_t i2cErr = 0;

// #define DISP_BUF_SIZE ((LV_HOR_RES_MAX * LV_VER_RES_MAX)/10)

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1;
static lv_color_t *buf2;

static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO 2 // Define the output GPIO
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY (4095)				// Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY (5000)			// Frequency in Hertz. Set frequency at 5 kHz

static void initBacklight(void) {
	// // Prepare and then apply the LEDC PWM timer configuration
	// ledc_timer_config_t ledc_timer = {
	//      .speed_mode       = LEDC_MODE,
	//      .duty_resolution  = LEDC_DUTY_RES,
	//      .timer_num        = LEDC_TIMER,
	//      .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5
	//      kHz
	//     .clk_cfg          = LEDC_AUTO_CLK
	// };
	// ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

	// // Prepare and then apply the LEDC PWM channel configuration
	// ledc_channel_config_t ledc_channel = {
	// 	.gpio_num       = LEDC_OUTPUT_IO,
	//     .speed_mode     = LEDC_MODE,
	//     .channel        = LEDC_CHANNEL,
	// 	.intr_type      = LEDC_INTR_DISABLE,
	//     .timer_sel      = LEDC_TIMER,
	// 	.duty           = 0, // Set duty to 0%
	//     .hpoint         = 0,
	// 	.flags =
	// 	{
	// 		.output_invert = 0,
	// 	}
	// };
	// ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}
void setBacklight(int percent) {
	// 	uint32_t duty = 0;
	// 	if ( percent <= 100)
	// 		duty = ((1<<13)  * percent) / 100;
	// 	else
	// 		duty = percent;
	// 	if ( duty >= (1<<13))
	// 		duty = (1<<13) -1;
	// //	ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
	// //	ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

	// 	ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
	// 	ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void initGui(void) {

	//	initBacklight();
	//
	//	buf1 = (lv_color_t*)heap_caps_malloc(DISP_BUF_SIZE *
	// sizeof(lv_color_t), MALLOC_CAP_DMA); 	assert(buf1 != NULL); 	buf2 =
	//(lv_color_t*)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t),
	// MALLOC_CAP_DMA); 	assert(buf2 != NULL);
	//
	//	xGuiSemaphore = xSemaphoreCreateMutex();
	//
	//	lv_init();
	//
	//	lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DISP_BUF_SIZE);
	//
	//	lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
	//	disp_drv.flush_cb = disp_driver_flush;    /*Set your driver
	// function*/ 	disp_drv.draw_buf = &draw_buf;          /*Assign the buffer
	// to the display*/ 	disp_drv.hor_res = LV_HOR_RES_MAX;   /*Set the
	// horizontal resolution of the display*/ 	disp_drv.ver_res =
	// LV_VER_RES_MAX;   /*Set the vertical resolution of the display*/
	//	lv_disp_drv_register(&disp_drv);      /*Finally register the
	// driver*/
	//
	//		/* Initialize SPI or I2C bus used by the drivers */
	//	lvgl_driver_init();
	//
	//	/* Register an input device when enabled on the menuconfig */
	// #if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
	//	lv_indev_drv_t indev_drv;
	//	lv_indev_drv_init(&indev_drv);
	//	indev_drv.read_cb = touch_driver_read;
	//	indev_drv.type = LV_INDEV_TYPE_POINTER;
	//	lv_indev_drv_register(&indev_drv);
	// #endif
	//
	//	/* Create and start a periodic timer interrupt to call
	// lv_tick_inc */ 	const esp_timer_create_args_t periodic_timer_args = {
	//			.callback = &lv_tick_task,
	//			.name = "periodic_gui"
	//	};
	//
	//	esp_timer_handle_t periodic_timer;
	//	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args,
	//&periodic_timer));
	//	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer,
	//(LV_TICK_PERIOD_MS/portTICK_PERIOD_MS) * 1000));
}

void guiCommonTask(void *pvParameter) {

	(void)pvParameter;
	TickType_t xLastWakeTime;

	initBacklight();

	buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
	assert(buf1 != NULL);
	buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
	assert(buf2 != NULL);

	xGuiSemaphore = xSemaphoreCreateMutex();

	lv_init();

	lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DISP_BUF_SIZE);

	lv_disp_drv_init(&disp_drv);		   /*Basic initialization*/
	disp_drv.flush_cb = disp_driver_flush; /*Set your driver function*/
	disp_drv.draw_buf = &draw_buf;		   /*Assign the buffer to the display*/
	disp_drv.hor_res = LV_HOR_RES_MAX;	   /*Set the horizontal resolution of the display*/
	disp_drv.ver_res = LV_VER_RES_MAX;	   /*Set the vertical resolution of the display*/
	lv_disp_drv_register(&disp_drv);	   /*Finally register the driver*/

	/* Initialize SPI or I2C bus used by the drivers */
	lvgl_driver_init();

	/* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.read_cb = touch_driver_read;
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	lv_indev_drv_register(&indev_drv);
#endif

	/* Create and start a periodic timer interrupt to call lv_tick_inc */
	const esp_timer_create_args_t periodic_timer_args = {.callback = &lv_tick_task, .name = "periodic_gui"};

	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, (LV_TICK_PERIOD_MS / portTICK_PERIOD_MS) * 1000));

	setBacklight(100);

	initStyles();
	
	displayReady = true;

	while (1) {
		vTaskDelayUntil(&xLastWakeTime, LV_TICK_PERIOD_MS / portTICK_PERIOD_MS);
		/* Try to take the semaphore, call lvgl related function on success
		 */
		if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
			lv_timer_handler();
			xSemaphoreGive(xGuiSemaphore);
		}
	}
}

static void lv_tick_task(void *arg) {
	(void)arg;

	lv_tick_inc(LV_TICK_PERIOD_MS);

	///	keydata.key = mpr121.readTouchBits();
}
}
