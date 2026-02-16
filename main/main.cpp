/*
 * SPDX-FileCopyrightText: 2022 atanisoft (github.com/atanisoft)
 *
 * SPDX-License-Identifier: MIT
 */

#include <driver/gpio.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>

#include "gui.h"


uint16_t values[ NR_VALUES];

extern "C" void app_main() {

    int p = 0;
    displayMssg_t mssg;


    xTaskCreatePinnedToCore(guiTask, "guiTask", 2* 4096, NULL, 0, NULL, 1);
    mssg.values = &values[0];

     while (1) {
         vTaskDelay(100);
         for ( int n = 0; n < NR_VALUES;n++) {
            values[n] = n + p;
         }
         p++;
        xQueueSend(displayMssgBox,&mssg, 0);

     }

// 	   lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

// 	//     /*Create a white label, set its text and align it to the center*/
// 	//     lv_obj_t * label = lv_label_create(lv_screen_active());

// 	//     lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);

// 	//     lv_example_style_1();

// 	// //     /*Change the active screen's background color*/
// 	// //     lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x0D47A1), LV_PART_MAIN);

// 	    /*Create a white label, set its text and align it to the center*/

// 	    lv_obj_t * obj = lv_obj_create(lv_screen_active());
// //	   lv_obj_add_style(obj, &style, 0);

// 	  lv_obj_t * label = lv_label_create(obj);
// 	// //     lv_obj_t * label = lv_label_create(lv_screen_active());

// 	     lv_label_set_text(label, "Hello world");

// 	     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

// 	    // /*Change the active screen's background color*/
// 	    // lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

// 	    // /*Create a white label, set its text and align it to the center*/
// 	    // lv_obj_t * label = lv_label_create(lv_screen_active());
// 	    // lv_label_set_text(label, "Hello world");
// 	    // lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
// 	    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

// 	     while (1)
// 	    {
// 	       vTaskDelay(pdMS_TO_TICKS(10));
// 	        if ( presc++ > 100) {
// 	            presc = 0;
// 	            sprintf(str, "Cntr: %d",cntr++);
// 	            lv_label_set_text(label, str);
// 	        }
// 	        lv_timer_handler();
// 	    }
}
