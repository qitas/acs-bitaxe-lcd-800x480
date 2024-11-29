#include <Arduino.h>
#include <SPIFFS.h>
#include <esp_heap_caps.h>

#include "spiffsDriver.h"

#include <lv_conf.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>
#include "I2CData.h"
#include <TimeLib.h>



#include <lvgl.h>
#include "lvgl_port_v8.h"
#include <demos/lv_demos.h>
#include "UIScreens.h"
#include "UIHandler.h"
// Extend IO Pin define
#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5


// TODO Figure out why this will crash on bootup when deleted. Most likely something to do with memory initialization.
// DO NOT DELETE!!! 
struct HeapStats {
    uint32_t totalHeap;
    uint32_t freeHeap;
    uint32_t largestBlock;
    uint32_t minFreeHeap;
};

HeapStats lastHeapStats = {0, 0, 0, 0};

void monitorHeapAllocation() {
    HeapStats currentStats;
    
    // Get current heap statistics
    currentStats.totalHeap = ESP.getHeapSize();
    currentStats.freeHeap = ESP.getFreeHeap();
    currentStats.largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    currentStats.minFreeHeap = ESP.getMinFreeHeap();

    // Check if values have changed significantly (more than 1KB)
    const uint32_t threshold = 1024;
    bool hasChanged = false;

    if (abs(static_cast<int32_t>(currentStats.freeHeap - lastHeapStats.freeHeap)) > threshold ||
        abs(static_cast<int32_t>(currentStats.largestBlock - lastHeapStats.largestBlock)) > threshold) {
        hasChanged = true;
    }

    // Print only if values have changed significantly
    if (hasChanged) {
        Serial.println("=== Heap Statistics ===");
        Serial.printf("Total heap: %u bytes\n", currentStats.totalHeap);
        Serial.printf("Free heap: %u bytes\n", currentStats.freeHeap);
        Serial.printf("Largest free block: %u bytes\n", currentStats.largestBlock);
        Serial.printf("Minimum free heap: %u bytes\n", currentStats.minFreeHeap);
        Serial.printf("Used heap: %u bytes\n", currentStats.totalHeap - currentStats.freeHeap);
        Serial.println("====================");
        
        // Update last stats
        lastHeapStats = currentStats;
    }
}

void printMemoryInfo() {
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Total heap: %d bytes\n", ESP.getHeapSize());
}



void setup()
{
    initI2CBuffer();
    if(!psramFound()) {
        Serial.println("PSRAM not found, halting!");
        while(1) {
            delay(1000);
        }
    }
    Serial.printf("PSRAM size: %d bytes\n", ESP.getPsramSize());
    
    // Initialize SPIFFS

    //Initialize Serial
    Serial.begin(115200);
    while(!Serial);
    {
        Serial.println("Serial initialized");
    }
    // Initialize I2C Slav
    
   

    //Initialize IO Expander
    pinMode(GPIO_INPUT_IO_4, OUTPUT);
    /**
     * These development boards require the use of an IO expander to configure the screen,
     * so it needs to be initialized in advance and registered with the panel for use.
     *
     */
    Serial.println("Initialize IO expander");
    /* Initialize IO expander */
    ESP_IOExpander *expander = new ESP_IOExpander_CH422G((i2c_port_t)I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);
    // ESP_IOExpander *expander = new ESP_IOExpander_CH422G(I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000);
    expander->init();
    expander->begin();
    expander->multiPinMode(TP_RST | LCD_BL | LCD_RST | SD_CS | USB_SEL, OUTPUT);
    expander->multiDigitalWrite(TP_RST | LCD_BL | LCD_RST, HIGH);
    
    
    
    //gt911 initialization, must be added, otherwise the touch screen will not be recognized  
    //initialization begin
    expander->multiDigitalWrite(TP_RST | LCD_RST, LOW);
    
    digitalWrite(GPIO_INPUT_IO_4, LOW);
    
    expander->multiDigitalWrite(TP_RST | LCD_RST, HIGH);
    
    //initialization end
    
    //Initialize Panel
    Serial.println("Initialize panel device");
    ESP_Panel *panel = new ESP_Panel();
    panel->init();
#if LVGL_PORT_AVOID_TEAR
    // When avoid tearing function is enabled, configure the RGB bus according to the LVGL configuration
    ESP_PanelBus_RGB *rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
    rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
    rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
#endif
    panel->begin();
    

    //Initialize LVGL
    Serial.println("Initialize LVGL");
    lvgl_port_init(panel->getLcd(), panel->getTouch());
    
    lv_img_cache_set_size(LV_IMG_CACHE_DEF_SIZE);
    
    // Initialize PNG decoder
    lv_png_init();
    Serial.println("PNG decoder initialized");
    initSpiffsDriver(); 
    // Initialize UI with backlight off
    
    splashScreen();
    delay(1000);
    initalizeOneScreen();
    switchToScreen(activeScreenHome);
    initI2CSlave();
    
    Serial.println("LVGL porting example end");

    // After LVGL and PNG decoder init
    Serial.printf("Image cache size: %d KB\n", LV_IMG_CACHE_DEF_SIZE / 1024);

    // After LVGL initialization
    Serial.printf("Image cache configured for %.2f KB\n", 
        (float)LV_IMG_CACHE_DEF_SIZE / 1024.0);
}

void loop() {
 
    checkI2CHealth();

    static uint32_t lastHeapCheck = 0;
    static uint32_t lastClockSync = 0;
    // Monitor heap every second
    if (millis() - lastHeapCheck > 1000) {
        //monitorHeapAllocation();
        printMemoryInfo();
       // lastHeapCheck = millis();
       lastHeapCheck = millis();
    }

    if (millis() - lastClockSync > 1000) {
        keepClockTime();
        lastClockSync = millis();
        Serial.printf("Clock synced to: %02d/%02d/%04d %02d:%02d:%02d\n", 
            month(), day(), year(),
            hour(), minute(), second());
    }

    // Add buffer check to health check
    if (i2cBuffer == nullptr) {
        initI2CBuffer();
    }
}
