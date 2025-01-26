#include "I2CData.h"
#include <Arduino.h>
#include "esp_mac.h"
#include <algorithm>
#include <TimeLib.h>
#include "UIScreens.h"
#include <lvgl.h>
#include "modelConfig.h"
#if I2C == 1

// Custom min function because the standard one is not available 
#define __min(a,b) ((a)<(b)?(a):(b))

// Global variables
uint32_t PSRAM_ATTR byteCount = 0;
TwoWire Wire2 = TwoWire(1);
IncomingDataContainer PSRAM_ATTR IncomingData = {0};


static bool i2cNeedsReset = false;
static uint32_t lastErrorTime = 0;
static uint32_t errorCount = 0;
#define ERROR_THRESHOLD 25        // Number of errors before forcing reset

uint8_t* i2cBuffer = nullptr;

// Global variables for tracking request state
static uint8_t currentSettingsReg = 0;

// Add at the top with other globals
static uint8_t responseBuffer[32] = {0};  // Pre-allocated buffer for quick responses
static uint8_t responseLength = 3;        // Default length (register + 2 bytes)

void initI2CBuffer() 
{
    if (i2cBuffer == nullptr) 
    {
        i2cBuffer = (uint8_t*)heap_caps_calloc(I2C_BUFFER_SIZE, sizeof(uint8_t), MALLOC_CAP_8BIT);
    }
}

void freeI2CBuffer() 
{
    if (i2cBuffer != nullptr) 
    {
        heap_caps_free(i2cBuffer);
        i2cBuffer = nullptr;
    }
}

// Resetting the I2C bus to avoid memory corruption due to accumulated errors
void resetI2C() 
{
    Serial.println("Starting I2C reset...");
    
    Wire2.end();
    Serial.println("I2C bus ended");
    
    // Free and reinitialize the I2C buffer
    //freeI2CBuffer();
    //Serial.println("I2C buffer freed");
    //initI2CBuffer();
    //Serial.println("I2C buffer initialized");
    
    errorCount = 0;
    i2cNeedsReset = false;
    
    delay(100);
    
    Wire2.onReceive(onReceive);
    Serial.println("I2C onReceive set");
    Wire2.onRequest(onRequest);
    Serial.println("I2C onRequest set");
    Wire2.setPins(i2cSlaveSDA, i2cSlaveSCL);
    Serial.println("I2C pins set");
    Wire2.setBufferSize(I2C_BUFFER_SIZE);
    Serial.println("I2C buffer size set");
    
    bool beginResult = Wire2.begin((uint8_t)i2cSlaveAddress);
    Serial.println("I2C begun");
    
    if (beginResult) 
    {
        Serial.println("I2C bus reset completed successfully");
    } 
    else 
    {
        Serial.println("I2C bus reset failed - will try again later");
        i2cNeedsReset = true;
    }
    
    //memset(&IncomingData, 0, sizeof(IncomingDataContainer));
}

// Data handling functions
void handleNetworkData(uint8_t* buffer, uint8_t len) 
{
    if (len < 2) return;
    
    uint8_t reg = buffer[0];
    uint8_t dataLen = buffer[1];
    
    switch(reg) 
    {
        case LVGL_REG_SSID:
            memset(IncomingData.network.ssid, 0, MAX_SSID_LENGTH);
            memcpy(IncomingData.network.ssid, &buffer[2], __min(dataLen, MAX_SSID_LENGTH - 1));
            break;
        case LVGL_REG_IP_ADDR:
            memset(IncomingData.network.ipAddress, 0, MAX_IP_LENGTH);
            memcpy(IncomingData.network.ipAddress, &buffer[2], __min(dataLen, MAX_IP_LENGTH - 1));
            break;
        case LVGL_REG_WIFI_STATUS:
            memset(IncomingData.network.wifiStatus, 0, MAX_STATUS_LENGTH);
            memcpy(IncomingData.network.wifiStatus, &buffer[2], __min(dataLen, MAX_STATUS_LENGTH - 1));
            break;
        case LVGL_REG_POOL_URL:
            memset(IncomingData.network.poolUrl, 0, MAX_URL_LENGTH);
            memcpy(IncomingData.network.poolUrl, &buffer[2], __min(dataLen, MAX_URL_LENGTH - 1));
            break;
        case LVGL_REG_FALLBACK_URL:
            memset(IncomingData.network.fallbackUrl, 0, MAX_URL_LENGTH);
            memcpy(IncomingData.network.fallbackUrl, &buffer[2], __min(dataLen, MAX_URL_LENGTH - 1));
            break;
        case LVGL_REG_POOL_PORTS:
                if (dataLen >= 4) 
            {
                IncomingData.network.poolPort = (buffer[3] << 8) | buffer[2];
                IncomingData.network.fallbackPort = (buffer[5] << 8) | buffer[4];
                Serial.printf("Pool: %s : %d, Fallback: %s : %d\n", IncomingData.network.poolUrl, IncomingData.network.poolPort, IncomingData.network.fallbackUrl, IncomingData.network.fallbackPort);
            }
            break;
        default:
            Serial.printf("Warning: Unknown network register 0x%02X with length %d\n", reg, dataLen);
            break;
    }
}

