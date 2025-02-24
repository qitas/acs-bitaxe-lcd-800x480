#include "UIHandler.h"
#include "lvgl_port_v8.h"
#include <lvgl.h>
#include <Arduino.h>
#include "UIScreens.h"

extern bool settingsChanged;

ScreenObjects screenObjs = {
    .background = nullptr,
    .homeMainContainer = nullptr,
    .miningMainContainer = nullptr,
    .activityMainContainer = nullptr,
    .bitcoinNewsMainContainer = nullptr,
    .settingsMainContainer = nullptr,
    .labelUpdateTimer = nullptr,
    .chartUpdateTimer = nullptr,
    .statusBarUpdateTimer = nullptr,
    .clockTimer = nullptr,
    .apiUpdateTimer = nullptr,
    .autoTuneSettingsTimer = nullptr,
    
};


void switchToScreen(ScreenType newScreen)
{
    if (activeScreen == newScreen) {
        return; // Already on this screen
    }
  //  if (settingsChanged) {
    //    showSettingsConfirmationOverlay();
   //     return;
  // }

    lvgl_port_lock(-1);  // Lock for thread safety

    // Hide all containers first
    lv_obj_add_flag(screenObjs.homeMainContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screenObjs.miningMainContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screenObjs.activityMainContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screenObjs.bitcoinNewsMainContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screenObjs.settingsMainContainer, LV_OBJ_FLAG_HIDDEN);
    Serial.println("Settings Container Hidden");
    // Pause all active timers
    if (screenObjs.labelUpdateTimer) lv_timer_pause(screenObjs.labelUpdateTimer);
    if (screenObjs.chartUpdateTimer) lv_timer_pause(screenObjs.chartUpdateTimer);
    if (screenObjs.clockTimer) lv_timer_pause(screenObjs.clockTimer);
    if (screenObjs.apiUpdateTimer) lv_timer_pause(screenObjs.apiUpdateTimer);
    
    Serial.println("All Timers Paused");

    // Update tab visibility based on new screen
    // First, show all tabs
    lv_obj_clear_flag(tabHome, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(tabMining, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(tabActivity, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(tabBitcoinNews, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(tabSettings, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(statusBarObj, LV_OBJ_FLAG_HIDDEN);

    Serial.println("All Tabs Shown");
    // Then hide the active tab (as it's the current screen)
    switch (newScreen) {
        case activeScreenSplash:
            lv_obj_add_flag(tabHome, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(tabMining, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(tabActivity, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(tabBitcoinNews, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(tabSettings, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(statusBarObj, LV_OBJ_FLAG_HIDDEN);
            splashScreen();
            break;
        case activeScreenInitStartup:
            lv_obj_add_flag(tabHome, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(tabMining, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(tabActivity, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(tabBitcoinNews, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(tabSettings, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(statusBarObj, LV_OBJ_FLAG_HIDDEN);
            setupSettingsScreen();
            break;
        case activeScreenHome:
            lv_obj_add_flag(tabHome, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(screenObjs.homeMainContainer, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(screenObjs.clockTimer);
            Serial.println("Home Screen Shown");
            break;
        case activeScreenMining:
            lv_obj_add_flag(tabBitcoinNews, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(screenObjs.miningMainContainer, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(screenObjs.chartUpdateTimer);
            Serial.println("Mining Screen Shown");
            break;
        case activeScreenActivity:
            lv_obj_add_flag(tabBitcoinNews, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(screenObjs.activityMainContainer, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(screenObjs.labelUpdateTimer);
            Serial.println("Activity Screen Shown");
            break;
        case activeScreenBitcoinNews:
            lv_obj_add_flag(tabBitcoinNews, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(screenObjs.bitcoinNewsMainContainer, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(screenObjs.apiUpdateTimer);
            Serial.println("Bitcoin News Screen Shown");
            break;
        case activeScreenSettings:
            lv_obj_add_flag(tabBitcoinNews, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(screenObjs.settingsMainContainer, LV_OBJ_FLAG_HIDDEN);
            
            Serial.println("Settings Screen Shown");
            break;
    }

    activeScreen = newScreen;
    lvgl_port_unlock();
}

void tabIconEventHandler(lv_event_t* tabEvent)
{
    lv_event_code_t code = lv_event_get_code(tabEvent);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* clickedTab = lv_event_get_target(tabEvent);
        
        // Determine which tab was clicked and switch screens accordingly
        if (clickedTab == tabHome) {
            switchToScreen(activeScreenHome);
            Serial.println("Home Tab Clicked");
        } else if (clickedTab == tabMining) {
            switchToScreen(activeScreenMining);
            Serial.println("Mining Tab Clicked");
        } else if (clickedTab == tabActivity) {
            switchToScreen(activeScreenActivity);
            Serial.println("Activity Tab Clicked");
        } else if (clickedTab == tabBitcoinNews) {
            switchToScreen(activeScreenBitcoinNews);
            Serial.println("Bitcoin News Tab Clicked");
        } else if (clickedTab == tabSettings) {
            switchToScreen(activeScreenSettings);
            Serial.println("Settings Tab Clicked");
        }
    }
}

