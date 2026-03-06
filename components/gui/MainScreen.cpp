/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "MainScreen.h"
#include "backGround.h"
#include "fonts.h"
#include "guiCommonTask.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LINEHEIGHT 35

const char measLabelTxt[][10] = {{"mc 1p0"}, {"mc 2p5"}, {"mc 4p0"}, {"mc 10p0"}, {"nc 0p5"},
								 {"nc 1p0"}, {"nc 2p5"}, {"nc 4p0"}, {"nc 10p0"}, {"typ size"}};

static lv_coord_t column_dsc[] = {140, 60, LV_GRID_TEMPLATE_LAST}; /* 2 columns with 100- and 400-px width */
static lv_coord_t row_dsc[] = {LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LINEHEIGHT,
							   LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LINEHEIGHT, LV_GRID_TEMPLATE_LAST};

#define NR_VALUES ((sizeof(row_dsc) / sizeof(lv_coord_t)) - 1)

static lv_obj_t *valueLabel[NR_VALUES];

MainScreen::MainScreen() {
	int line = 0;
	screen = lv_obj_create(NULL);

	backGround = makeBackGround(screen);

	/*Create a container with grid*/
	lv_obj_t *cont = lv_obj_create(backGround);
	lv_obj_set_style_grid_column_dsc_array(cont, column_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
	lv_obj_set_size(cont, LV_HOR_RES_MAX - 20, LV_VER_RES_MAX - 27);
	// lv_obj_center(cont);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	lv_obj_add_style(cont,&style_background,0);

	lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
		

	lv_obj_t *label;
	lv_obj_t *obj;

	for (line = 0; line < NR_VALUES; line++) { // rows
		obj = lv_obj_create(cont);
		lv_obj_add_style(obj, &styleMeasName, 0);
		/*Stretch the cell horizontally and vertically too
		 *Set span to 1 to make the cell 1 column/row sized*/
		lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, line, 1);

		label = lv_label_create(obj);

		lv_label_set_text(label, measLabelTxt[line]);
		lv_obj_center(label);

		obj = lv_obj_create(cont);
		lv_obj_add_style(obj, &styleValue, 0);

		/*Stretch the cell horizontally and vertically too
		 *Set span to 1 to make the cell 1 column/row sized*/
		// lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, line, 1);
		lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_STRETCH, line, 1);
		label = lv_label_create(obj);
		valueLabel[line] = label;

		lv_label_set_text(label, "--");
		lv_obj_center(label);
	}
	statusLine = new StatusLine(backGround);
	statusLine->setText(NULL);

	//	navigArrows = new NavigArrows(backGround, true, true);
}

// void MainScreen::setValues(mainScreenVars_t *p){
//	vars = *p;
//	update();
//
// }
//
// void MainScreen::getValues(mainScreenVars_t *p){
//	*p = vars;
// }

void MainScreen::update(float *values) {

	char str[20];
	for (int n = 0; n < NR_VALUES; n++) {
		if (n < (NR_VALUES - 1))
			sprintf(str, "%3.1f", *values++);
		else
			sprintf(str, "%3.2f", *values++); // typ_size
		// lv_label_set_text_fmt(valueLabel[n],"%3.1f", *values);
		lv_label_set_text(valueLabel[n], str);
	}
}

void MainScreen::setStatusLine(const char *text) {
	if (strlen(text) > 0) {
		statusLine->setText(text);
	} else
		statusLine->setText(NULL);
}

void MainScreen::show() { lv_scr_load(screen); }

MainScreen::~MainScreen() {
	// TODO Auto-generated destructor stub
}