void handleMiningData(uint8_t* buffer, uint8_t len) 
{
    // must be more than the register and length bytes
    if (len < 2) return;
    
    uint8_t reg = buffer[0];
    uint8_t dataLen = buffer[1];
    
    
    switch(reg) 
    {
        case LVGL_REG_HASHRATE: 
        {
            float hashrate;
            memset(&hashrate, 0, sizeof(float));
            memcpy(&hashrate, &buffer[2], sizeof(float));
            IncomingData.mining.hashrate = hashrate;
            Serial.print("Hashrate received: ");
            Serial.println(IncomingData.mining.hashrate);
            break;
        }
        case LVGL_REG_HIST_HASHRATE: 
        {
            float historicalHashrate;
            memset(&historicalHashrate, 0, sizeof(float));
            memcpy(&historicalHashrate, &buffer[2], sizeof(float));
            IncomingData.mining.historicalHashrate = historicalHashrate;
            Serial.print("Historical Hashrate received: ");
            Serial.println(IncomingData.mining.historicalHashrate);
            break;
        }
        case LVGL_REG_EFFICIENCY: 
        {
            float efficiency;
            memset(&efficiency, 0, sizeof(float));
            memcpy(&efficiency, &buffer[2], sizeof(float));
            IncomingData.mining.efficiency = efficiency;
            Serial.print("Efficiency received: ");
            Serial.println(IncomingData.mining.efficiency);
            break;
        }
        case LVGL_REG_BEST_DIFF:
        {
            memset(IncomingData.mining.bestDiff, 0, MAX_DIFF_LENGTH);
            memcpy(IncomingData.mining.bestDiff, &buffer[2], __min(dataLen, MAX_DIFF_LENGTH - 1));
            IncomingData.mining.bestDiff[dataLen] = '\0';  // Null terminate the string
            Serial.print("Best Diff received: ");
            Serial.println(IncomingData.mining.bestDiff);
            break;
        }
        case LVGL_REG_SESSION_DIFF:
        {
            memset(IncomingData.mining.sessionDiff, 0, MAX_DIFF_LENGTH);
            memcpy(IncomingData.mining.sessionDiff, &buffer[2], __min(dataLen, MAX_DIFF_LENGTH - 1));
            IncomingData.mining.sessionDiff[dataLen] = '\0';  // Null terminate the string
            Serial.print("Session Diff received: ");
            Serial.println(IncomingData.mining.sessionDiff);
            break;
        }
        case LVGL_REG_SHARES:
        // Each share is 4 bytes, sent in the same I2C sequence. Accepted Shares first, then Rejected Shares
        {
            memset(&IncomingData.mining.acceptedShares, 0, sizeof(uint32_t));
            memset(&IncomingData.mining.rejectedShares, 0, sizeof(uint32_t));
            IncomingData.mining.acceptedShares = (buffer[2] << 0) | (buffer[3] << 8) | (buffer[4] << 16) | (buffer[5] << 24);
            IncomingData.mining.rejectedShares = (buffer[6] << 0) | (buffer[7] << 8) | (buffer[8] << 16) | (buffer[9] << 24);
            IncomingData.mining.shares = IncomingData.mining.acceptedShares + IncomingData.mining.rejectedShares;
            Serial.printf("Accepted shares: %d, Rejected shares: %d, Total shares: %lu\n", 
                IncomingData.mining.acceptedShares, 
                IncomingData.mining.rejectedShares,
                IncomingData.mining.shares);
            if (IncomingData.mining.shares > 0) 
            {
                IncomingData.mining.rejectRatePercent = (IncomingData.mining.rejectedShares * 100.0f) / IncomingData.mining.shares;
            } 
            else 
            {
                IncomingData.mining.rejectRatePercent = 0.0f;
            }
            break;
        }
        default:
            Serial.printf("Warning: Unknown mining register 0x%02X with length %d\n", reg, dataLen);
            break;
    }
}

