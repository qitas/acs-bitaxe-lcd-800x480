#include "UIHandler.h"
#include "lvgl_port_v8.h"
#include <lvgl.h>
#include <Arduino.h>
#include "UIScreens.h"

ScreenObjects screenObjs = {0};
void switchToScreen(ScreenType newScreen)
{
    if (activeScreen == newScreen) 
    {
        return; // Already on this screen
    }

    lvgl_port_lock(-1);  // Lock for thread safety

    // Hide all containers first
    lv_obj_add_flag(screenObjs.homeMainContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screenObjs.miningMainContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screenObjs.activityMainContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screenObjs.bitcoinNewsMainContainer, LV_OBJ_FLAG_HIDDEN);
    // Pause all timers
     if (screenObjs.labelUpdateTimer) {
        lv_timer_pause(screenObjs.labelUpdateTimer);
    }
    if (screenObjs.chartUpdateTimer) {
        lv_timer_pause(screenObjs.chartUpdateTimer);
    }
    if (screenObjs.clockTimer) {
        lv_timer_pause(screenObjs.clockTimer);
    }

    // Show the requested container
    switch (newScreen) 
    {
        case activeScreenHome:
            lv_obj_clear_flag(screenObjs.homeMainContainer, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(screenObjs.clockTimer);
            break;
        case activeScreenMining:
            lv_obj_clear_flag(screenObjs.miningMainContainer, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(screenObjs.chartUpdateTimer);
            break;
        case activeScreenActivity:
            lv_obj_clear_flag(screenObjs.activityMainContainer, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(screenObjs.labelUpdateTimer);
            break;
        case activeScreenBitcoinNews:
            lv_obj_clear_flag(screenObjs.bitcoinNewsMainContainer, LV_OBJ_FLAG_HIDDEN);
            break;
    }
    // Update the active screen to new screen
    activeScreen = newScreen;
    lvgl_port_unlock();  // Release the lock
}

void tabIconEventHandler(lv_event_t* tabEvent)
{
    lv_event_code_t code = lv_event_get_code(tabEvent);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* clickedTab = lv_event_get_target(tabEvent);
        
        // Determine which tab was clicked and switch screens accordingly
        if (clickedTab == tabHome) {
            switchToScreen(activeScreenHome);
        } else if (clickedTab == tabMining) {
            switchToScreen(activeScreenMining);
        } else if (clickedTab == tabActivity) {
            switchToScreen(activeScreenActivity);
        } else if (clickedTab == tabBitcoinNews) {
            switchToScreen(activeScreenBitcoinNews);
        }
    }
}

