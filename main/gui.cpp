

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>

#include "gui.h"

#include <stdio.h>

void initializeLCD();

static lv_style_t style1;
static char measLabelTxt[][10] = {{"mc 1p0"}, {"mc 2p5"},  {"mc 4p0"},	{"mc 10p0"},  {"nc 0p5"},
								  {"nc 1p0"}, {"ncc 2p5"}, {"ncc 4p0"}, {"ncc 1-p0"}, {"typ size"}};

static int LINEHEIGHT = 30;

// uint16_t mc_1p0 = 0;
// uint16_t mc_2p5 = 0;
// uint16_t mc_4p0 = 0;
// uint16_t mc_10p0 = 0;
// uint16_t nc_0p5 = 0;
// uint16_t nc_1p0 = 0;
// uint16_t nc_2p5 = 0;
// uint16_t nc_4p0 = 0;
// uint16_t nc_10p0 = 0;
// uint16_t typical_particle_size = 0;

static int32_t column_dsc[] = {150, 100, LV_GRID_TEMPLATE_LAST}; /* 2 columns with 100- and 400-px width */
static int32_t row_dsc[] = {LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LINEHEIGHT,			  LINEHEIGHT,
							LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LV_GRID_TEMPLATE_LAST};

static lv_obj_t *valueLabel[NR_VALUES];
static bool isInitialized;

QueueHandle_t displayMssgBox;
QueueHandle_t displayReadyMssgBox;


// void initMeasureScreen(void) {
// 	static int32_t col_dsc[] = {70, 70, 70, LV_GRID_TEMPLATE_LAST};
// 	static int32_t row_dsc[] = {50, 50, 50, LV_GRID_TEMPLATE_LAST};

// 	/*Create a container with grid*/
// 	lv_obj_t *cont = lv_obj_create(lv_screen_active());
// 	lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
// 	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
// 	lv_obj_set_size(cont, 300, 220);
// 	lv_obj_center(cont);
// 	lv_obj_set_layout(cont, LV_LAYOUT_GRID);

// 	lv_obj_t *label;
// 	lv_obj_t *obj;

// 	uint8_t i;
// 	for (i = 0; i < 9; i++) {
// 		uint8_t col = i % 3;
// 		uint8_t row = i / 3;

// 		obj = lv_button_create(cont);
// 		/*Stretch the cell horizontally and vertically too
// 		 *Set span to 1 to make the cell 1 column/row sized*/
// 		lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);

// 		label = lv_label_create(obj);
// 		lv_label_set_text_fmt(label, "c%d, r%d", col, row);
// 		lv_obj_center(label);
// 	}
// }

void initMeasureScreen(void) {
	int line = 0;
	lv_style_init(&style1);
	lv_style_set_radius(&style1, 5);
	lv_style_set_width(&style1, 150);
	lv_style_set_height(&style1, LV_SIZE_CONTENT);

	lv_style_set_pad_ver(&style1, 20);
	lv_style_set_pad_left(&style1, 5);

	lv_style_set_x(&style1, lv_pct(50));
	lv_style_set_y(&style1, 80);
	lv_style_set_text_font(&style1, &lv_font_montserrat_22);

	/*Create a container with grid*/
	lv_obj_t *cont = lv_obj_create(lv_screen_active());
	lv_obj_set_style_grid_column_dsc_array(cont, column_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
	lv_obj_set_size(cont, 320, 440);
	// lv_obj_center(cont);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);

	lv_obj_t *label;
	lv_obj_t *obj;

	for (line = 0; line < NR_VALUES; line++) { // rows

		obj = lv_obj_create(cont);
		lv_obj_add_style(obj, &style1, 0);
		/*Stretch the cell horizontally and vertically too
		 *Set span to 1 to make the cell 1 column/row sized*/
		lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, line, 1);

		label = lv_label_create(obj);

		lv_label_set_text_fmt(label, measLabelTxt[line]);
		lv_obj_center(label);

		obj = lv_obj_create(cont);
		lv_obj_add_style(obj, &style1, 0);
	
		/*Stretch the cell horizontally and vertically too
		 *Set span to 1 to make the cell 1 column/row sized*/
		lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, line, 1);

		label = lv_label_create(obj);
		valueLabel[line] = label;

		lv_label_set_text_fmt(label, "--");
		lv_obj_center(label);
	}
}

void updateValues(uint16_t *values) {
	if (isInitialized) {
		for (int n = 0; n < NR_VALUES; n++) {
			lv_label_set_text_fmt(valueLabel[n],"%d", *values);
		//	lv_label_set_text_fmt(valueLabel[n], "hallo"); // *values);
			values++;
		}
	}

}

void guiTask(void *pvParameter) {
    displayMssg_t recDdisplayMssg;
   	displayMssgBox = xQueueCreate(5, sizeof(displayMssg_t));
	displayReadyMssgBox = xQueueCreate(1, sizeof(uint32_t));


	initializeLCD();

	initMeasureScreen();
	isInitialized = true;

	while (1) {
		vTaskDelay(10 / portTICK_PERIOD_MS);
		lv_timer_handler();
        if (xQueueReceive(displayMssgBox, &recDdisplayMssg, 0) == pdTRUE) {
            updateValues(recDdisplayMssg.values);
        }

	}
}
