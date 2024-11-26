#pragma once

#include "lvgl_port_v8.h"
#include <lvgl.h>
#include <Arduino.h>

extern void lvglTabIcons(lv_obj_t* parent);
extern void backgroundImage(lv_obj_t* parent);
extern void statusBar(lv_obj_t* parent);
extern void mainContainer(lv_obj_t* parent);

extern lv_style_t mainContainerBorder;
extern lv_obj_t* mainContainerObj;

extern lv_timer_t* statusBarTimer;
