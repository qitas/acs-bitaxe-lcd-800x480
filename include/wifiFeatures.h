#pragma once

#include <lvgl.h>
#include <Arduino.h>
#include <esp_wifi.h>
#include "esp_event.h"
#include <WiFi.h>

struct WifiNetworkScan {
    char ssid[33];  // 32 chars + null terminator
    int32_t rssi;
};

void inistialiseWifiScanBuffers();
void listNetworks();
