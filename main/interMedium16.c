/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --font Inter_18pt-Medium.ttf --symbols ° --range 32-127 --format lvgl -o interMedium16.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include <lvgl.h>
#endif

#ifndef INTERMEDIUM16
#define INTERMEDIUM16 1
#endif

#if INTERMEDIUM16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xf,

    /* U+0022 "\"" */
    0xde, 0xf7, 0xb0,

    /* U+0023 "#" */
    0x19, 0x84, 0x61, 0x11, 0xff, 0x11, 0xc, 0xc3,
    0x30, 0x8c, 0xff, 0x88, 0x82, 0x21, 0x88,

    /* U+0024 "$" */
    0x4, 0x2, 0x7, 0x86, 0xf6, 0x5b, 0x21, 0xf0,
    0x7c, 0x1f, 0x3, 0xc1, 0x6c, 0xb6, 0x71, 0xf0,
    0x10, 0x8,

    /* U+0025 "%" */
    0x70, 0x46, 0xc2, 0x36, 0x21, 0xb2, 0x7, 0x10,
    0x1, 0x0, 0x13, 0xc0, 0xb3, 0x9, 0x98, 0x8c,
    0xc4, 0x66, 0x41, 0xe0,

    /* U+0026 "&" */
    0x3c, 0x19, 0x86, 0x61, 0xb8, 0x7c, 0xe, 0x7,
    0xc3, 0xb6, 0xc7, 0xb1, 0xc6, 0x78, 0xf6,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x36, 0x66, 0xcc, 0xcc, 0xcc, 0xc6, 0x66, 0x30,

    /* U+0029 ")" */
    0xc6, 0x66, 0x33, 0x33, 0x33, 0x36, 0x66, 0xc0,

    /* U+002A "*" */
    0x10, 0xa9, 0xf1, 0xe5, 0x42, 0x0,

    /* U+002B "+" */
    0x18, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18, 0x18,

    /* U+002C "," */
    0xfe,

    /* U+002D "-" */
    0xf8,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0xc, 0x21, 0x86, 0x18, 0x43, 0xc, 0x20, 0x86,
    0x18, 0x41, 0xc, 0x0,

    /* U+0030 "0" */
    0x3e, 0x31, 0x98, 0xd8, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xb1, 0x98, 0xc7, 0xc0,

    /* U+0031 "1" */
    0x3b, 0xf6, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x30,

    /* U+0032 "2" */
    0x3c, 0x66, 0xc3, 0x3, 0x3, 0x7, 0xe, 0x1c,
    0x38, 0x70, 0x60, 0xff,

    /* U+0033 "3" */
    0x3c, 0xe7, 0xc3, 0x3, 0x6, 0x1c, 0x6, 0x3,
    0x3, 0xc3, 0x66, 0x3c,

    /* U+0034 "4" */
    0x7, 0x1, 0xc0, 0xf0, 0x6c, 0x13, 0xc, 0xc6,
    0x31, 0x8c, 0xff, 0xc0, 0xc0, 0x30, 0xc,

    /* U+0035 "5" */
    0x7e, 0x40, 0xc0, 0xc0, 0xfc, 0xc6, 0x3, 0x3,
    0x3, 0xc3, 0x46, 0x3c,

    /* U+0036 "6" */
    0x3c, 0x66, 0x43, 0xc0, 0xdc, 0xe6, 0xc3, 0xc3,
    0xc3, 0x43, 0x66, 0x3c,

    /* U+0037 "7" */
    0xff, 0x3, 0x6, 0x6, 0xc, 0xc, 0x18, 0x18,
    0x30, 0x30, 0x20, 0x60,

    /* U+0038 "8" */
    0x3c, 0xe7, 0xc3, 0xc3, 0x66, 0x3c, 0x66, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+0039 "9" */
    0x3c, 0x66, 0xc2, 0xc3, 0xc3, 0x67, 0x3b, 0x3,
    0xc3, 0xc6, 0x66, 0x3c,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0xf0, 0x3, 0xa8,

    /* U+003C "<" */
    0x3, 0xf, 0x3c, 0xf0, 0xc0, 0x78, 0x1e, 0x7,
    0x1,

    /* U+003D "=" */
    0xfe, 0x0, 0x0, 0xf, 0xe0,

    /* U+003E ">" */
    0x80, 0xe0, 0x38, 0xe, 0x7, 0x1c, 0x70, 0xc0,
    0x0,

    /* U+003F "?" */
    0x7d, 0x8f, 0x18, 0x30, 0xe3, 0x8e, 0x18, 0x30,
    0x0, 0xc1, 0x80,

    /* U+0040 "@" */
    0xf, 0xc0, 0xe1, 0xc3, 0x1, 0x98, 0x6, 0xc7,
    0xef, 0x33, 0xbd, 0x86, 0xf6, 0x1b, 0xd8, 0x6f,
    0x61, 0xbc, 0xce, 0x99, 0xee, 0x70, 0x0, 0xe0,
    0x0, 0xfe, 0x0,

    /* U+0041 "A" */
    0xc, 0x7, 0x81, 0xe0, 0x48, 0x33, 0xc, 0xc2,
    0x31, 0x86, 0x7f, 0x90, 0x2c, 0xf, 0x3,

    /* U+0042 "B" */
    0xfe, 0x63, 0xb0, 0xd8, 0x6c, 0x67, 0xe3, 0xd,
    0x83, 0xc1, 0xe0, 0xf0, 0xdf, 0xc0,

    /* U+0043 "C" */
    0x1f, 0xc, 0x66, 0xf, 0x1, 0xc0, 0x30, 0xc,
    0x3, 0x0, 0xc0, 0x58, 0x33, 0x18, 0x7c,

    /* U+0044 "D" */
    0xfe, 0x30, 0xcc, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xf0, 0x6c, 0x33, 0xf8,

    /* U+0045 "E" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xff,

    /* U+0046 "F" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0x1e, 0xc, 0x66, 0x1f, 0x3, 0xc0, 0x30, 0xc,
    0x7f, 0x3, 0xc0, 0xd8, 0x73, 0x18, 0x78,

    /* U+0048 "H" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1f, 0xff, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x30,

    /* U+0049 "I" */
    0xff, 0xff, 0xff,

    /* U+004A "J" */
    0x6, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x83, 0xc7,
    0x8f, 0x13, 0xe0,

    /* U+004B "K" */
    0xc1, 0xb0, 0xcc, 0x63, 0x30, 0xd8, 0x3e, 0xf,
    0xc3, 0xb0, 0xc6, 0x30, 0xcc, 0x33, 0x6,

    /* U+004C "L" */
    0xc1, 0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc1,
    0x83, 0x7, 0xf0,

    /* U+004D "M" */
    0xe0, 0x7e, 0x7, 0xf0, 0xff, 0xf, 0xd0, 0xbd,
    0x9b, 0xd9, 0xbc, 0x93, 0xcf, 0x3c, 0xf3, 0xc6,
    0x3c, 0x63,

    /* U+004E "N" */
    0xe0, 0xf8, 0x3f, 0xf, 0x43, 0xd8, 0xf3, 0x3c,
    0xcf, 0x1b, 0xc2, 0xf0, 0xfc, 0x1f, 0x7,

    /* U+004F "O" */
    0x1f, 0x6, 0x31, 0x83, 0x60, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x6c, 0x18, 0xc6, 0xf,
    0x80,

    /* U+0050 "P" */
    0xfc, 0xc6, 0xc3, 0xc3, 0xc3, 0xc6, 0xfc, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0051 "Q" */
    0x1f, 0x6, 0x31, 0x83, 0x60, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc6, 0x6c, 0x78, 0xc6, 0xf,
    0xc0, 0xc,

    /* U+0052 "R" */
    0xfc, 0xc6, 0xc3, 0xc3, 0xc3, 0xc6, 0xfc, 0xcc,
    0xc4, 0xc6, 0xc7, 0xc3,

    /* U+0053 "S" */
    0x1e, 0x19, 0xd8, 0x6c, 0x7, 0x81, 0xf0, 0x7c,
    0x7, 0x1, 0xb0, 0xd8, 0xc7, 0xc0,

    /* U+0054 "T" */
    0xff, 0x86, 0x3, 0x1, 0x80, 0xc0, 0x60, 0x30,
    0x18, 0xc, 0x6, 0x3, 0x1, 0x80,

    /* U+0055 "U" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xd8, 0xc7, 0xc0,

    /* U+0056 "V" */
    0x40, 0x6c, 0x19, 0x83, 0x10, 0x63, 0x18, 0x63,
    0x4, 0x60, 0xd8, 0x1b, 0x1, 0x60, 0x38, 0x7,
    0x0,

    /* U+0057 "W" */
    0xc1, 0x82, 0x61, 0x86, 0x63, 0xc6, 0x63, 0xc6,
    0x22, 0xc4, 0x32, 0x4c, 0x36, 0x6c, 0x36, 0x6c,
    0x14, 0x68, 0x14, 0x38, 0x1c, 0x38, 0x1c, 0x38,

    /* U+0058 "X" */
    0x60, 0xc6, 0x30, 0xc6, 0xd, 0x80, 0xa0, 0x1c,
    0x3, 0x80, 0xd8, 0x1b, 0x6, 0x31, 0x83, 0x30,
    0x60,

    /* U+0059 "Y" */
    0xc0, 0xd8, 0x66, 0x18, 0xcc, 0x33, 0x7, 0x80,
    0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30,

    /* U+005A "Z" */
    0xff, 0x3, 0x6, 0x4, 0xc, 0x18, 0x18, 0x30,
    0x20, 0x60, 0xc0, 0xff,

    /* U+005B "[" */
    0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xf0,

    /* U+005C "\\" */
    0xc1, 0x4, 0x18, 0x60, 0x82, 0xc, 0x30, 0x41,
    0x6, 0x18, 0x20, 0x80,

    /* U+005D "]" */
    0xf3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xf0,

    /* U+005E "^" */
    0x31, 0xc5, 0xb2, 0xce, 0x30,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0xcc,

    /* U+0061 "a" */
    0x3d, 0xcc, 0x8, 0xf7, 0xfc, 0xf1, 0xe7, 0x7e,

    /* U+0062 "b" */
    0xc0, 0xc0, 0xc0, 0xdc, 0xe6, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xe6, 0xdc,

    /* U+0063 "c" */
    0x3c, 0x63, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x63,
    0x3c,

    /* U+0064 "d" */
    0x3, 0x3, 0x3, 0x3b, 0x67, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x67, 0x3b,

    /* U+0065 "e" */
    0x3c, 0x66, 0xc3, 0xc3, 0xff, 0xc0, 0xc0, 0x67,
    0x3c,

    /* U+0066 "f" */
    0x1c, 0xc3, 0x3f, 0x30, 0xc3, 0xc, 0x30, 0xc3,
    0xc,

    /* U+0067 "g" */
    0x3b, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x67,
    0x3b, 0x3, 0xe3, 0x3c,

    /* U+0068 "h" */
    0xc1, 0x83, 0x6, 0xee, 0x78, 0xf1, 0xe3, 0xc7,
    0x8f, 0x1e, 0x30,

    /* U+0069 "i" */
    0xf3, 0xff, 0xff,

    /* U+006A "j" */
    0x33, 0x3, 0x33, 0x33, 0x33, 0x33, 0x33, 0xe0,

    /* U+006B "k" */
    0xc0, 0xc0, 0xc0, 0xc6, 0xcc, 0xd8, 0xd8, 0xf0,
    0xd8, 0xcc, 0xc6, 0xc7,

    /* U+006C "l" */
    0xff, 0xff, 0xff,

    /* U+006D "m" */
    0xdd, 0xee, 0x63, 0xc6, 0x3c, 0x63, 0xc6, 0x3c,
    0x63, 0xc6, 0x3c, 0x63, 0xc6, 0x30,

    /* U+006E "n" */
    0xdd, 0xcf, 0x1e, 0x3c, 0x78, 0xf1, 0xe3, 0xc6,

    /* U+006F "o" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66,
    0x3c,

    /* U+0070 "p" */
    0xdc, 0xe6, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe6,
    0xdc, 0xc0, 0xc0, 0xc0,

    /* U+0071 "q" */
    0x3b, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x67,
    0x3b, 0x3, 0x3, 0x3,

    /* U+0072 "r" */
    0xff, 0x31, 0x8c, 0x63, 0x18, 0xc0,

    /* U+0073 "s" */
    0x7d, 0x8f, 0x7, 0x87, 0xc3, 0xd1, 0xe3, 0x7c,

    /* U+0074 "t" */
    0x63, 0x3e, 0xc6, 0x31, 0x8c, 0x63, 0xe,

    /* U+0075 "u" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xe7, 0x76,

    /* U+0076 "v" */
    0x41, 0xb1, 0x98, 0xc4, 0x43, 0x61, 0xb0, 0x50,
    0x38, 0x1c, 0x0,

    /* U+0077 "w" */
    0x43, 0x1b, 0x39, 0x99, 0x4c, 0xca, 0x62, 0x5a,
    0x1e, 0xf0, 0xe3, 0x83, 0x1c, 0x18, 0xc0,

    /* U+0078 "x" */
    0xc6, 0xd9, 0xb1, 0xc3, 0x5, 0x1b, 0x22, 0xc6,

    /* U+0079 "y" */
    0x41, 0xb1, 0x98, 0xc4, 0x43, 0x61, 0xb0, 0x50,
    0x38, 0x8, 0xc, 0x1e, 0xe, 0x0,

    /* U+007A "z" */
    0xfe, 0x1c, 0x30, 0xc3, 0x86, 0x18, 0x70, 0xfe,

    /* U+007B "{" */
    0x19, 0x8c, 0x63, 0x18, 0xdc, 0x31, 0x8c, 0x63,
    0xc,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff, 0xff,

    /* U+007D "}" */
    0xe1, 0x8c, 0x63, 0x18, 0xc3, 0x31, 0x8c, 0x63,
    0x70,

    /* U+007E "~" */
    0x63, 0xd3, 0xce,

    /* U+00B0 "°" */
    0x7b, 0x3c, 0xf3, 0x78
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 67, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 74, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 123, .box_w = 5, .box_h = 4, .ofs_x = 2, .ofs_y = 8},
    {.bitmap_index = 7, .adv_w = 162, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 22, .adv_w = 164, .box_w = 9, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 40, .adv_w = 248, .box_w = 13, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 60, .adv_w = 165, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 75, .adv_w = 77, .box_w = 2, .box_h = 4, .ofs_x = 2, .ofs_y = 8},
    {.bitmap_index = 76, .adv_w = 91, .box_w = 4, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 84, .adv_w = 91, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 92, .adv_w = 132, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 98, .adv_w = 169, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 106, .adv_w = 73, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 107, .adv_w = 117, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 108, .adv_w = 73, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 93, .box_w = 6, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 121, .adv_w = 164, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 104, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 155, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 159, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 167, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 194, .adv_w = 159, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 206, .adv_w = 144, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 218, .adv_w = 159, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 230, .adv_w = 159, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 242, .adv_w = 73, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 76, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 248, .adv_w = 169, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 169, .box_w = 7, .box_h = 5, .ofs_x = 2, .ofs_y = 2},
    {.bitmap_index = 262, .adv_w = 169, .box_w = 8, .box_h = 9, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 271, .adv_w = 136, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 282, .adv_w = 252, .box_w = 14, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 309, .adv_w = 180, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 167, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 187, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 353, .adv_w = 183, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 368, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 380, .adv_w = 149, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 392, .adv_w = 191, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 407, .adv_w = 189, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 421, .adv_w = 68, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 424, .adv_w = 146, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 435, .adv_w = 174, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 450, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 461, .adv_w = 231, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 479, .adv_w = 191, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 494, .adv_w = 195, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 511, .adv_w = 163, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 523, .adv_w = 196, .box_w = 11, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 541, .adv_w = 165, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 553, .adv_w = 164, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 567, .adv_w = 165, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 581, .adv_w = 187, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 595, .adv_w = 179, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 612, .adv_w = 255, .box_w = 16, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 636, .adv_w = 177, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 653, .adv_w = 176, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 668, .adv_w = 163, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 680, .adv_w = 91, .box_w = 4, .box_h = 15, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 688, .adv_w = 93, .box_w = 6, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 700, .adv_w = 91, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 708, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 713, .adv_w = 118, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 714, .adv_w = 82, .box_w = 3, .box_h = 2, .ofs_x = 1, .ofs_y = 10},
    {.bitmap_index = 715, .adv_w = 143, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 723, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 735, .adv_w = 145, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 744, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 756, .adv_w = 148, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 765, .adv_w = 94, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 774, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 786, .adv_w = 152, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 797, .adv_w = 63, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 800, .adv_w = 63, .box_w = 4, .box_h = 15, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 808, .adv_w = 141, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 820, .adv_w = 63, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 225, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 837, .adv_w = 152, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 845, .adv_w = 152, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 854, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 866, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 878, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 884, .adv_w = 135, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 892, .adv_w = 87, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 899, .adv_w = 152, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 907, .adv_w = 144, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 918, .adv_w = 209, .box_w = 13, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 933, .adv_w = 140, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 941, .adv_w = 145, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 955, .adv_w = 139, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 963, .adv_w = 111, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 972, .adv_w = 87, .box_w = 2, .box_h = 20, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 977, .adv_w = 111, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 986, .adv_w = 169, .box_w = 8, .box_h = 3, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 989, .adv_w = 115, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 7}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 1, .glyph_id_start = 96,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    3, 7,
    3, 13,
    3, 15,
    3, 21,
    5, 24,
    7, 3,
    7, 8,
    7, 61,
    8, 7,
    8, 13,
    8, 15,
    8, 21,
    9, 93,
    11, 7,
    11, 13,
    11, 15,
    11, 21,
    11, 33,
    11, 64,
    12, 19,
    12, 20,
    12, 24,
    12, 61,
    13, 1,
    13, 2,
    13, 3,
    13, 8,
    13, 17,
    13, 18,
    13, 19,
    13, 20,
    13, 22,
    13, 23,
    13, 24,
    13, 25,
    13, 26,
    13, 32,
    13, 33,
    14, 19,
    14, 20,
    14, 24,
    14, 61,
    15, 1,
    15, 2,
    15, 3,
    15, 8,
    15, 17,
    15, 18,
    15, 19,
    15, 20,
    15, 22,
    15, 23,
    15, 24,
    15, 25,
    15, 26,
    15, 32,
    15, 33,
    16, 13,
    16, 15,
    16, 61,
    17, 13,
    17, 15,
    17, 24,
    17, 61,
    17, 64,
    18, 10,
    18, 62,
    18, 64,
    18, 94,
    19, 21,
    20, 11,
    20, 13,
    20, 15,
    20, 63,
    20, 96,
    21, 11,
    21, 13,
    21, 15,
    21, 18,
    21, 24,
    21, 63,
    21, 96,
    22, 11,
    22, 13,
    22, 15,
    22, 18,
    22, 63,
    22, 96,
    23, 13,
    23, 15,
    23, 64,
    24, 4,
    24, 6,
    24, 7,
    24, 11,
    24, 13,
    24, 15,
    24, 17,
    24, 20,
    24, 21,
    24, 22,
    24, 23,
    24, 24,
    24, 25,
    24, 26,
    24, 27,
    24, 28,
    24, 29,
    24, 63,
    24, 64,
    24, 96,
    25, 11,
    25, 13,
    25, 15,
    25, 63,
    25, 96,
    26, 13,
    26, 15,
    26, 24,
    26, 61,
    26, 64,
    27, 61,
    28, 61,
    30, 61,
    31, 24,
    31, 61,
    33, 13,
    33, 15,
    33, 16,
    33, 61,
    33, 64,
    60, 93,
    61, 3,
    61, 5,
    61, 8,
    61, 11,
    61, 12,
    61, 14,
    61, 16,
    61, 17,
    61, 18,
    61, 23,
    61, 29,
    61, 30,
    61, 32,
    61, 33,
    61, 61,
    61, 63,
    61, 95,
    61, 96,
    63, 7,
    63, 13,
    63, 15,
    63, 21,
    63, 33,
    63, 64,
    64, 11,
    64, 17,
    64, 18,
    64, 20,
    64, 21,
    64, 22,
    64, 23,
    64, 25,
    64, 26,
    64, 33,
    64, 61,
    64, 63,
    64, 93,
    64, 96,
    92, 93,
    93, 10,
    93, 62,
    93, 64,
    93, 94,
    95, 19,
    95, 20,
    95, 24,
    95, 61,
    96, 7,
    96, 13,
    96, 15,
    96, 21,
    96, 33,
    96, 64
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -9, -22, -22, -16, 0, -9, -9, -16,
    -9, -22, -22, -16, 1, -9, -35, -35,
    -12, -3, -17, -8, -2, -6, -12, -3,
    -1, -22, -22, -7, -23, 0, -6, -3,
    -7, -5, -5, -2, -25, -11, -8, -2,
    -6, -12, -3, -1, -22, -22, -7, -23,
    0, -6, -3, -7, -5, -5, -2, -25,
    -11, -10, -10, 1, -7, -7, -5, -5,
    -12, 1, 1, 1, 1, -4, -4, -5,
    -5, -4, -4, -8, -9, -9, -8, -2,
    -8, -8, -1, -6, -6, -1, -1, -1,
    -8, -8, -12, -14, 1, -12, 1, -32,
    -32, -4, -4, -15, -2, -4, 5, -4,
    -2, -9, -9, -23, 1, -41, 1, -4,
    -5, -5, -4, -4, -7, -7, -5, -5,
    -12, -16, -16, -17, -19, -19, -11, -11,
    -10, -9, -10, 1, -20, -1, -20, -22,
    -9, -9, 4, -1, -13, -1, -3, -13,
    -16, -9, -13, -22, -9, -22, -9, -35,
    -35, -12, -3, -17, -17, -12, -28, -12,
    -14, -12, -12, -12, -12, -10, -20, -17,
    7, -17, 1, 1, 1, 1, 1, -8,
    -2, -6, -12, -9, -35, -35, -12, -3,
    -17
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 185,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t interMedium16 = {
#else
lv_font_t interMedium16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 20,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if INTERMEDIUM16*/

