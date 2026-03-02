/*******************************************************************************
 * Size: NaN px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef DEJAVUSANSMONO50
#define DEJAVUSANSMONO50 1
#endif
#include "math.h"
#define NaN NAN

#if DEJAVUSANSMONO50

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0030 "0" */
    0x0,

    /* U+0031 "1" */
    0x0,

    /* U+0032 "2" */
    0x0,

    /* U+0033 "3" */
    0x0,

    /* U+0034 "4" */
    0x0,

    /* U+0035 "5" */
    0x0,

    /* U+0036 "6" */
    0x0,

    /* U+0037 "7" */
    0x0,

    /* U+0038 "8" */
    0x0,

    /* U+0039 "9" */
    0x0,

    /* U+003A ":" */
    0x0,

    /* U+003B ";" */
    0x0, 0x0,

    /* U+003C "<" */
    0x0,

    /* U+003D "=" */
    0x0,

    /* U+003E ">" */
    0x0,

    /* U+003F "?" */
    0x0,

    /* U+0040 "@" */
    0x0, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 6, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 7, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 8, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 9, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 10, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 11, .adv_w = 10, .box_w = 6, .box_h = 2, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 13, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 14, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 15, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 16, .adv_w = 10, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 17, .adv_w = 10, .box_w = 6, .box_h = 2, .ofs_x = -1, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 48, .range_length = 17, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t dejaVuSansMono50 = {
#else
lv_font_t dejaVuSansMono50 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 3,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_HOR,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = NaN,
    .underline_thickness = NaN,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if DEJAVUSANSMONO50*/