void handleMonitoringData(uint8_t* buffer, uint8_t len) 
{
    if (len < 2) return;
    
    uint8_t reg = buffer[0];
    uint8_t dataLen = buffer[1];
    
    switch(reg) {
        case LVGL_REG_TEMPS: 
        {
            // Take the last 4 bytes from the buffer for Temp 1
            float temperature;
            memset(&temperature, 0, sizeof(float));
            uint8_t tempBytes[4] = 
            {
                buffer[len-4],
                buffer[len-3],
                buffer[len-2],
                buffer[len-1]
            };
            memcpy(&temperature, tempBytes, sizeof(float));
            IncomingData.monitoring.temperatures[0] = temperature;
            Serial.print("Temperature received: ");
            Serial.println(IncomingData.monitoring.temperatures[0]);
            break;
        }
        case LVGL_REG_ASIC_FREQ:
        {
            float asicFreq;
            memset(&asicFreq, 0, sizeof(float));
            memcpy(&asicFreq, &buffer[2], sizeof(float));
            IncomingData.monitoring.asicFrequency = asicFreq;
            Serial.print("ASIC Frequency received: ");
            Serial.println(IncomingData.monitoring.asicFrequency);
            break;
        }
        case LVGL_REG_FAN:
        {
            float fanStats[2];  // Array to hold RPM and percentage
            memset(fanStats, 0, sizeof(float) * 2);
            memcpy(fanStats, &buffer[2], __min(dataLen, sizeof(float) * 2));
            IncomingData.monitoring.fanSpeed = fanStats[0];
            IncomingData.monitoring.fanSpeedPercent = fanStats[1];
            
            Serial.printf("Fan RPM: %f, Fan Percent: %f%%\n", 
                IncomingData.monitoring.fanSpeed,
                IncomingData.monitoring.fanSpeedPercent);
            break;
        }
        case LVGL_REG_POWER_STATS:
        {
            float powerStatsArray[4];
            memset(powerStatsArray, 0, sizeof(float) * 4);
            memcpy(powerStatsArray, &buffer[2], __min(dataLen, sizeof(float) * 4));
            IncomingData.monitoring.powerStats.voltage = powerStatsArray[0];
            IncomingData.monitoring.powerStats.current = powerStatsArray[1];
            IncomingData.monitoring.powerStats.power = powerStatsArray[2];
            IncomingData.monitoring.powerStats.domainVoltage = powerStatsArray[3];
            Serial.printf("Voltage: %f, Current: %f, Power: %f, Domain Voltage: %f\n", 
                IncomingData.monitoring.powerStats.voltage,
                IncomingData.monitoring.powerStats.current,
                IncomingData.monitoring.powerStats.power,
                IncomingData.monitoring.powerStats.domainVoltage);
            break;
        }
        case LVGL_REG_ASIC_INFO:
        {
            memset(&IncomingData.monitoring.asicInfo, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.monitoring.asicInfo, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            break;
        }
        case LVGL_REG_UPTIME:
        {
            memset(&IncomingData.monitoring.uptime, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.monitoring.uptime, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            break;  
        }
        default:
        {   
            Serial.printf("Warning: Unknown monitoring register 0x%02X with length %d\n", reg, dataLen);
            break;
        }
    }
}

void handleDeviceStatus(uint8_t* buffer, uint8_t len) 
{
    if (len < 2) return;
    
    uint8_t reg = buffer[0];
    uint8_t dataLen = buffer[1];
    
    switch(reg)
    {
        case LVGL_REG_FLAGS:
        {
            memset(&IncomingData.status.flags, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.status.flags, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            break;
        }
        case LVGL_REG_DEVICE_INFO:
        {
            memset(IncomingData.status.deviceInfo, 0, MAX_INFO_LENGTH);
            memcpy(IncomingData.status.deviceInfo, &buffer[2], __min(dataLen, MAX_INFO_LENGTH - 1));
            break;
        }
        case LVGL_REG_BOARD_INFO:
        {
            memset(IncomingData.status.boardInfo, 0, MAX_INFO_LENGTH);
            memcpy(IncomingData.status.boardInfo, &buffer[2], __min(dataLen, MAX_INFO_LENGTH - 1));
            break;
        }
        case LVGL_REG_CLOCK_SYNC:
        {
            uint32_t timestamp;
            memset(&timestamp, 0, MAX_UINT32_SIZE);
            memcpy(&timestamp, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            IncomingData.status.clockSync = timestamp; // Subtract 6 hours to account for timezone TODO: make this dynamic
            Serial.printf("Clock sync: %lu\n", timestamp);
            if (timestamp > 946684800) 
            {  // Basic sanity check (timestamp after year 2000)
                setTime(timestamp);
                Serial.printf("Time set to: %02d/%02d/%04d %02d:%02d:%02d\n", 
                    month(), day(), year(),
                    hour(), minute(), second());
            }
            break;
        }
        default:
        {
            Serial.printf("Warning: Unknown device status register 0x%02X with length %d\n", reg, dataLen);
            break;
        }
    }
}

void handleAPIData(uint8_t* buffer, uint8_t len) 
{
        if (len < 2) return;
    
    uint8_t reg = buffer[0];
    uint8_t dataLen = buffer[1];

    switch(reg)
    {
        case LVGL_REG_API_BTC_PRICE:
        {
            memset(&IncomingData.api.btcPriceUSD, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.btcPriceUSD, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("BTC Price received: %lu\n", IncomingData.api.btcPriceUSD);
            break;
        }
        case LVGL_REG_API_NETWORK_HASHRATE:
        {
            memset(&IncomingData.api.networkHashrate, 0, sizeof(double));
            memcpy(&IncomingData.api.networkHashrate, &buffer[2], __min(dataLen, sizeof(double)));
            Serial.printf("Network Hashrate received: %f\n", IncomingData.api.networkHashrate);
            break;
        }
        case LVGL_REG_API_NETWORK_DIFFICULTY:
        {
            memset(&IncomingData.api.networkDifficulty, 0, sizeof(double));
            memcpy(&IncomingData.api.networkDifficulty, &buffer[2], __min(dataLen, sizeof(double)));
            Serial.printf("Network Difficulty received: %f\n", IncomingData.api.networkDifficulty);
            break;
        }
        case LVGL_REG_API_BLOCK_HEIGHT:
        {
            memset(&IncomingData.api.blockHeight, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.blockHeight, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Block Height received: %lu\n", IncomingData.api.blockHeight);
            break;
        }
        case LVGL_REG_API_DIFFICULTY_PROGRESS:
        {
            memset(&IncomingData.api.difficultyProgressPercent, 0, sizeof(double));
            memcpy(&IncomingData.api.difficultyProgressPercent, &buffer[2], __min(dataLen, sizeof(double)));
            Serial.printf("Difficulty Progress Percent received: %f\n", IncomingData.api.difficultyProgressPercent);
            break;
        }
        case LVGL_REG_API_DIFFICULTY_CHANGE:
        {
            memset(&IncomingData.api.difficultyChangePercent, 0, sizeof(double));
            memcpy(&IncomingData.api.difficultyChangePercent, &buffer[2], __min(dataLen, sizeof(double)));
            Serial.printf("Difficulty Change Percent received: %f\n", IncomingData.api.difficultyChangePercent);
            break;
        }
        case LVGL_REG_API_REMAINING_BLOCKS:
        {
            memset(&IncomingData.api.remainingBlocksToDifficultyAdjustment, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.remainingBlocksToDifficultyAdjustment, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Remaining Blocks to Difficulty Adjustment received: %lu\n", IncomingData.api.remainingBlocksToDifficultyAdjustment);
            break;
        }
        case LVGL_REG_API_REMAINING_TIME:
        {
            memset(&IncomingData.api.remainingTimeToDifficultyAdjustment, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.remainingTimeToDifficultyAdjustment, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            IncomingData.api.remainingTimeToDifficultyAdjustment /= 1000; // Convert from milliseconds to seconds?
            Serial.printf("Remaining Time to Difficulty Adjustment received: %lu\n", IncomingData.api.remainingTimeToDifficultyAdjustment);
            break;
        }
        case LVGL_REG_API_FASTEST_FEE:
        {
            memset(&IncomingData.api.fastestFee, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.fastestFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Fastest Fee received: %lu\n", IncomingData.api.fastestFee);
            break;
        }
        case LVGL_REG_API_HALF_HOUR_FEE:
        {
            memset(&IncomingData.api.halfHourFee, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.halfHourFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Half Hour Fee received: %lu\n", IncomingData.api.halfHourFee);
            break;
        }
        case LVGL_REG_API_HOUR_FEE:
        {
            memset(&IncomingData.api.hourFee, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.hourFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Hour Fee received: %lu\n", IncomingData.api.hourFee);
            break;
        }
        case LVGL_REG_API_ECONOMY_FEE:
        {
            memset(&IncomingData.api.economyFee, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.economyFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Economy Fee received: %lu\n", IncomingData.api.economyFee);
            break;
        }
        case LVGL_REG_API_MINIMUM_FEE:
        {
            memset(&IncomingData.api.minimumFee, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.api.minimumFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Minimum Fee received: %lu\n", IncomingData.api.minimumFee);
            break;
        }
        default:
        {
            Serial.printf("Warning: Unknown API register 0x%02X with length %d\n", reg, dataLen);
            break;
        }
    }
}

const char* getRegisterName(uint8_t reg) 
{
    switch(reg) 
    {
        // Network registers
        case LVGL_REG_SSID: return "SSID";
        case LVGL_REG_IP_ADDR: return "IP_ADDR";
        case LVGL_REG_WIFI_STATUS: return "WIFI_STATUS";
        case LVGL_REG_POOL_URL: return "POOL_URL";
        case LVGL_REG_FALLBACK_URL: return "FALLBACK_URL";
        case LVGL_REG_POOL_PORTS: return "POOL_PORTS";
        
        // Mining registers
        case LVGL_REG_HASHRATE: return "HASHRATE";
        case LVGL_REG_HIST_HASHRATE: return "HIST_HASHRATE";
        case LVGL_REG_EFFICIENCY: return "EFFICIENCY";
        case LVGL_REG_BEST_DIFF: return "BEST_DIFF";
        case LVGL_REG_SESSION_DIFF: return "SESSION_DIFF";
        case LVGL_REG_SHARES: return "SHARES";
        
        // Monitoring registers
        case LVGL_REG_TEMPS: return "TEMPS";
        case LVGL_REG_ASIC_FREQ: return "ASIC_FREQ";
        case LVGL_REG_FAN: return "FAN";
        case LVGL_REG_POWER_STATS: return "POWER_STATS";
        case LVGL_REG_ASIC_INFO: return "ASIC_INFO";
        
        // Device status registers
        case LVGL_REG_FLAGS: return "FLAGS";
        case LVGL_REG_UPTIME: return "UPTIME";
        case LVGL_REG_DEVICE_INFO: return "DEVICE_INFO";
        case LVGL_REG_BOARD_INFO: return "BOARD_INFO";
        case LVGL_REG_CLOCK_SYNC: return "CLOCK_SYNC";
        
        // API registers
        case LVGL_REG_API_BTC_PRICE: return "API_BTC_PRICE";
        case LVGL_REG_API_NETWORK_HASHRATE: return "API_NETWORK_HASHRATE";
        case LVGL_REG_API_NETWORK_DIFFICULTY: return "API_NETWORK_DIFFICULTY";
        case LVGL_REG_API_BLOCK_HEIGHT: return "API_BLOCK_HEIGHT";
        case LVGL_REG_API_DIFFICULTY_PROGRESS: return "API_DIFFICULTY_PROGRESS";
        case LVGL_REG_API_DIFFICULTY_CHANGE: return "API_DIFFICULTY_CHANGE";
        case LVGL_REG_API_REMAINING_BLOCKS: return "API_REMAINING_BLOCKS";
        case LVGL_REG_API_REMAINING_TIME: return "API_REMAINING_TIME";
        case LVGL_REG_API_FASTEST_FEE: return "API_FASTEST_FEE";
        case LVGL_REG_API_HALF_HOUR_FEE: return "API_HALF_HOUR_FEE";
        case LVGL_REG_API_HOUR_FEE: return "API_HOUR_FEE";
        case LVGL_REG_API_ECONOMY_FEE: return "API_ECONOMY_FEE";
        case LVGL_REG_API_MINIMUM_FEE: return "API_MINIMUM_FEE";
        
        default: return "UNKNOWN";
    }
}

// At the top of the file with other defines
#define I2C_BUFFER_SIZE 512
#define MIN_MESSAGE_LENGTH 2  // Register + Length bytes
static uint8_t requestedRegister = 0;

// Modified onReceive function
void onReceive(int len) 
{
    if (len == 1) {
        // This is likely a register request setup
        requestedRegister = Wire2.read();
        return;
    }

    if (i2cBuffer == nullptr) 
    {
        initI2CBuffer();
    }

    // Use i2cBuffer instead of static buffer
    memset(i2cBuffer, 0, I2C_BUFFER_SIZE);
    
    // Read with timeout
    uint32_t startTime = millis();
    int bytesRead = 0;
    
    while (Wire2.available() && bytesRead < I2C_BUFFER_SIZE && 
           (millis() - startTime) < 50) // 50ms timeout
    {  
        i2cBuffer[bytesRead] = Wire2.read();
        bytesRead++;
    }

    // Check for timeout or incomplete read
    if (Wire2.available() || (bytesRead != len)) 
    {
        Serial.printf("Error: Read timeout or length mismatch. Expected %d, got %d\n", 
                     len, bytesRead);
        errorCount++;
        Serial.printf("Error count: %lu\n", errorCount);
        
        // Flush remaining bytes
        while (Wire2.available()) 
        {
            Wire2.read();
        }
        
        if (errorCount >= ERROR_THRESHOLD) 
        {
            Serial.println("Error threshold reached, scheduling I2C reset");
            i2cNeedsReset = true;
        }
        return;
    }

    // Validate message structure
    uint8_t reg = i2cBuffer[0];
    uint8_t dataLen = i2cBuffer[1];
    
    if (dataLen > (I2C_BUFFER_SIZE - 2) || (dataLen + 2) != bytesRead) 
    {
        Serial.printf("Error: Invalid data length. Claimed: %d, Actual: %d\n", 
                     dataLen, bytesRead - 2);
        errorCount++;
        Serial.printf("Error count: %lu\n", errorCount);
        if (errorCount >= ERROR_THRESHOLD) 
        {
            Serial.println("Error threshold reached, scheduling I2C reset");
            i2cNeedsReset = true;
        }
        return;
    }

    // Process the message (existing code)
    if (reg >= 0x21 && reg <= 0x26) handleNetworkData(i2cBuffer, bytesRead);
    else if (reg >= 0x30 && reg <= 0x35) handleMiningData(i2cBuffer, bytesRead);
    else if (reg >= 0x40 && reg <= 0x45) handleMonitoringData(i2cBuffer, bytesRead);
    else if (reg >= 0x50 && reg <= 0x54) handleDeviceStatus(i2cBuffer, bytesRead);
    else if (reg >= 0x60 && reg <= 0x6C) handleAPIData(i2cBuffer, bytesRead);
    else 
    {
        Serial.printf("Error: Register 0x%02X outside of valid ranges\n", reg);
        errorCount++;
        Serial.printf("Error count: %lu\n", errorCount);
    }
}

// Modified initialization function
void initI2CSlave() {
    Wire2.onReceive(onReceive);
    Wire2.onRequest(onRequest);
    Wire2.setPins(i2cSlaveSDA, i2cSlaveSCL);
    Wire2.setBufferSize(I2C_BUFFER_SIZE);
    Wire2.begin((uint8_t)i2cSlaveAddress);
    
    // Set clock speed to 400kHz (Fast mode)
    Wire2.setClock(100000);
}

// Keep the clock time updated
uint32_t keepClockTime() 
{
    static portMUX_TYPE timeMux = portMUX_INITIALIZER_UNLOCKED;
    static uint32_t lastSyncMillis = 0;
    static uint32_t lastSyncTime = 0;
    uint32_t currentTime;

    portENTER_CRITICAL(&timeMux);
    
    if (IncomingData.status.clockSync > lastSyncTime) 
    {
        lastSyncMillis = millis();
        lastSyncTime = IncomingData.status.clockSync;
    }
    
    if (lastSyncMillis == 0) 
    {
        lastSyncMillis = millis();
        currentTime = IncomingData.status.clockSync;
    } else {
        uint32_t millisSinceSync = millis() - lastSyncMillis;
        currentTime = lastSyncTime + (millisSinceSync / 1000);
    }
    
    portEXIT_CRITICAL(&timeMux);
    
    // Update the TimeLib internal time
    setTime(currentTime);  
    
    // If time has been set, adjust for timezone offset chosen by user
    if (currentTime > 946684800) 
    {
        adjustTime(-6 * 3600); //to do make this user adjustable
    }
    return currentTime;
}

// Add a periodic check in your main loop
void checkI2CHealth() 
{
    static uint32_t lastCheck = 0;
    const uint32_t CHECK_INTERVAL = 5000;  // Check every 5 seconds

    if (millis() - lastCheck > CHECK_INTERVAL) 
    {
        lastCheck = millis();
        
        // Perform reset if needed
        if (i2cNeedsReset) 
        {
            resetI2C();
        }
    }
}


// Modified onRequest handler for faster response
void onRequest() {
    // Quickly prepare the default response
    responseBuffer[0] = requestedRegister;
    responseBuffer[1] = 0x00;
    responseBuffer[2] = 0x00;
    
    // Send the pre-prepared response immediately
    Wire2.write(responseBuffer, responseLength);
    
    Serial.printf("Sent register 0x%02X with length %d\n", requestedRegister, responseLength);
    
    // Only after sending, prepare the next response if settings changed
    if (settingsChanged) {
        switch (requestedRegister) {
            case LVGL_REG_SETTINGS_HOSTNAME:
                if (settingsTextAreas.hostnameTextArea) {
                    const char* text = lv_textarea_get_text(settingsTextAreas.hostnameTextArea);
                    prepareSettingsResponse(text, LVGL_REG_SETTINGS_HOSTNAME);
                }
                break;
                
            case LVGL_REG_SETTINGS_WIFI_SSID:
                if (settingsTextAreas.wifiTextArea) {
                    const char* text = lv_textarea_get_text(settingsTextAreas.wifiTextArea);
                    prepareSettingsResponse(text, LVGL_REG_SETTINGS_WIFI_SSID);
                }
                break;
                
            case LVGL_REG_SETTINGS_WIFI_PASSWORD:
                if (settingsTextAreas.wifiPasswordTextArea) {
                    const char* text = lv_textarea_get_text(settingsTextAreas.wifiPasswordTextArea);
                    prepareSettingsResponse(text, LVGL_REG_SETTINGS_WIFI_PASSWORD);
                }
                break;
        }
    }
    
    requestedRegister = 0;  // Reset for next request
}

// Helper function to prepare the next response
void prepareSettingsResponse(const char* text, uint8_t reg) {
    if (text) {
        uint8_t len = strlen(text);
        if (len > 0 && len < 30) {  // Leave room for register and length bytes
            responseBuffer[0] = reg;
            responseBuffer[1] = len;
            memcpy(&responseBuffer[2], text, len);
            responseLength = len + 2;
        }
    }
}
#endif