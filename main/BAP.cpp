#include "BAP.h"
#include "modelConfig.h"
#if BAPPORT == 1

uint8_t* BAPReadBuffer = nullptr;
uint8_t* BAPHostnameBuffer = nullptr;
uint8_t* BAPWifiSSIDBuffer = nullptr;
uint8_t* BAPWifiPassBuffer = nullptr;
uint8_t* BAPStratumUrlMainBuffer = nullptr;
uint8_t* BAPStratumPortMainBuffer = nullptr;
uint8_t* BAPStratumUserMainBuffer = nullptr;
uint8_t* BAPStratumPassMainBuffer = nullptr;
uint8_t* BAPStratumUrlFallbackBuffer = nullptr;
uint8_t* BAPStratumPortFallbackBuffer = nullptr;
uint8_t* BAPStratumUserFallbackBuffer = nullptr;
uint8_t* BAPStratumPassFallbackBuffer = nullptr;
uint8_t* BAPAsicVoltageBuffer = nullptr;
uint8_t* BAPAsicFreqBuffer = nullptr;
uint8_t* BAPFanSpeedBuffer = nullptr;
uint8_t* BAPAutoFanSpeedBuffer = nullptr;

SpecialRegisters specialRegisters = {0};
IncomingDataContainer PSRAM_ATTR IncomingData = {0};

#define FLAG_CONFIRMATION_COUNT 3 
uint8_t overheatModeCounter = 0;
uint8_t foundBlockCounter = 0;
bool confirmedOverheatMode = false;
bool confirmedFoundBlock = false;

#define __min(a,b) ((a)<(b)?(a):(b))

