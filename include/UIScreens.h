#pragma once

#include <lvgl.h>

#define SMOOTHING_WINDOW_SIZE 5

extern const lv_img_dsc_t Logos;
extern const lv_img_dsc_t Splash;
extern const lv_img_dsc_t UIBackground;
extern const lv_img_dsc_t openSourceBitcoinMining;
extern const lv_img_dsc_t bitcoin;
extern const lv_img_dsc_t activity;
extern const lv_img_dsc_t house;
extern const lv_img_dsc_t pickaxe;
extern const lv_img_dsc_t statusbar;
extern const lv_img_dsc_t bitcoin20by20;
extern const lv_img_dsc_t plugZap20by20;
extern const lv_img_dsc_t cpu20by20;
extern const lv_img_dsc_t wifi20by20;

extern lv_obj_t* tabHome;
extern lv_obj_t* tabMining;
extern lv_obj_t* tabActivity;
extern lv_obj_t* tabBitcoinNews;
extern lv_obj_t* tabSettings;

enum ScreenType 
{
    activeScreenSplash,
    activeScreenHome,
    activeScreenMining,
    activeScreenActivity,
    activeScreenBitcoinNews,
    activeScreenSettings
};

struct ScreenObjects 
{
    lv_obj_t* background;
    lv_obj_t* statusBar;
    lv_obj_t* tabIcons;
    lv_obj_t* homeMainContainer;
    lv_obj_t* miningMainContainer;
    lv_obj_t* activityMainContainer;
    lv_obj_t* bitcoinNewsMainContainer;
    lv_timer_t* labelUpdateTimer;
    lv_timer_t* chartUpdateTimer;
    lv_timer_t* statusBarUpdateTimer;
    lv_timer_t* clockTimer;
};

extern ScreenObjects screenObjs;

// global variables
extern ScreenType activeScreen;
extern lv_timer_t* chartUpdateTimer;
extern lv_timer_t* networkUpdateTimer;

extern float calculateMovingAverage(float newValue);
extern void hashrateGraph(lv_obj_t* parent);

// graph buffer
extern float hashrateBuffer[SMOOTHING_WINDOW_SIZE];
extern int bufferIndex;

extern void splashScreen();
extern void homeScreen();
extern void miningStatusScreen();
extern void initalizeOneScreen();
extern void bitcoinNewsScreen();
extern void activityScreen();
