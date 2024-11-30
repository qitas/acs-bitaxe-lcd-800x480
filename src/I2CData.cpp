#include "I2CData.h"
#include <Arduino.h>
#include "esp_mac.h"
#include <algorithm>
#include <TimeLib.h>

// Custom min function because the standard one is not available 
#define __min(a,b) ((a)<(b)?(a):(b))

// Global variables
uint32_t PSRAM_ATTR byteCount = 0;
TwoWire Wire2 = TwoWire(1);
I2CDataContainer PSRAM_ATTR i2cData = {0};

static bool i2cNeedsReset = false;
static uint32_t lastErrorTime = 0;
static uint32_t errorCount = 0;
#define ERROR_THRESHOLD 25        // Number of errors before forcing reset

uint8_t* i2cBuffer = nullptr;

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
    
    //memset(&i2cData, 0, sizeof(I2CDataContainer));
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
            memset(i2cData.network.ssid, 0, MAX_SSID_LENGTH);
            memcpy(i2cData.network.ssid, &buffer[2], __min(dataLen, MAX_SSID_LENGTH - 1));
            break;
        case LVGL_REG_IP_ADDR:
            memset(i2cData.network.ipAddress, 0, MAX_IP_LENGTH);
            memcpy(i2cData.network.ipAddress, &buffer[2], __min(dataLen, MAX_IP_LENGTH - 1));
            break;
        case LVGL_REG_WIFI_STATUS:
            memset(i2cData.network.wifiStatus, 0, MAX_STATUS_LENGTH);
            memcpy(i2cData.network.wifiStatus, &buffer[2], __min(dataLen, MAX_STATUS_LENGTH - 1));
            break;
        case LVGL_REG_POOL_URL:
            memset(i2cData.network.poolUrl, 0, MAX_URL_LENGTH);
            memcpy(i2cData.network.poolUrl, &buffer[2], __min(dataLen, MAX_URL_LENGTH - 1));
            break;
        case LVGL_REG_FALLBACK_URL:
            memset(i2cData.network.fallbackUrl, 0, MAX_URL_LENGTH);
            memcpy(i2cData.network.fallbackUrl, &buffer[2], __min(dataLen, MAX_URL_LENGTH - 1));
            break;
        case LVGL_REG_POOL_PORTS:
                if (dataLen >= 4) 
            {
                i2cData.network.poolPort = (buffer[3] << 8) | buffer[2];
                i2cData.network.fallbackPort = (buffer[5] << 8) | buffer[4];
                Serial.printf("Pool: %s : %d, Fallback: %s : %d\n", i2cData.network.poolUrl, i2cData.network.poolPort, i2cData.network.fallbackUrl, i2cData.network.fallbackPort);
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
            i2cData.mining.hashrate = hashrate;
            Serial.print("Hashrate received: ");
            Serial.println(i2cData.mining.hashrate);
            break;
        }
        case LVGL_REG_HIST_HASHRATE: 
        {
            float historicalHashrate;
            memset(&historicalHashrate, 0, sizeof(float));
            memcpy(&historicalHashrate, &buffer[2], sizeof(float));
            i2cData.mining.historicalHashrate = historicalHashrate;
            Serial.print("Historical Hashrate received: ");
            Serial.println(i2cData.mining.historicalHashrate);
            break;
        }
        case LVGL_REG_EFFICIENCY: 
        {
            float efficiency;
            memset(&efficiency, 0, sizeof(float));
            memcpy(&efficiency, &buffer[2], sizeof(float));
            i2cData.mining.efficiency = efficiency;
            Serial.print("Efficiency received: ");
            Serial.println(i2cData.mining.efficiency);
            break;
        }
        case LVGL_REG_BEST_DIFF:
        {
            memset(i2cData.mining.bestDiff, 0, MAX_DIFF_LENGTH);
            memcpy(i2cData.mining.bestDiff, &buffer[2], __min(dataLen, MAX_DIFF_LENGTH - 1));
            i2cData.mining.bestDiff[dataLen] = '\0';  // Null terminate the string
            Serial.print("Best Diff received: ");
            Serial.println(i2cData.mining.bestDiff);
            break;
        }
        case LVGL_REG_SESSION_DIFF:
        {
            memset(i2cData.mining.sessionDiff, 0, MAX_DIFF_LENGTH);
            memcpy(i2cData.mining.sessionDiff, &buffer[2], __min(dataLen, MAX_DIFF_LENGTH - 1));
            i2cData.mining.sessionDiff[dataLen] = '\0';  // Null terminate the string
            Serial.print("Session Diff received: ");
            Serial.println(i2cData.mining.sessionDiff);
            break;
        }
        case LVGL_REG_SHARES:
        // Each share is 4 bytes, sent in the same I2C sequence. Accepted Shares first, then Rejected Shares
        {
            memset(&i2cData.mining.acceptedShares, 0, sizeof(uint32_t));
            memset(&i2cData.mining.rejectedShares, 0, sizeof(uint32_t));
            i2cData.mining.acceptedShares = (buffer[2] << 0) | (buffer[3] << 8) | (buffer[4] << 16) | (buffer[5] << 24);
            i2cData.mining.rejectedShares = (buffer[6] << 0) | (buffer[7] << 8) | (buffer[8] << 16) | (buffer[9] << 24);
            i2cData.mining.shares = i2cData.mining.acceptedShares + i2cData.mining.rejectedShares;
            Serial.printf("Accepted shares: %d, Rejected shares: %d, Total shares: %d\n", 
                i2cData.mining.acceptedShares, 
                i2cData.mining.rejectedShares,
                i2cData.mining.shares);
            if (i2cData.mining.shares > 0) 
            {
                i2cData.mining.rejectRatePercent = (i2cData.mining.rejectedShares * 100.0f) / i2cData.mining.shares;
            } 
            else 
            {
                i2cData.mining.rejectRatePercent = 0.0f;
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
            i2cData.monitoring.temperatures[0] = temperature;
            Serial.print("Temperature received: ");
            Serial.println(i2cData.monitoring.temperatures[0]);
            break;
        }
        case LVGL_REG_ASIC_FREQ:
        {
            float asicFreq;
            memset(&asicFreq, 0, sizeof(float));
            memcpy(&asicFreq, &buffer[2], sizeof(float));
            i2cData.monitoring.asicFrequency = asicFreq;
            Serial.print("ASIC Frequency received: ");
            Serial.println(i2cData.monitoring.asicFrequency);
            break;
        }
        case LVGL_REG_FAN:
        {
            memset(&i2cData.monitoring.fanSpeed, 0, sizeof(uint16_t));
            memcpy(&i2cData.monitoring.fanSpeed, &buffer[2], __min(dataLen, MAX_UINT16_SIZE));
            break;
        }
        case LVGL_REG_POWER_STATS:
        {
            float powerStatsArray[4];
            memset(powerStatsArray, 0, sizeof(float) * 4);
            memcpy(powerStatsArray, &buffer[2], __min(dataLen, sizeof(float) * 4));
            i2cData.monitoring.powerStats.voltage = powerStatsArray[0];
            i2cData.monitoring.powerStats.current = powerStatsArray[1];
            i2cData.monitoring.powerStats.power = powerStatsArray[2];
            i2cData.monitoring.powerStats.domainVoltage = powerStatsArray[3];
            Serial.printf("Voltage: %f, Current: %f, Power: %f, Domain Voltage: %f\n", 
                i2cData.monitoring.powerStats.voltage,
                i2cData.monitoring.powerStats.current,
                i2cData.monitoring.powerStats.power,
                i2cData.monitoring.powerStats.domainVoltage);
            break;
        }
        case LVGL_REG_ASIC_INFO:
        {
            memset(&i2cData.monitoring.asicInfo, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.monitoring.asicInfo, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            break;
        }
        case LVGL_REG_UPTIME:
        {
            memset(&i2cData.monitoring.uptime, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.monitoring.uptime, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
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
            memset(&i2cData.status.flags, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.status.flags, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            break;
        }
        case LVGL_REG_DEVICE_INFO:
        {
            memset(i2cData.status.deviceInfo, 0, MAX_INFO_LENGTH);
            memcpy(i2cData.status.deviceInfo, &buffer[2], __min(dataLen, MAX_INFO_LENGTH - 1));
            break;
        }
        case LVGL_REG_BOARD_INFO:
        {
            memset(i2cData.status.boardInfo, 0, MAX_INFO_LENGTH);
            memcpy(i2cData.status.boardInfo, &buffer[2], __min(dataLen, MAX_INFO_LENGTH - 1));
            break;
        }
        case LVGL_REG_CLOCK_SYNC:
        {
            uint32_t timestamp;
            memset(&timestamp, 0, MAX_UINT32_SIZE);
            memcpy(&timestamp, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            i2cData.status.clockSync = timestamp; // Subtract 6 hours to account for timezone TODO: make this dynamic
            Serial.printf("Clock sync: %u\n", timestamp);
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
            memset(&i2cData.api.btcPriceUSD, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.btcPriceUSD, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("BTC Price received: %d\n", i2cData.api.btcPriceUSD);
            break;
        }
        case LVGL_REG_API_NETWORK_HASHRATE:
        {
            memset(&i2cData.api.networkHashrate, 0, sizeof(double));
            memcpy(&i2cData.api.networkHashrate, &buffer[2], __min(dataLen, sizeof(double)));
            Serial.printf("Network Hashrate received: %f\n", i2cData.api.networkHashrate);
            break;
        }
        case LVGL_REG_API_NETWORK_DIFFICULTY:
        {
            memset(&i2cData.api.networkDifficulty, 0, sizeof(double));
            memcpy(&i2cData.api.networkDifficulty, &buffer[2], __min(dataLen, sizeof(double)));
            Serial.printf("Network Difficulty received: %f\n", i2cData.api.networkDifficulty);
            break;
        }
        case LVGL_REG_API_BLOCK_HEIGHT:
        {
            memset(&i2cData.api.blockHeight, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.blockHeight, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Block Height received: %d\n", i2cData.api.blockHeight);
            break;
        }
        case LVGL_REG_API_DIFFICULTY_PROGRESS:
        {
            memset(&i2cData.api.difficultyProgressPercent, 0, sizeof(double));
            memcpy(&i2cData.api.difficultyProgressPercent, &buffer[2], __min(dataLen, sizeof(double)));
            Serial.printf("Difficulty Progress Percent received: %f\n", i2cData.api.difficultyProgressPercent);
            break;
        }
        case LVGL_REG_API_DIFFICULTY_CHANGE:
        {
            memset(&i2cData.api.difficultyChangePercent, 0, sizeof(double));
            memcpy(&i2cData.api.difficultyChangePercent, &buffer[2], __min(dataLen, sizeof(double)));
            Serial.printf("Difficulty Change Percent received: %f\n", i2cData.api.difficultyChangePercent);
            break;
        }
        case LVGL_REG_API_REMAINING_BLOCKS:
        {
            memset(&i2cData.api.remainingBlocksToDifficultyAdjustment, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.remainingBlocksToDifficultyAdjustment, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Remaining Blocks to Difficulty Adjustment received: %d\n", i2cData.api.remainingBlocksToDifficultyAdjustment);
            break;
        }
        case LVGL_REG_API_REMAINING_TIME:
        {
            memset(&i2cData.api.remainingTimeToDifficultyAdjustment, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.remainingTimeToDifficultyAdjustment, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Remaining Time to Difficulty Adjustment received: %d\n", i2cData.api.remainingTimeToDifficultyAdjustment);
            break;
        }
        case LVGL_REG_API_FASTEST_FEE:
        {
            memset(&i2cData.api.fastestFee, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.fastestFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Fastest Fee received: %d\n", i2cData.api.fastestFee);
            break;
        }
        case LVGL_REG_API_HALF_HOUR_FEE:
        {
            memset(&i2cData.api.halfHourFee, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.halfHourFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Half Hour Fee received: %d\n", i2cData.api.halfHourFee);
            break;
        }
        case LVGL_REG_API_HOUR_FEE:
        {
            memset(&i2cData.api.hourFee, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.hourFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Hour Fee received: %d\n", i2cData.api.hourFee);
            break;
        }
        case LVGL_REG_API_ECONOMY_FEE:
        {
            memset(&i2cData.api.economyFee, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.economyFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Economy Fee received: %d\n", i2cData.api.economyFee);
            break;
        }
        case LVGL_REG_API_MINIMUM_FEE:
        {
            memset(&i2cData.api.minimumFee, 0, MAX_UINT32_SIZE);
            memcpy(&i2cData.api.minimumFee, &buffer[2], __min(dataLen, MAX_UINT32_SIZE));
            Serial.printf("Minimum Fee received: %d\n", i2cData.api.minimumFee);
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

// Modified onReceive function
void onReceive(int len) 
{
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
        Serial.printf("Error count: %d\n", errorCount);
        
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
        Serial.printf("Error count: %d\n", errorCount);
        if (errorCount >= ERROR_THRESHOLD) 
        {
            Serial.println("Error threshold reached, scheduling I2C reset");
            i2cNeedsReset = true;
        }
        return;
    }

    // If we got here, we have a valid message
    //errorCount = 0;  // Reset error count on successful message

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
        Serial.printf("Error count: %d\n", errorCount);
    }
}

// This is called when the master requests data from the slave 
// Not Implemented yet
// I would like to be able to send wifi data to the master on request for an alternative to HTML page
void onRequest() 
{
    Wire2.print(byteCount++);
    Wire2.print(" Packets.");
    Serial.println("onRequest");
}

// Initialize the I2C slave on Wire2 Since Wire is already used for the display touchscreen
void initI2CSlave() 
{
    Wire2.onReceive(onReceive);
    Wire2.onRequest(onRequest);
    Wire2.setPins(i2cSlaveSDA, i2cSlaveSCL);
    Wire2.setBufferSize(I2C_BUFFER_SIZE);
    Wire2.begin((uint8_t)i2cSlaveAddress);
}

// Keep the clock time updated
uint32_t keepClockTime() 
{
    static portMUX_TYPE timeMux = portMUX_INITIALIZER_UNLOCKED;
    static uint32_t lastSyncMillis = 0;
    static uint32_t lastSyncTime = 0;
    uint32_t currentTime;

    portENTER_CRITICAL(&timeMux);
    
    if (i2cData.status.clockSync > lastSyncTime) 
    {
        lastSyncMillis = millis();
        lastSyncTime = i2cData.status.clockSync;
    }
    
    if (lastSyncMillis == 0) 
    {
        lastSyncMillis = millis();
        currentTime = i2cData.status.clockSync;
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
