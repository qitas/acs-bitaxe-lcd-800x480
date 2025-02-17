#include "modelPresets.h"
#include "BAP.h"
#include "I2CData.h"
#include "modelConfig.h"
#include "NVS.h"

uint16_t currentPresetFrequency = 0;
uint16_t currentPresetVoltage = 0;
uint8_t currentPresetFanSpeed = 0;
bool currentPresetAutoFanMode = false;

#if (BitaxeGamma == 1)
const uint16_t voltageLowPower = 1030;
const uint16_t voltageNormalPower = 1090;
const uint16_t voltageHighPower = 1150;

const uint16_t freqLowPower = 420;
const uint16_t freqNormalPower = 490;
const uint16_t freqHighPower = 700;

const uint8_t fanSpeedLowPower = 27;
const uint8_t fanSpeedNormalPower = 35;
const uint8_t fanSpeedHighPower = 0;

const uint8_t autoFanModeLowPower = 0;
const uint8_t autoFanModeNormalPower = 0;
const uint8_t autoFanModeHighPower = 1;
#endif

#if (BitaxeUltra == 1)
static const uint16_t voltageLowPower = 1130;
static const uint16_t voltageNormalPower = 1190;
static const uint16_t voltageHighPower = 1250;

static const uint16_t freqLowPower = 420;
static const uint16_t freqNormalPower = 490;
static const uint16_t freqHighPower = 625;

static const uint8_t fanSpeedLowPower = 27;
static const uint8_t fanSpeedNormalPower = 35;
static const uint8_t fanSpeedHighPower = 0;

static const uint8_t autoFanModeLowPower = 0;
static const uint8_t autoFanModeNormalPower = 0;
static const uint8_t autoFanModeHighPower = 1;
#endif

void setLowPowerPreset() 
{
 Serial.println("Low Power Mode Selected");

    #if (BitaxeGamma == 1 && BAPPORT == 1)
                // fan speed
                memset(BAPFanSpeedBuffer, 0, BAP_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAutoFanSpeedBuffer, 0, BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAsicVoltageBuffer, 0, BAP_ASIC_VOLTAGE_BUFFER_SIZE);
                memset(BAPAsicFreqBuffer, 0, BAP_ASIC_FREQ_BUFFER_SIZE);
                delay(10);
                BAPFanSpeedBuffer[0] = 0x00;
                BAPFanSpeedBuffer[1] = 27; // 27%
                // auto fan speed
                
                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = 0x00; // Auto Fan Speed off
                currentPresetFanSpeed = 27;
                currentPresetAutoFanMode = 0;

                // asic voltage
                 
                uint16_t voltageNumber = voltageLowPower;  // Convert string to number
                uint8_t voltageBytes[2] = {
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);
                currentPresetVoltage = voltageLowPower;
                // asic freq
                
                uint16_t freqNumber = freqLowPower;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
                currentPresetFrequency = freqLowPower;
                //writeDataToBAP(freqBytes, 2, BAP_ASIC_FREQ_BUFFER_REG);
                delay(10);

       
                //writeDataToBAP(BAPFanSpeedBuffer, 2, BAP_FAN_SPEED_BUFFER_REG);
                //writeDataToBAP(BAPAutoFanSpeedBuffer, 2, BAP_AUTO_FAN_SPEED_BUFFER_REG);
    #elif (BitaxeUltra == 1 && BAPPORT == 1)
// fan speed
                memset(BAPFanSpeedBuffer, 0, BAP_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAutoFanSpeedBuffer, 0, BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAsicVoltageBuffer, 0, BAP_ASIC_VOLTAGE_BUFFER_SIZE);
                memset(BAPAsicFreqBuffer, 0, BAP_ASIC_FREQ_BUFFER_SIZE);
                delay(10);
                BAPFanSpeedBuffer[0] = 0x00;
                BAPFanSpeedBuffer[1] = fanSpeedLowPower; // 27%
                // auto fan speed
                
                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = 0x00; // Auto Fan Speed off
                currentPresetFanSpeed = 27;
                currentPresetAutoFanMode = 0;


                // asic voltage
                 
                uint16_t voltageNumber = voltageLowPower;  // Convert string to number
                uint8_t voltageBytes[2] = {
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);
                currentPresetVoltage = voltageLowPower;
                // asic freq
                
                uint16_t freqNumber = freqLowPower;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
                currentPresetFrequency = freqLowPower;
                //writeDataToBAP(freqBytes, 2, BAP_ASIC_FREQ_BUFFER_REG);
                delay(10);

       
                //writeDataToBAP(BAPFanSpeedBuffer, 2, BAP_FAN_SPEED_BUFFER_REG);
                //writeDataToBAP(BAPAutoFanSpeedBuffer, 2, BAP_AUTO_FAN_SPEED_BUFFER_REG);
    #endif
}