void initializeBAPBuffers()
{
    BAPReadBuffer = (uint8_t*)malloc(BAPReadBufferLength);
    BAPHostnameBuffer = (uint8_t*)malloc(BAP_HOSTNAME_BUFFER_SIZE);
    BAPWifiSSIDBuffer = (uint8_t*)malloc(BAP_WIFI_SSID_BUFFER_SIZE);
    BAPWifiPassBuffer = (uint8_t*)malloc(BAP_WIFI_PASS_BUFFER_SIZE);
    BAPStratumUrlMainBuffer = (uint8_t*)malloc(BAP_STRATUM_URL_MAIN_BUFFER_SIZE);
    BAPStratumPortMainBuffer = (uint8_t*)malloc(BAP_STRATUM_PORT_MAIN_BUFFER_SIZE);
    BAPStratumUserMainBuffer = (uint8_t*)malloc(BAP_STRATUM_USER_MAIN_BUFFER_SIZE);
    BAPStratumPassMainBuffer = (uint8_t*)malloc(BAP_STRATUM_PASS_MAIN_BUFFER_SIZE);
    BAPStratumUrlFallbackBuffer = (uint8_t*)malloc(BAP_STRATUM_URL_FALLBACK_BUFFER_SIZE);
    BAPStratumPortFallbackBuffer = (uint8_t*)malloc(BAP_STRATUM_PORT_FALLBACK_BUFFER_SIZE);
    BAPStratumUserFallbackBuffer = (uint8_t*)malloc(BAP_STRATUM_USER_FALLBACK_BUFFER_SIZE);
    BAPStratumPassFallbackBuffer = (uint8_t*)malloc(BAP_STRATUM_PASS_FALLBACK_BUFFER_SIZE);
    BAPAsicVoltageBuffer = (uint8_t*)malloc(BAP_ASIC_VOLTAGE_BUFFER_SIZE);
    BAPAsicFreqBuffer = (uint8_t*)malloc(BAP_ASIC_FREQ_BUFFER_SIZE);
    BAPFanSpeedBuffer = (uint8_t*)malloc(BAP_FAN_SPEED_BUFFER_SIZE);
    BAPAutoFanSpeedBuffer = (uint8_t*)malloc(BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
    memset(BAPFanSpeedBuffer, 0, BAP_FAN_SPEED_BUFFER_SIZE);
    memset(BAPAutoFanSpeedBuffer, 0, BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
    memset(BAPAsicVoltageBuffer, 0, BAP_ASIC_VOLTAGE_BUFFER_SIZE);
    memset(BAPAsicFreqBuffer, 0, BAP_ASIC_FREQ_BUFFER_SIZE);
}

void setupBAP()
{
    Serial2.begin(115200, SERIAL_8N1, BAP_RX, BAP_TX);
    uint32_t startTime = millis();
    while (!Serial2)
    {
        if (millis() - startTime > 5000) {  // 5 second timeout
            ESP_LOGE("BAP", "Failed to initialize Serial2");
            return;
        }
        yield();
    }
    ESP_LOGI("BAP", "BAP Setup Complete - RX: %d, TX: %d", BAP_RX, BAP_TX);
}

void readDataFromBAP()
{
    // Check if there's actually data available
    if (!Serial2.available()) {
        return;
    }

    uint16_t dataLength = 0;
    uint32_t startTime = millis();
    
    // Clear the read buffer before reading new data
    memset(BAPReadBuffer, 0, BAPReadBufferLength);
    
    // Read first byte (should be register)
    int firstByte = Serial2.read();
    if (firstByte == -1) {
        Serial0.println("Failed to read register byte");
        return;
    }
    
    BAPReadBuffer[0] = (uint8_t)firstByte;
    dataLength++;

    // Wait for length byte
    while (!Serial2.available()) {
        if (millis() - startTime > 1000) {  // 1 second timeout
            Serial0.println("Timeout waiting for length byte");
            return;
        }
        yield();
    }

    // Read length byte
    int lengthByte = Serial2.read();
    if (lengthByte == -1) {
        Serial0.println("Failed to read length byte");
        return;
    }

    BAPReadBuffer[1] = (uint8_t)lengthByte;
    dataLength++;

    // Read remaining data based on length byte
    uint8_t expectedLength = (uint8_t)lengthByte;
    
    // Sanity check on expected length
    if (expectedLength > BAPReadBufferLength - 2) {
        Serial0.printf("Expected length too large: %d", expectedLength);
        return;
    }

    // Read the rest of the data
    while (dataLength < (expectedLength + 2) && (millis() - startTime < 1000)) {
        if (Serial2.available()) {
            int byte = Serial2.read();
            if (byte != -1) {
                BAPReadBuffer[dataLength++] = (uint8_t)byte;
            }
        }
        yield();
    }

    // Validate the data
    if (dataLength == BAPReadBuffer[1]+2)
    {
        //Print Success Messgae
        Serial0.println("BAP Data Length Validated");

        // Process the data
        uint8_t reg = BAPReadBuffer[0];
        uint8_t dataLen = BAPReadBuffer[1];
        uint8_t* data = BAPReadBuffer + 2;

        // Process the data based on the register
        if (reg >= 0x21 && reg <= 0x26) 
        {
            handleNetworkDataSerial(BAPReadBuffer, dataLen);
            Serial0.printf("Network Data Received for register 0x%02X", reg);
            Serial0.printf("Raw Data: ");
            for (int i = 0; i < dataLen + 2; i++)
            {
                Serial0.printf("%02X ", BAPReadBuffer[i]);
            }
        }
        else if (reg >= 0x30 && reg <= 0x35) 
        {
            handleMiningDataSerial(BAPReadBuffer, dataLen);
            Serial0.printf("Mining Data Received for register 0x%02X", reg);
            Serial0.printf("Raw Data: ");
            for (int i = 0; i < dataLen + 2; i++)
            {
                Serial.printf("%02X ", BAPReadBuffer[i]);
            }
        }
        else if (reg >= 0x40 && reg <= 0x46) 
        {
            handleMonitoringDataSerial(BAPReadBuffer, dataLen);
            Serial0.printf("Monitoring Data Received for register 0x%02X", reg);
            Serial0.printf("Raw Data: ");
            for (int i = 0; i < dataLen + 2; i++)
            {
                Serial.printf("%02X ", BAPReadBuffer[i]);
            }
        }
        else if (reg >= 0x50 && reg <= 0x54) 
        {
            handleDeviceStatusSerial(BAPReadBuffer, dataLen);
            Serial0.printf("Device Status Data Received for register 0x%02X", reg);
            Serial0.printf("Raw Data: ");
            for (int i = 0; i < dataLen + 2; i++)
            {
                Serial0.printf("%02X ", BAPReadBuffer[i]);
            }
        }
        else if (reg >= 0x60 && reg <= 0x6C) 
        {
            handleAPIDataSerial(BAPReadBuffer, dataLen);
            Serial0.printf("API Data Received for register 0x%02X", reg);
            Serial0.printf("Raw Data: ");
            for (int i = 0; i < dataLen + 2; i++)
            {
                Serial0.printf("%02X ", BAPReadBuffer[i]);
            }
        }
        else if (reg >= 0xE0 && reg <= 0xEF) 
        {
            handleFlagsDataSerial(BAPReadBuffer, dataLen);
        }
        else if (reg >= 0xF0 && reg <= 0xFF) 
        {
            handleSpecialRegistersSerial(BAPReadBuffer, dataLen);
        }
        else 
        {
            Serial0.printf("Error: Register 0x%02X outside of valid ranges", reg);
            Serial0.printf("Raw Data: ");
            for (int i = 0; i < dataLen + 2; i++)
            {
                Serial0.printf("%02X ", BAPReadBuffer[i]);
            }
        }
    }
    else
    {
        //Print Error Message
        Serial0.printf("BAP Data Length Invalid");
        Serial0.printf("Register: 0x%02X", BAPReadBuffer[0]);
        Serial0.printf("Expected Length: %d, got %d", BAPReadBuffer[1]+2, dataLength);
    }
}

void writeDataToBAP(uint8_t* buffer, size_t dataLen, uint8_t reg) {
    const size_t chunkSize = 32;
    uint32_t serialWriteStartTime = millis();
    
    // Prepare header: preamble (0xFF 0xAA), register, length
    uint8_t header[4] = {
        0xFF,           // Preamble byte
        0xAA,           // Preamble byte
        (uint8_t)dataLen,  // Length byte
        reg             // Register byte
    };

    // Write header first
    Serial2.write(header, 4);
    delay(1); // Small delay after header

    // Write data in chunks
    size_t bytesWritten = 0;
    while (bytesWritten < dataLen) {
        // Calculate size of next chunk
        size_t remainingBytes = dataLen - bytesWritten;
        size_t currentChunkSize = __min(remainingBytes, chunkSize);

        // Wait for serial to be ready
        while (Serial2.availableForWrite() < currentChunkSize) {
            if (millis() - serialWriteStartTime > 1000) {
                Serial0.println("Timeout waiting for BAP to be ready");
                return;
            }
            yield();
        }

        // Write chunk
        Serial2.write(&buffer[bytesWritten], currentChunkSize);
        bytesWritten += currentChunkSize;

        // Debug output
        Serial0.printf("Chunk written for reg 0x%02X: %d bytes (Total: %d/%d)\n", 
                     reg, currentChunkSize, bytesWritten, dataLen);
        
        delay(1); // Small delay between chunks
    }

    // Debug output
    Serial.printf("Write complete - Preamble: 0xAA, Register: 0x%02X, Length: %d\n", reg, dataLen);
    Serial.print("Data: ");
    for (int i = 0; i < dataLen; i++) {
        Serial.printf("%02X ", buffer[i]);
    }
    Serial.printf("\nTotal time: %lu ms\n", millis() - serialWriteStartTime);
    delay(500); // Small delay after write in case of multiple writes
}

void handleNetworkDataSerial(uint8_t* buffer, uint8_t len)
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

void handleMonitoringDataSerial(uint8_t* buffer, uint8_t len) 
{
    if (len < 2) return;
    
    uint8_t reg = buffer[0];
    uint8_t dataLen = buffer[1];
    
    switch(reg) {
        case LVGL_REG_TEMPS: 
        {
            // Create union to safely handle the byte-to-float conversion
            union {
                uint8_t bytes[4];
                float value;
            } temp;

            // Read bytes in correct order (assuming little-endian)
            temp.bytes[0] = buffer[6];
            temp.bytes[1] = buffer[7];
            temp.bytes[2] = buffer[8];
            temp.bytes[3] = buffer[9];

            IncomingData.monitoring.temperatures[0] = temp.value;
            Serial.printf("Temperature received: %.2f°C\n", IncomingData.monitoring.temperatures[0]);
            break;
        }
        case LVGL_REG_ASIC_FREQ:
        {
            float asicFreq;
            memset(&asicFreq, 0, sizeof(float));
            memcpy(&asicFreq, &buffer[2], sizeof(float));
            IncomingData.monitoring.asicFrequency = asicFreq;
            Serial.print("ASIC Frequency received: ");
            Serial.printf("%lu\n", IncomingData.monitoring.asicFrequency);
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
        case LVGL_REG_TARGET_VOLTAGE:
        {
            memset(&IncomingData.monitoring.targetDomainVoltage, 0, MAX_UINT32_SIZE);
            memcpy(&IncomingData.monitoring.targetDomainVoltage, &buffer[2], __min(dataLen, MAX_UINT16_SIZE));
            break;
        }
        default:
        {   
            Serial.printf("Warning: Unknown monitoring register 0x%02X with length %d\n", reg, dataLen);
            break;
        }
    }
}

void handleDeviceStatusSerial(uint8_t* buffer, uint8_t len) 
{
    // if (len < 2) return;
    
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

void handleMiningDataSerial(uint8_t* buffer, uint8_t len) 
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

void handleAPIDataSerial(uint8_t* buffer, uint8_t len) 
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

void handleFlagsDataSerial(uint8_t* buffer, uint8_t len)
{
    //if (len < 2) return;
    
    uint8_t reg = buffer[0];
    uint8_t dataLen = buffer[1];

    switch(reg)
    {
        case LVGL_FLAG_STARTUP_DONE:
        {
            specialRegisters.startupDone = buffer[2];
            Serial.printf("Startup Done flag received: %d\n", specialRegisters.startupDone);
            break;
        }
        case LVGL_FLAG_OVERHEAT_MODE:
        {
            specialRegisters.overheatMode = buffer[2];  // Keep raw incoming data
            bool currentFlag = buffer[2];
            
            if (currentFlag) {
                overheatModeCounter++;
                if (overheatModeCounter >= FLAG_CONFIRMATION_COUNT && !confirmedOverheatMode) {
                    confirmedOverheatMode = true;
                    ESP_LOGI("BAP", "Overheat Mode confirmed after %d consecutive readings", FLAG_CONFIRMATION_COUNT);
                }
            } else {
                overheatModeCounter = 0;
                if (confirmedOverheatMode) {
                    confirmedOverheatMode = false;
                    ESP_LOGI("BAP", "Overheat Mode cleared");
                }
            }
            ESP_LOGD("BAP", "Overheat Mode reading: %d, counter: %d, confirmed: %d", 
                     currentFlag, overheatModeCounter, confirmedOverheatMode);
            break;
        }
        case LVGL_FLAG_FOUND_BLOCK:
        {
            specialRegisters.foundBlock = buffer[2];  // Keep raw incoming data
            bool currentFlag = buffer[2];
            
            if (currentFlag) {
                foundBlockCounter++;
                if (foundBlockCounter >= FLAG_CONFIRMATION_COUNT && !confirmedFoundBlock) {
                    confirmedFoundBlock = true;
                    ESP_LOGI("BAP", "Found Block confirmed after %d consecutive readings", FLAG_CONFIRMATION_COUNT);
                }
            } else {
                foundBlockCounter = 0;
                if (confirmedFoundBlock) {
                    confirmedFoundBlock = false;
                    ESP_LOGI("BAP", "Found Block cleared");
                }
            }
            ESP_LOGD("BAP", "Found Block reading: %d, counter: %d, confirmed: %d", 
                     currentFlag, foundBlockCounter, confirmedFoundBlock);
            break;
        }
        default:
        {
            Serial.printf("Warning: Unknown flags register 0x%02X with length %d\n", reg, dataLen);
            break;
        }
    }
}   

void handleSpecialRegistersSerial(uint8_t* buffer, uint8_t len)
{
    return;
}

void sendRestartToBAP()
{
    if (specialRegisters.restart == 1)
    {
        writeDataToBAP(&specialRegisters.restart, sizeof(specialRegisters.restart), LVGL_REG_SPECIAL_RESTART);
        specialRegisters.restart = 0;
        Serial.println("Restart command sent to BAP");
        delay(5000);
        esp_restart();
    }
    else
    {
        Serial.println("No restart command to send to BAP");
    }
    return;
}

#endif