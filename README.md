# 800x480 Screen Add-On for ESP-Miner for Bitaxe Devices

## Based On 
![image](https://github.com/user-attachments/assets/b7c3f45b-9023-491d-a716-e8a729b3063f)
[Wavshare ESP32S3 4.3" LCD](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3)
## Hardware Changes neccessary 
In Progress

## Data Interface
In Progress

## Requirements

### VSCode
### [PlatformIO](https://platformio.org/)
### [LVGLv8.3](https://lvgl.io/)
### [ESP32_Display_Panel](https://github.com/espressif/esp-display-panel)
### [ESP32_IO_Expander](https://github.com/espressif/esp-io-expander)
### [ESP32 Arduino](https://github.com/espressif/arduino-esp32)
### [TimeLib](https://github.com/PaulStoffregen/Time)

## PlatformIO Configuration
### See platformio.ini for more information

## Library Configuration

PlatformIO will automatically install the required libraries when the project is opened. However, ESP32_Display_Panel needs to be configured. 

In /backuplvgldrivers `ESP_Panel_conf.h` and `ESP_Panel_conf.c` need to replace the files in `/pio/libdeps/ESP-LCD/ESP_Display_Panel/src/`


# Related Projects:
### [ACS Bitaxe Hardware](https://github.com/Advanced-Crypto-Services/acs-bitaxe)
### [800x480 LCD Add-On](https://github.com/Advanced-Crypto-Services/acs-bitaxe-lcd-800x480)