void setNormalPowerPreset() 
{
    #if (BitaxeGamma == 1 && BAPPORT == 1)
     Serial.println("Normal Power Mode Selected");
                // fan speed
                memset(BAPFanSpeedBuffer, 0, BAP_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAutoFanSpeedBuffer, 0, BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAsicFreqBuffer, 0, BAP_ASIC_FREQ_BUFFER_SIZE);
                memset(BAPAsicVoltageBuffer, 0, BAP_ASIC_VOLTAGE_BUFFER_SIZE);
                delay(10);
                BAPFanSpeedBuffer[0] = 0x00;
                BAPFanSpeedBuffer[1] = fanSpeedNormalPower; // 35%   
                // auto fan speed
                
                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = autoFanModeNormalPower; // Auto Fan Speed off
                currentPresetFanSpeed = 35;
                currentPresetAutoFanMode = 0;

                // asic voltage
                uint16_t voltageNumber = voltageNormalPower;  // Convert string to number
                uint8_t voltageBytes[2] = { 
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);
                currentPresetVoltage = voltageNormalPower;


                // asic freq
                
                uint16_t freqNumber = freqNormalPower;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
                currentPresetFrequency = freqNormalPower;
                //writeDataToBAP(freqBytes, 2, BAP_ASIC_FREQ_BUFFER_REG);
                delay(10);


                //writeDataToBAP(BAPFanSpeedBuffer, 2, BAP_FAN_SPEED_BUFFER_REG);
                //writeDataToBAP(BAPAutoFanSpeedBuffer, 2, BAP_AUTO_FAN_SPEED_BUFFER_REG);
    #elif (BitaxeUltra == 1 && BAPPORT == 1)
     Serial.println("Normal Power Mode Selected");
                // fan speed
                memset(BAPFanSpeedBuffer, 0, BAP_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAutoFanSpeedBuffer, 0, BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAsicFreqBuffer, 0, BAP_ASIC_FREQ_BUFFER_SIZE);
                memset(BAPAsicVoltageBuffer, 0, BAP_ASIC_VOLTAGE_BUFFER_SIZE);
                delay(10);
                BAPFanSpeedBuffer[0] = 0x00;
                BAPFanSpeedBuffer[1] = fanSpeedNormalPower; // 35%   
                // auto fan speed
                
                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = autoFanModeNormalPower; // Auto Fan Speed off
                currentPresetFanSpeed = 35;
                currentPresetAutoFanMode = 0;

                // asic voltage
                uint16_t voltageNumber = voltageNormalPower;  // Convert string to number
                uint8_t voltageBytes[2] = { 
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);
                currentPresetVoltage = voltageNormalPower;
                // asic freq
                
                uint16_t freqNumber = freqNormalPower;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
                currentPresetFrequency = freqNormalPower;
                //writeDataToBAP(freqBytes, 2, BAP_ASIC_FREQ_BUFFER_REG);
                delay(10);


                //writeDataToBAP(BAPFanSpeedBuffer, 2, BAP_FAN_SPEED_BUFFER_REG);
                //writeDataToBAP(BAPAutoFanSpeedBuffer, 2, BAP_AUTO_FAN_SPEED_BUFFER_REG);

    #endif
}

