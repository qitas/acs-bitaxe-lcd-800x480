# 800x480 Screen Add-On for ESP-Miner for Bitaxe Devices

## Based On 
![image](https://github.com/user-attachments/assets/b7c3f45b-9023-491d-a716-e8a729b3063f)
[Wavshare ESP32S3 4.3" LCD](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3)
## Hardware Changes neccessary 
![Screenshot 2024-11-29 125242](https://github.com/user-attachments/assets/73940b4c-8c93-4794-bab6-f0de2b29ead9)
![Screenshot 2024-11-29 125143](https://github.com/user-attachments/assets/140bacf1-e01c-4938-97af-2d2e1734db14)
![Screenshot 2024-11-29 125108](https://github.com/user-attachments/assets/31cb1a4c-5ea1-484c-ad55-9e5e0d2f036f)
![pxl_20241129_190656428_720](https://github.com/user-attachments/assets/bb1fdd5a-bf9c-4be9-9df6-5ddf2caee506)


### It is nececssary to Isolate the I2C Connector from the I2C bus on the LCD PCB. This I2C is used for the touch screen interface, and cannot be properly set into slave mode. 

We Create a second I2C Bus dedicated for the communication between the Bitaxe and the LCD Driver. 
Currently, We utilize Pin GPIO15 and 16 for this new Bus, however this requires cutting the current RS-485 traces present. It will be more simplified to use the unused GPIO 35 and 36 for future builds.

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
