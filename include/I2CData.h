#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <esp_attr.h>
#include "UIScreens.h"
#include <lvgl.h>
// Unavailable Addresses : 20-27 30-3f, 5d These are used for touch screen
// Unavailable Bitaxe Addresses : 0x40, 0x49, 0x4c, 0x3c -> This if for the OLED and has overlapping I2c. Need to get rid of this.

#define i2cSlaveAddress 0x50
#define i2cSlaveSDA 16
#define i2cSlaveSCL 15

#define LVGL_REG_SSID           0x21
#define LVGL_REG_IP_ADDR        0x22
#define LVGL_REG_WIFI_STATUS    0x23
#define LVGL_REG_POOL_URL       0x24
#define LVGL_REG_FALLBACK_URL   0x25
#define LVGL_REG_POOL_PORTS     0x26

// Mining data registers (5 second updates)
#define LVGL_REG_HASHRATE        0x30
#define LVGL_REG_HIST_HASHRATE   0x31
#define LVGL_REG_EFFICIENCY      0x32
#define LVGL_REG_BEST_DIFF       0x33
#define LVGL_REG_SESSION_DIFF    0x34
#define LVGL_REG_SHARES          0x35

// Monitoring data registers (5 second updates)
#define LVGL_REG_TEMPS           0x40 // 8 * float
#define LVGL_REG_ASIC_FREQ      0x41 // float
#define LVGL_REG_FAN            0x42 // 2 * float
#define LVGL_REG_POWER_STATS    0x43 // 4 * float
#define LVGL_REG_ASIC_INFO      0x44 // uint16
#define LVGL_REG_UPTIME         0x45 // uint32 Look into moving to 64 bit to increase time 

// Device status registers (on change only)
#define LVGL_REG_FLAGS          0x50 // 4 bytes

#define LVGL_REG_DEVICE_INFO   0x52 // not used
#define LVGL_REG_BOARD_INFO    0x53 // variable length
#define LVGL_REG_CLOCK_SYNC    0x54 // uint32

// API Data Registers
#define LVGL_REG_API_BTC_PRICE   0x60 // uint32
#define LVGL_REG_API_NETWORK_HASHRATE 0x61 // double
#define LVGL_REG_API_NETWORK_DIFFICULTY 0x62 // double
#define LVGL_REG_API_BLOCK_HEIGHT 0x63 // uint32
#define LVGL_REG_API_DIFFICULTY_PROGRESS 0x64 // double
#define LVGL_REG_API_DIFFICULTY_CHANGE 0x65 // double
#define LVGL_REG_API_REMAINING_BLOCKS 0x66 // uint32
#define LVGL_REG_API_REMAINING_TIME 0x67 // uint32
#define LVGL_REG_API_FASTEST_FEE 0x68 // uint32
#define LVGL_REG_API_HALF_HOUR_FEE 0x69 // uint32
#define LVGL_REG_API_HOUR_FEE 0x6A // uint32
#define LVGL_REG_API_ECONOMY_FEE 0x6B // uint32
#define LVGL_REG_API_MINIMUM_FEE 0x6C // uint32

// Settings registers
#define LVGL_REG_SETTINGS_HOSTNAME 0xA0 // 32 bytes
#define LVGL_REG_SETTINGS_WIFI_SSID 0xA1 // 32 bytes
#define LVGL_REG_SETTINGS_WIFI_PASSWORD 0xA2 // 32 bytes

// Maximum lengths for strings
#define MAX_SSID_LENGTH       32
#define MAX_IP_LENGTH        16
#define MAX_URL_LENGTH       128
#define MAX_STATUS_LENGTH    32
#define MAX_INFO_LENGTH      64

// Maximum lengths for numeric data
#define MAX_FLOAT_SIZE      sizeof(float)
#define MAX_UINT32_SIZE    sizeof(uint32_t)
#define MAX_UINT64_SIZE    sizeof(uint64_t)
#define MAX_UINT16_SIZE    sizeof(uint16_t)
#define MAX_TEMPS_COUNT    8

#define PSRAM_ATTR __attribute__((section(".psram")))

#define MAX_DIFF_LENGTH 16
#define I2C_BUFFER_SIZE 512

// Network data structure
struct NetworkData 
{
    char ssid[MAX_SSID_LENGTH];
    char ipAddress[MAX_IP_LENGTH];
    char wifiStatus[MAX_STATUS_LENGTH];
    char poolUrl[MAX_URL_LENGTH];
    char fallbackUrl[MAX_URL_LENGTH];
    uint16_t poolPort;
    uint16_t fallbackPort;
};

// Mining data structure
struct MiningData 
{
    float hashrate;
    float historicalHashrate;
    float efficiency;
    char bestDiff[MAX_DIFF_LENGTH];
    char sessionDiff[MAX_DIFF_LENGTH];
    uint32_t shares;
    int acceptedShares;
    int rejectedShares;
    float rejectRatePercent;
};

// Monitoring data structure
struct MonitoringData 
{
    float temperatures[8];    // Array for multiple temperature sensors For Hex model
    uint32_t asicFrequency;
    float fanSpeed;
    float fanSpeedPercent;
    struct 
    {
        float voltage;
        float current;
        float power;
        float domainVoltage;
    } powerStats;
    uint32_t asicInfo;
    uint32_t uptime;
};

// Device status structure
struct DeviceStatus 
{
    uint32_t flags;
    char deviceInfo[MAX_INFO_LENGTH];
    char boardInfo[MAX_INFO_LENGTH];
    uint32_t clockSync;
};

// API Data Structure
struct APIData
{
    uint32_t btcPriceUSD;
    double networkHashrate;
    double networkDifficulty;
    uint32_t blockHeight;
    double difficultyProgressPercent;
    double difficultyChangePercent;
    uint32_t remainingBlocksToDifficultyAdjustment;
    uint32_t remainingTimeToDifficultyAdjustment;
    uint32_t fastestFee;
    uint32_t halfHourFee;
    uint32_t hourFee;
    uint32_t economyFee;
    uint32_t minimumFee;
};

// Global data container
struct I2CDataContainer 
{
    NetworkData network;
    MiningData mining;
    MonitoringData monitoring;
    DeviceStatus status;
    APIData api;
};

extern bool settingsChanged;

// External declarations
extern uint32_t byteCount;
extern TwoWire Wire2;
extern PSRAM_ATTR I2CDataContainer i2cData;
extern uint8_t* i2cBuffer;
extern SettingsTextAreas settingsTextAreas;


// Function declarations
extern void onRequest();
extern void onReceive(int len);
extern void initI2CSlave();
extern uint32_t keepClockTime();
extern void resetI2C();
extern void checkI2CHealth();
extern void initI2CBuffer();
extern void prepareSettingsResponse(const char* text, uint8_t reg);