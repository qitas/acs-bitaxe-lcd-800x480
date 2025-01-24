#pragma once

#include "nvs_flash.h"
#include "nvs.h"
#include "UIThemes.h"


// Add these constants
#define NVS_NAMESPACE_THEMES "uiThemes"
#define NVS_NAMESPACE_FLAGS "uiFlags"
#define NVS_NAMESPACE_SETTINGS "uiSettings"
#define NVS_NAMESPACE_ASIC_SETTINGS "asicSettings"

#define NVS_KEY_THEME "themePreset"
#define NVS_KEY_FIRST_BOOT "firstBoot"

#define NVS_KEY_WIFI_SSID1 "wifiSSID1"
#define NVS_KEY_WIFI_PASSWORD1 "wifiPassword1"
#define NVS_KEY_WIFI_SSID2 "wifiSSID2"
#define NVS_KEY_WIFI_PASSWORD2 "wifiPassword2"
#define NVS_KEY_BTC_ADDRESS "btcAddress"
#define NVS_KEY_TIME_OFFSET_HOURS "timeOffsetHours"
#define NVS_KEY_TIME_OFFSET_MINUTES "timeOffsetMinutes"

#define NVS_KEY_ASIC_FREQ_LOW "asicFreqLow"
#define NVS_KEY_ASIC_FREQ_MED "asicFreqMed"
#define NVS_KEY_ASIC_FREQ_HIGH "asicFreqHigh"

#define NVS_KEY_ASIC_FAN_VOLTAGE_LOW "asicFanVoltageLow"
#define NVS_KEY_ASIC_FAN_VOLTAGE_MED "asicFanVoltageMed"
#define NVS_KEY_ASIC_FAN_VOLTAGE_HIGH "asicFanVoltageHigh"

extern void initializeNVS(void);
extern themePreset_t loadThemeFromNVS(void);
extern void saveThemeToNVS(themePreset_t preset);
extern bool isFirstBoot(void);
extern void setFirstBootComplete(void);
extern void saveSettingsToNVSasString(const char* key, const char* value, size_t size);
extern void loadSettingsFromNVSasString(const char* key, char* value, size_t size);
extern void factoryResetNVS(void);

