#pragma once

#include <lvgl.h>
#include <esp_wifi.h>
#include "esp_event.h"
#include "wifiFeatures.h"







extern WifiNetworkScan* storedNetworks;
extern uint16_t* storedNetworkCount;



enum initialSetupScreens {
    initialSetupScreenSplash,
    initialSetupScreenSetupWifi,
    //initialSetupScreenSetupTheme,
    initialSetupScreenSetupPool,
    //initialSetupScreenSetupPoolUser,
    //initialSetupScreenSetupComplete
};

extern void initialSetupScreen();
extern void switchToInitialSetupScreen(initialSetupScreens newScreen);
extern void initSetupSplashScreen();
extern void initSetupWifiScreen();
extern void initSetupThemeScreen();
extern void initSetupPoolScreen();
//extern void initSetupPoolUserScreen();
extern void initSetupCompleteScreen();

