#include "modelPresets.h"
#include "BAP.h"
#include "I2CData.h"
#include "modelConfig.h"

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

                // asic voltage
                 
                uint16_t voltageNumber = 1030;  // Convert string to number
                uint8_t voltageBytes[2] = {
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);
                // asic freq
                
                uint16_t freqNumber = 420;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
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
                BAPFanSpeedBuffer[1] = 27; // 27%
                // auto fan speed
                
                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = 0x00; // Auto Fan Speed off

                // asic voltage
                 
                uint16_t voltageNumber = 1130;  // Convert string to number
                uint8_t voltageBytes[2] = {
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);
                // asic freq
                
                uint16_t freqNumber = 420;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
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
                BAPFanSpeedBuffer[1] = 35; // 35%   
                // auto fan speed
                
                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = 0x00; // Auto Fan Speed off

                // asic voltage
                uint16_t voltageNumber = 1090;  // Convert string to number
                uint8_t voltageBytes[2] = { 
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);

                // asic freq
                
                uint16_t freqNumber = 490;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
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
                BAPFanSpeedBuffer[1] = 35; // 35%   
                // auto fan speed
                
                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = 0x00; // Auto Fan Speed off

                // asic voltage
                uint16_t voltageNumber = 1150;  // Convert string to number
                uint8_t voltageBytes[2] = { 
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);

                // asic freq
                
                uint16_t freqNumber = 485;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
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
                BAPAutoFanSpeedBuffer[1] = 0x01; // Auto Fan Speed
                //writeDataToBAP(BAPAutoFanSpeedBuffer, 2, BAP_AUTO_FAN_SPEED_BUFFER_REG);
                // asic voltage
                uint16_t voltageNumber = 1170;  // Convert string to number
                uint8_t voltageBytes[2] = {
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);

                // asic freq
                
                uint16_t freqNumber = 750;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);
    #elif (BitaxeUltra == 1 && BAPPORT == 1)
    Serial.println("High Power Mode Selected");
                // fan speed
                memset(BAPFanSpeedBuffer, 0, BAP_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAutoFanSpeedBuffer, 0, BAP_AUTO_FAN_SPEED_BUFFER_SIZE);
                memset(BAPAsicVoltageBuffer, 0, BAP_ASIC_VOLTAGE_BUFFER_SIZE);
                memset(BAPAsicFreqBuffer, 0, BAP_ASIC_FREQ_BUFFER_SIZE);
                delay(10);

                BAPAutoFanSpeedBuffer[0] = 0x00;
                BAPAutoFanSpeedBuffer[1] = 0x01; // Auto Fan Speed
                //writeDataToBAP(BAPAutoFanSpeedBuffer, 2, BAP_AUTO_FAN_SPEED_BUFFER_REG);
                // asic voltage
                uint16_t voltageNumber = 1250;  // Convert string to number
                uint8_t voltageBytes[2] = {
                    (uint8_t)(voltageNumber >> 8),    // High byte
                    (uint8_t)(voltageNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicVoltageBuffer, voltageBytes, 2);

                // asic freq
                
                uint16_t freqNumber = 625;  // Convert string to number
                uint8_t freqBytes[2] = {
                    (uint8_t)(freqNumber >> 8),    // High byte
                    (uint8_t)(freqNumber & 0xFF)   // Low byte
                };
                memcpy(BAPAsicFreqBuffer, freqBytes, 2);

    #endif
}

