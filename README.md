# 800x480 Screen Add-On for ESP-Miner for Bitaxe Devices

# As of 12-26-26, PlatformIO is not used on the main branch
We have switch over to using ESP-IDF Directly, with Arduino Components on top. This allows much better compiler options with a proper SDK Config. This proved useful because Arduino Pre-compiled libraries were casuing many issues with memory initialization as the project grew bigger. 

## Based On 
![image](https://github.com/user-attachments/assets/b7c3f45b-9023-491d-a716-e8a729b3063f)

[Wavshare ESP32S3 4.3" LCD](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3)
## Hardware Changes neccessary 
![Screenshot 2024-11-29 125242](https://github.com/user-attachments/assets/73940b4c-8c93-4794-bab6-f0de2b29ead9)
![Screenshot 2024-11-29 125143](https://github.com/user-attachments/assets/140bacf1-e01c-4938-97af-2d2e1734db14)
![Screenshot 2024-11-29 125108](https://github.com/user-attachments/assets/31cb1a4c-5ea1-484c-ad55-9e5e0d2f036f)
![pxl_20241129_190656428_720](https://github.com/user-attachments/assets/bb1fdd5a-bf9c-4be9-9df6-5ddf2caee506)


### It is nececssary to Isolate the I2C Connector from the I2C bus on the LCD PCB. This I2C is used for the touch screen interface, and cannot be properly set into slave mode. This is now also compatible with Serial Therough the BAP port using the same connection

We Create a second I2C Bus dedicated for the communication between the Bitaxe and the LCD Driver. 
Currently, We utilize Pin GPIO15 and 16 for this new Bus, however this requires cutting the current RS-485 traces present. It will be more simplified to use the unused GPIO 35 and 36 for future builds.

## Data Interface

Currently Data is one way from the Bitaxe to the Display. Communication is done through i2C and uses a custom Register based scheme for keeping track of what data is being brodcasted.
BAP Port uses the UART Protocol for 2 way Async Communication

I2C Slave address is set too 0x50

I2C implemntation in in I2CData.cpp

The current layout looks like this:
- Byte 1: Register number
- Byte 2: Data length
- Byte 3+: Data

## Register Numbers
### Network Registers
- SSID        0x21
- IP Address      0x22
- Wifi Status    0x23
- Pool URL       0x24
- Pool 2 URL   0x25
- Pool Port Numbers (1 and 2)     0x26

### Mining Data Registers
- Current Hashrate       0x30
- Historical Hashrate   0x31
- Current Efficiency      0x32
- Best Difficulty All Time       0x33
- Best Difficulty Session    0x34
- Accepted Shares, Rejected Shares          0x35

### Monitoring Data Regesters
- Asic Temp          0x40
- Asic Frequnecy      0x41
- Fan Percentage and Speed            0x42
- VIN, CurrentIN, Wattage Used, Domain Voltage     0x43
- Asic Ingo     0x44
- Uptime         0x45 

### Device Status Registers
- Flags (Block Found, Startup Done, Overheat Mode)          0x50
- Device Info   0x52
- Board Info    0x53
- Clock Ntime Sync    0x54

### Mempool API Registers
- BTC Price   0x60
- Network Hashrate 0x61
- Network Difficulty 0x62
- Current Block Height 0x63
- Percentage Through Current Difficuty Adjustment Period  0x64
- Estimate Difficukty Change Percentage 0x65
- Remaing Block in Current Difficulty Period 0x66
- Remaining Time in Current Difficulty Period 0x67


## Requirements

### VSCode
### ~~[PlatformIO](https://platformio.org/)~~
ESP IDF V5.1.4
### [LVGLv8.3](https://lvgl.io/)
### [ESP32_Display_Panel](https://github.com/espressif/esp-display-panel)
### [ESP32_IO_Expander](https://github.com/espressif/esp-io-expander)
### [ESP32 Arduino](https://github.com/espressif/arduino-esp32)
### [TimeLib](https://github.com/PaulStoffregen/Time)

## SDK Config
The SDK included in the repo should be setup correctly, however here are the guidlines for manual setup:
### SPI Ram
```
CONFIG_FREERTOS_HZ=1000
CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240=y
CONFIG_ESPTOOLPY_FLASHMODE_QIO=y
CONFIG_ESPTOOLPY_FLASHFREQ_120M=y [should align with PSRAM]
CONFIG_SPIRAM_MODE_OCT=y
CONFIG_IDF_EXPERIMENTAL_FEATURES=y and CONFIG_SPIRAM_SPEED_120M=y [should align with PSRAM]
CONFIG_SPIRAM_FETCH_INSTRUCTIONS=y
CONFIG_SPIRAM_RODATA=y
CONFIG_ESP32S3_DATA_CACHE_LINE_64B=y
CONFIG_COMPILER_OPTIMIZATION_PERF=y
```
### LVGL
Copy the settings found in lv_conf.h into the sdk editor
as well as 
```
#define LV_MEM_CUSTOM 1 or CONFIG_LV_MEM_CUSTOM=y
#define LV_MEMCPY_MEMSET_STD 1 or CONFIG_LV_MEMCPY_MEMSET_STD=y
#define LV_ATTRIBUTE_FAST_MEM IRAM_ATTR or CONFIG_LV_ATTRIBUTE_FAST_MEM=y
```

### ESP Display Panel Configurations
Copy the settings found in ESP_Panel_Board_Custom.h


# Related Projects:
### [ACS Bitaxe Hardware](https://github.com/Advanced-Crypto-Services/acs-bitaxe)
### [800x480 LCD Add-On](https://github.com/Advanced-Crypto-Services/acs-bitaxe-lcd-800x480)
