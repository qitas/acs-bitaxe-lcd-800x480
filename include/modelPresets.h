#pragma once
#include "Arduino.h"
#include "BAP.h"
#include "I2CData.h"
#include "modelConfig.h"


extern uint16_t currentPresetFrequency;
extern uint16_t currentPresetVoltage;
extern uint8_t currentPresetFanSpeed;
extern bool currentPresetAutoFanMode;

extern const uint16_t voltageLowPower;
extern const uint16_t voltageNormalPower;
extern const uint16_t voltageHighPower;

extern const uint16_t freqLowPower;
extern const uint16_t freqNormalPower;
extern const uint16_t freqHighPower;

extern uint16_t frequencyOffset;
extern uint16_t voltageOffset;
extern uint16_t fanSpeedOffset; 


extern void setLowPowerPreset();
extern void setNormalPowerPreset();
extern void setHighPowerPreset();

extern void readCurrentPresetSettingsFromNVS();
extern void presetAutoTune();
extern bool isValidPresetPair(uint16_t freq, uint16_t voltage);