void setHighPowerPreset() 
{
    #if (BitaxeGamma == 1 && BAPPORT == 1)
    Serial.println("High Power Mode Selected");
                // fan speed
                memset(BAPFanSpeedBuffer, 0, BAP_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAutoFanSpeedBuffer, 0, BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAsicVoltageBuffer, 0, BAP_ASIC_VOLTAGE_BUFFER_SIZE);
                memset(BAPAsicFreqBuffer, 0, BAP_ASIC_FREQ_BUFFER_SIZE);
                delay(10);

                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = autoFanModeHighPower; // Auto Fan Speed
                currentPresetFanSpeed = 0;
                currentPresetAutoFanMode = 1;
                //writeDataToBAP(BAPAutoFanSpeedBuffer, 2, BAP_AUTO_FAN_SPEED_BUFFER_REG);
                // asic voltage
                uint16_t voltageNumber = voltageHighPower;  // Convert string to number
                uint8_t voltageBytes[2] = {
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);
                currentPresetVoltage = voltageHighPower;
                // asic freq
                
                uint16_t freqNumber = freqHighPower;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
                currentPresetFrequency = freqHighPower;
    #elif (BitaxeUltra == 1 && BAPPORT == 1)
    Serial.println("High Power Mode Selected");
                // fan speed
                memset(BAPFanSpeedBuffer, 0, BAP_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAutoFanSpeedBuffer, 0, BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAsicVoltageBuffer, 0, BAP_ASIC_VOLTAGE_BUFFER_SIZE);
                memset(BAPAsicFreqBuffer, 0, BAP_ASIC_FREQ_BUFFER_SIZE);
                delay(10);

                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = autoFanModeHighPower; // Auto Fan Speed
                currentPresetFanSpeed = 0;
                currentPresetAutoFanMode = 1;
                //writeDataToBAP(BAPAutoFanSpeedBuffer, 2, BAP_AUTO_FAN_SPEED_BUFFER_REG);
                // asic voltage
                uint16_t voltageNumber = voltageHighPower;  // Convert string to number
                uint8_t voltageBytes[2] = {
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);
                currentPresetVoltage = voltageHighPower;
                // asic freq
                
                uint16_t freqNumber = freqHighPower;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
                currentPresetFrequency = freqHighPower;

    #endif
}

void readCurrentPresetSettingsFromNVS() {
    char currentPresetFrequencyString[16];
    char currentPresetVoltageString[16];
    char currentPresetFanSpeedString[16];
    char currentPresetAutoFanModeString[16];
    loadSettingsFromNVSasString(NVS_KEY_ASIC_CURRENT_FREQ, currentPresetFrequencyString , sizeof(currentPresetFrequencyString));
    currentPresetFrequency = atoi(currentPresetFrequencyString);
    delay(10);
    ESP_LOGI("ASIC", "Current Preset Frequency: %d", currentPresetFrequency);
    loadSettingsFromNVSasString(NVS_KEY_ASIC_CURRENT_VOLTAGE, currentPresetVoltageString , sizeof(currentPresetVoltageString));
    currentPresetVoltage = atoi(currentPresetVoltageString);
    delay(10);
    ESP_LOGI("ASIC", "Current Preset Voltage: %d", currentPresetVoltage);
    loadSettingsFromNVSasString(NVS_KEY_ASIC_CURRENT_FAN_SPEED, currentPresetFanSpeedString , sizeof(currentPresetFanSpeedString));
    currentPresetFanSpeed = atoi(currentPresetFanSpeedString);
    delay(10);
    ESP_LOGI("ASIC", "Current Preset Fan Speed: %d", currentPresetFanSpeed);
    loadSettingsFromNVSasString(NVS_KEY_ASIC_CURRENT_AUTO_FAN_SPEED, currentPresetAutoFanModeString , sizeof(currentPresetAutoFanModeString));
    currentPresetAutoFanMode = atoi(currentPresetAutoFanModeString);
    delay(10);
    ESP_LOGI("ASIC", "Current Preset Auto Fan Mode: %d", currentPresetAutoFanMode);
}
