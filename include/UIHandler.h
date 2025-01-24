#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include "UIScreens.h"

// Screen type enum


// Tab object declarations
extern lv_obj_t* tabHome;
extern lv_obj_t* tabMining;
extern lv_obj_t* tabActivity;
extern lv_obj_t* tabBitcoinNews;
extern lv_obj_t* tabSettings;

<<<<<<< Updated upstream
=======
// Status Bar Objects
extern lv_obj_t* statusBarObj;



>>>>>>> Stashed changes
// Current active screen
extern ScreenType activeScreen;

extern void switchToScreen(ScreenType newScreen);
extern void tabIconEventHandler(lv_event_t* tabEvent);