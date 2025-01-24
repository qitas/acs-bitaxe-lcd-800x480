#include "wifiFeatures.h"
#include "initialSetupScreen.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_heap_caps.h>

#define MAX_NETWORKS 20
#define MAX_SSID_SCAN_LENGTH 33

// Static PSRAM array to store networks
WifiNetworkScan* storedNetworks = nullptr;
uint16_t* storedNetworkCount = nullptr;

void inistialiseWifiScanBuffers() {
    storedNetworks = (WifiNetworkScan*)heap_caps_malloc(
            sizeof(WifiNetworkScan) * MAX_NETWORKS, MALLOC_CAP_SPIRAM);
    storedNetworkCount = (uint16_t*)heap_caps_malloc(
            sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

void listNetworks() {
    // Start WiFi scan

    int numNetworks = WiFi.scanNetworks();
    
    if (numNetworks == -1) {
        Serial.println("WiFi scan failed!");
        return;
    }
    
    // Allocate storage for network count if not already allocated
    if (storedNetworkCount == nullptr) {
        storedNetworkCount = (uint16_t*)heap_caps_malloc(
            sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!storedNetworkCount) {
            Serial.println("Failed to allocate network count storage!");
            return;
        }
    }
    
    // Allocate storage in PSRAM for discovered networks if not already allocated
    if (storedNetworks == nullptr) {
        storedNetworks = (WifiNetworkScan*)heap_caps_malloc(
            sizeof(WifiNetworkScan) * MAX_NETWORKS, MALLOC_CAP_SPIRAM);
        if (!storedNetworks) {
            Serial.println("Failed to allocate network storage in PSRAM!");
            return;
        }
    }
    
    // Store the actual number of networks found (limited by MAX_NETWORKS)
    memset(storedNetworkCount, 0, sizeof(uint16_t));
    *storedNetworkCount = min(numNetworks, MAX_NETWORKS);
    
    Serial.printf("Number of networks found: %d\n", *storedNetworkCount);
    
    for (int i = 0; i < *storedNetworkCount; i++) {
        //clear the ssid buffer
        memset(storedNetworks[i].ssid, 0, sizeof(storedNetworks[i].ssid));
        // Copy network info to PSRAM storage
        strncpy(storedNetworks[i].ssid, WiFi.SSID(i).c_str(), MAX_SSID_SCAN_LENGTH - 1);
        storedNetworks[i].ssid[MAX_SSID_SCAN_LENGTH - 1] = '\0';
        storedNetworks[i].rssi = WiFi.RSSI(i);
        
        // Print the network info
        Serial0.printf("%2d | SSID: %-32s | RSSI: %ld\n", 
            i + 1, storedNetworks[i].ssid, storedNetworks[i].rssi);
    }
    
    // Clean up scan results
    WiFi.scanDelete();

    Serial0.printf("Free heap: %lu\n", ESP.getFreeHeap());
    Serial0.printf("Minimum free heap: %lu\n", ESP.getMinFreeHeap());
}