/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "MainScreen.h"
#include "MeasDisplay.h"
#include "backGround.h"
#include "fonts.h"
#include "guiCommonTask.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PADDING 8
#define ITEMHEIGHT 98
#define ITEMWIDTH 200
#define FIRSTY 0

// test ASCII , no ut8 in lvgl config

static const char *units[] = { "\xb5" "g/m" "\xb3","\xb5" "g/m" "\xb3","\xb5" "g/m" "\xb3","\xb5" "g/m" "\xb3" }; // µg/m³"
//static const char *units[] = {"", "", "", ""};
static const char *formats[] = {"%2.1f", "%2.1f", "%2.1f", "%2.1f"};
static const char *name[] = {{"PM1:"}, {"PM2.5:"}, {"PM4:"}, {"PM10:"}}; 
const char xx[] = { "\xb5" "g/m" "\xb3"};

MainScreen::MainScreen() {
	screen = lv_obj_create(NULL);

	backGround = makeBackGround(screen);
	vTaskDelay( 100);

	for (int n = 0; n < NR_ITEMS; n++) {
		measDisplay[n] = new MeasDisplay(backGround, FIRSTY + n * (ITEMHEIGHT + PADDING) + PADDING, name[n], units[n], formats[n]);
		measDisplay[n]->setText("....");
	}
	statusLine = new StatusLine(backGround);
	statusLine->setText(NULL);
}

void MainScreen::update(float *values) {
	for (int n = 0; n < NR_ITEMS; n++) {
		measDisplay[n]->setValue(*values++);
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
