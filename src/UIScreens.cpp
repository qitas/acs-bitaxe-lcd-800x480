#include "UIScreens.h"
#include "lvgl_port_v8.h"
#include <lvgl.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include "UIHandler.h"
#include "UISharedAssets.h"
#include "I2CData.h"
#include "fonts.h"
#include <TimeLib.h>

#define MASK_WIDTH 300
#define MASK_HEIGHT 64

// Tab objects
lv_obj_t* tabHome = NULL;
lv_obj_t* tabMining = NULL;
lv_obj_t* tabActivity = NULL;
lv_obj_t* tabBitcoinNews = NULL;
lv_obj_t* tabSettings = NULL;

// Screen objects
ScreenType activeScreen = activeScreenSplash; 

// Timers
lv_timer_t* chartUpdateTimer = NULL;
lv_timer_t* networkUpdateTimer = NULL;
lv_timer_t* clockUpdateTimer = NULL;

// Labels
lv_obj_t* miningGraphFooterPoolLabel;
lv_obj_t* miningGraphFooterDiffLabel;
lv_obj_t* miningGraphHashrateLabel;
lv_obj_t* miningGraphFooterRejectRatePercentLabel;
lv_obj_t* clockLabel;
lv_obj_t* dateLabel;

// Hashrate variables
float minHashrateSeen = 250;  // Initialize to maximum possible float
float maxHashrateSeen = 400;        // Initialize to minimum for positive values
float hashrateBuffer[SMOOTHING_WINDOW_SIZE] = {0};
int bufferIndex = 0;


// This is used because Timelib day function is having issues
const char* customDayStr(uint8_t day)
{ 
    static const char* const days[] = {"", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    if (day > 7) return "";
    return days[day];
}
 
// It is used to create a gradient effect on the main container
// Not used at the moment due to performance hits
static void addMaskEventCallback(lv_event_t * e) 
{
    static lv_draw_mask_map_param_t m;
    static int16_t maskId;
    
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_opa_t * maskMap = (lv_opa_t *)lv_event_get_user_data(e);
    
    if(code == LV_EVENT_COVER_CHECK) 
    {
        lv_event_set_cover_res(e, LV_COVER_RES_MASKED);
    }
    else if(code == LV_EVENT_DRAW_MAIN_BEGIN) 
    {
        lv_draw_mask_map_init(&m, &obj->coords, maskMap);
        maskId = lv_draw_mask_add(&m, NULL);
    }
    else if(code == LV_EVENT_DRAW_MAIN_END) 
    {
        lv_draw_mask_free_param(&m);
        lv_draw_mask_remove_id(maskId);
    }
}

static void updateLabels(lv_timer_t* timer) 
{

    
    // Get values outside the lock
    char ssid[32], ip[32], status[32], pool[64];
    float hashrate = i2cData.mining.hashrate;
    float efficiency = i2cData.mining.efficiency;
    uint32_t shares = i2cData.mining.shares;
    float temp = i2cData.monitoring.temperatures[0];
    uint32_t acceptedShares = i2cData.mining.acceptedShares;
    uint32_t rejectedShares = i2cData.mining.rejectedShares;
    float rejectRatePercent = i2cData.mining.rejectRatePercent;
    uint32_t fanSpeed = i2cData.monitoring.fanSpeed;
    uint32_t asicFreq = i2cData.monitoring.asicFrequency;
    uint32_t uptime = i2cData.monitoring.uptime;
    float voltage = i2cData.monitoring.powerStats.voltage;
    float current = i2cData.monitoring.powerStats.current;
    float power = i2cData.monitoring.powerStats.power;
    float domainVoltage = i2cData.monitoring.powerStats.domainVoltage;
    
    // Lock for LVGL operations
    if (lvgl_port_lock(10)) 
    {  // 10ms timeout
        lv_obj_t** labels = (lv_obj_t**)timer->user_data;
        
        // Batch all LVGL operations together
        lv_label_set_text_fmt(labels[0], "SSID: %s", i2cData.network.ssid);
        lv_label_set_text_fmt(labels[1], "IP: %s", i2cData.network.ipAddress);
        lv_label_set_text_fmt(labels[2], "Status: %s", i2cData.network.wifiStatus);
        lv_label_set_text_fmt(labels[3], "Pool 1: %s:%d\nPool 2: %s:%d", i2cData.network.poolUrl, i2cData.network.poolPort, i2cData.network.fallbackUrl, i2cData.network.fallbackPort);
        lv_label_set_text_fmt(labels[4], "Hashrate: %d.%02d GH/s", (int)hashrate, (int)(hashrate * 100) % 100);
        lv_label_set_text_fmt(labels[5], "Efficiency: %d.%02d W/TH", (int)efficiency, (int)(efficiency * 100) % 100);
        lv_label_set_text_fmt(labels[6], "Best Diff: %s", i2cData.mining.bestDiff);
        lv_label_set_text_fmt(labels[7], "Session Diff: %s", i2cData.mining.sessionDiff);
        lv_label_set_text_fmt(labels[8], "Accepted: %u", acceptedShares);
        lv_label_set_text_fmt(labels[9], "Rejected: %u", rejectedShares);
        lv_label_set_text_fmt(labels[10], "Temp: %d°C", (int)temp);
        lv_label_set_text_fmt(labels[11], "Fan: %d RPM", fanSpeed);
        lv_label_set_text_fmt(labels[12], "ASIC: %d MHz", asicFreq);
        lv_label_set_text_fmt(labels[13], "Uptime: %d:%02d:%02d", (uptime / 3600), (uptime % 3600) / 60, uptime % 60);
        lv_label_set_text_fmt(labels[14], "Voltage: %d.%02d V", (int)(voltage / 1000), (int)(voltage / 10) % 100);
        lv_label_set_text_fmt(labels[15], "Current: %d.%02d A", (int)(current / 1000), (int)(current / 10) % 100);
        lv_label_set_text_fmt(labels[16], "Power: %d.%02d W", (int)power, (int)(power * 100) % 100);
        lv_label_set_text_fmt(labels[17], "Domain: %d mV", (int)domainVoltage);
        
        lvgl_port_unlock();
    }
}



void splashScreen()
{
    activeScreen = activeScreenSplash;
    Serial.println("Create UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    // Create a screen object
    lv_obj_t* splashScreen = lv_obj_create(NULL);
    lv_scr_load(splashScreen);

    // Create the Background image
    backgroundImage(splashScreen);

    // Create the OSBM logo image
    lv_obj_t* osbmLogo = lv_img_create(splashScreen);
    lv_img_set_src(osbmLogo, "S:/openSourceBitcoinMining.png");
    lv_obj_center(osbmLogo);

    // Create the ACS, Bitaxe, and OSMU image
    lv_obj_t* logos = lv_img_create(splashScreen);
    lv_img_set_src(logos, "S:/Logos.png");

    lv_obj_align(logos, LV_ALIGN_BOTTOM_MID, 0, -100);
    lvgl_port_unlock();
}

void homeScreen() 
{
    activeScreen = activeScreenHome;
    /*
    // Not using Logos on the home screen at the moment.
    lv_obj_t* logos = lv_img_create(screenObjs.homeMainContainer);
    lv_img_set_src(logos, "S:/Logos.png");
    lv_obj_align(logos, LV_ALIGN_TOP_MID, 0, 0);
    lv_img_set_zoom(logos, 384);
    //lv_obj_align(mainContainerLabel, LV_ALIGN_CENTER, 0, 0);  // Initial alignment 
    //lv_obj_add_style(mainContainerLabel, &mainContainerBorder, 0);
   // lvgl_port_unlock();
    */

    // Create the clock container
    lv_obj_t* clockContainer = lv_obj_create(screenObjs.homeMainContainer);
    lv_obj_set_size(clockContainer, 520, 304);
    lv_obj_align(clockContainer, LV_ALIGN_CENTER, 0, -48);
    lv_obj_set_style_bg_opa(clockContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(clockContainer, 0, LV_PART_MAIN);
    lv_obj_clear_flag(clockContainer, LV_OBJ_FLAG_SCROLLABLE);
    //lv_obj_set_style_border_width(clockContainer, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(clockContainer, lv_color_hex(0x00FF00), LV_PART_MAIN);

    // Create the clock label
    clockLabel = lv_label_create(clockContainer);
    lv_label_set_text(clockLabel, "-- : --");
    lv_obj_set_style_text_font(clockLabel, &interExtraBold56, LV_PART_MAIN);
    lv_obj_set_style_text_color(clockLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(clockLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(clockLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    //lv_obj_set_style_border_width(clockLabel, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(clockLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);

    // Create the date label
    dateLabel = lv_label_create(clockContainer);
    lv_label_set_text(dateLabel, "Syncing . . .");
    lv_obj_set_style_text_font(dateLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(dateLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_set_style_text_align(dateLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_opa(dateLabel, LV_OPA_80, LV_PART_MAIN);
    lv_obj_align(dateLabel, LV_ALIGN_BOTTOM_MID, 0, 0);
    //lv_obj_set_style_border_width(dateLabel, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(dateLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);

    // Create the clock timer
    screenObjs.clockTimer = lv_timer_create([](lv_timer_t* timer) 
    {
       // Get values outside the lock
        uint8_t h = hourFormat12();
        uint8_t m = minute();
        uint8_t s = second();
        uint8_t mo = month();
        uint8_t d = day();
        uint8_t w = weekday();
        uint16_t y = year();    
        bool isAm = isAM();

        // Lock for LVGL operations
        if (lvgl_port_lock(10)) 
        {  // 10ms timeout
            
            // Check if the time is before 2000, This is used to check if the time has been set
            if (now() < 946684800) 
            {
                Serial.println("Time is before 2000");
                lv_label_set_text(dateLabel, "Syncing . . .");
            }
            else 
            {
                // Batch all LVGL operations together
                lv_label_set_text_fmt(clockLabel, "%02d:%02d%s", h, m, isAm ? "AM" : "PM");
                lv_label_set_text_fmt(dateLabel, "%s\n%s %d %d", customDayStr(w), monthStr(mo), d, y);
            }
            
            lvgl_port_unlock();
        }
    }, 1000, &clockLabel);
}

void activityScreen() 
{
    activeScreen = activeScreenActivity;
    //lvgl_port_lock(-1);

    //This is used for Gradient 
    /*static lv_opa_t maskMap[MASK_WIDTH * MASK_HEIGHT];
    // Create canvas for mask
    lv_obj_t * canvas = lv_canvas_create(screenObjs.activityMainContainer);
    lv_canvas_set_buffer(canvas, maskMap, MASK_WIDTH, MASK_HEIGHT, LV_IMG_CF_ALPHA_8BIT);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_TRANSP);

    // Draw text to canvas
    lv_draw_label_dsc_t labelDsc;
    lv_draw_label_dsc_init(&labelDsc);
    labelDsc.color = lv_color_white();
    labelDsc.font = &interExtraBold56;
    labelDsc.align = LV_TEXT_ALIGN_CENTER;
    //lv_canvas_draw_text(canvas, 5, 5, MASK_WIDTH, &labelDsc, "10 : 00AM");

    // Delete canvas after creating mask
    lv_obj_del(canvas);
    */ 

    // Not using Logos on the activity screen at the moment.
    /*
    lv_obj_t* logos = lv_img_create(screenObjs.activityMainContainer);
    lv_img_set_src(logos, "S:/Logos.png");
    lv_obj_align(logos, LV_ALIGN_TOP_MID, 0, 0);
    lv_img_set_zoom(logos, 384);
    */

    /*
    // Create gradient object
    lv_obj_t * gradObj = lv_obj_create(screenObjs.activityMainContainer);
    lv_obj_set_size(gradObj, MASK_WIDTH, MASK_HEIGHT);
    lv_obj_center(gradObj);
    lv_obj_set_style_bg_color(gradObj, lv_color_hex(0x34D399), 0);
    lv_obj_set_style_bg_grad_color(gradObj, lv_color_hex(0xA7F3D0), 0);
    lv_obj_set_style_border_width(gradObj, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(gradObj, LV_OPA_0, LV_PART_MAIN);
    //lv_obj_set_style_bg_grad_dir(gradObj, LV_GRAD_DIR_VER, 0);
    //lv_obj_add_event_cb(gradObj, addMaskEventCallback, LV_EVENT_ALL, maskMap);
    */

    // Create container for network info
    lv_obj_t* networkContainer = lv_obj_create(screenObjs.activityMainContainer);
    lv_obj_set_size(networkContainer, 360, 184);
    lv_obj_align(networkContainer, LV_ALIGN_TOP_LEFT, -24, 0);
    lv_obj_set_style_bg_opa(networkContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(networkContainer, 0, LV_PART_MAIN);
    lv_obj_clear_flag(networkContainer, LV_OBJ_FLAG_SCROLLABLE);
    //lv_obj_set_style_border_width(networkContainer, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(networkContainer, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    Serial.println("Network Container Created");

    //Network Container Label
    lv_obj_t* networkContainerLabel = lv_label_create(networkContainer);
    lv_label_set_text(networkContainerLabel, "Network");
    lv_obj_set_style_text_font(networkContainerLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(networkContainerLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(networkContainerLabel, LV_ALIGN_TOP_LEFT, 0, -24);
    Serial.println("Network Container Label Created");
    // SSID Label
    lv_obj_t* ssidLabel = lv_label_create(networkContainer);
    lv_obj_set_style_text_font(ssidLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(ssidLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(ssidLabel, "SSID: %s", i2cData.network.ssid);
    lv_obj_align(ssidLabel, LV_ALIGN_TOP_LEFT, 16, 8);
    Serial.println("SSID Label Created");
    // IP Address Label
    lv_obj_t* ipLabel = lv_label_create(networkContainer);
    lv_obj_set_style_text_font(ipLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(ipLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(ipLabel, "IP: %s", i2cData.network.ipAddress);
    lv_obj_align(ipLabel, LV_ALIGN_TOP_LEFT, 16, 32);
    Serial.println("IP Address Label Created");
    // WiFi Status Label
    lv_obj_t* wifiStatusLabel = lv_label_create(networkContainer);
    lv_obj_set_style_text_font(wifiStatusLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(wifiStatusLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(wifiStatusLabel, "Status: %s", i2cData.network.wifiStatus);
    lv_obj_align(wifiStatusLabel, LV_ALIGN_TOP_LEFT, 16, 56);
    Serial.println("WiFi Status Label Created");
    // Pool Info Label
    lv_obj_t* poolUrlLabel = lv_label_create(networkContainer);
    lv_obj_set_style_text_font(poolUrlLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(poolUrlLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(poolUrlLabel, "Pool 1: %s:%d\nPool 2: %s:%d", i2cData.network.poolUrl, i2cData.network.poolPort, i2cData.network.fallbackUrl, i2cData.network.fallbackPort);
    lv_obj_align(poolUrlLabel, LV_ALIGN_TOP_LEFT, 16, 80);
    Serial.println("Pool Info Label Created");

   

    // Create container for mining info
    lv_obj_t* miningContainer = lv_obj_create(screenObjs.activityMainContainer);
    lv_obj_set_size(miningContainer, 304, 184);
    lv_obj_align(miningContainer, LV_ALIGN_TOP_RIGHT, 16, 0);
    lv_obj_set_style_bg_opa(miningContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(miningContainer, 0, LV_PART_MAIN);
    //lv_obj_set_style_border_width(miningContainer, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(miningContainer, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_clear_flag(miningContainer, LV_OBJ_FLAG_SCROLLABLE);
    Serial.println("Mining Container Created");

    // Mining Container Label
    lv_obj_t* miningContainerLabel = lv_label_create(miningContainer);
    lv_label_set_text(miningContainerLabel, "Mining");
    lv_obj_set_style_text_font(miningContainerLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(miningContainerLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(miningContainerLabel, LV_ALIGN_TOP_LEFT, 0, -24);
    Serial.println("Mining Container Label Created");
    // Hashrate Label
    lv_obj_t* hashrateLabel = lv_label_create(miningContainer);
    lv_obj_set_style_text_font(hashrateLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(hashrateLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(hashrateLabel, "Hashrate: %d.%02d GH/s", (int)i2cData.mining.hashrate, (int)(i2cData.mining.hashrate * 100) % 100);
    lv_obj_align(hashrateLabel, LV_ALIGN_TOP_LEFT, 16, 8);
    Serial.println("Hashrate Label Created");
    // Efficiency Label
    lv_obj_t* efficiencyLabel = lv_label_create(miningContainer);
    lv_obj_set_style_text_font(efficiencyLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(efficiencyLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(efficiencyLabel, "Efficiency: %d.%02d W/TH", (int)i2cData.mining.efficiency, (int)(i2cData.mining.efficiency * 100) % 100);
    lv_obj_align(efficiencyLabel, LV_ALIGN_TOP_LEFT, 16, 32);
    Serial.println("Efficiency Label Created");
    // Best Difficulty Label
    lv_obj_t* bestDiffLabel = lv_label_create(miningContainer);
    lv_obj_set_style_text_font(bestDiffLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(bestDiffLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(bestDiffLabel, "Best Diff: %s", i2cData.mining.bestDiff);
    lv_obj_align(bestDiffLabel, LV_ALIGN_TOP_LEFT, 16, 56);
    Serial.println("Best Difficulty Label Created");
    // Session Difficulty Label
    lv_obj_t* sessionDiffLabel = lv_label_create(miningContainer);
    lv_obj_set_style_text_font(sessionDiffLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(sessionDiffLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(sessionDiffLabel, "Session Diff: %s", i2cData.mining.sessionDiff);
    lv_obj_align(sessionDiffLabel, LV_ALIGN_TOP_LEFT, 16, 80);
    Serial.println("Session Difficulty Label Created");

    /*// Shares Label
    lv_obj_t* sharesLabel = lv_label_create(miningContainer);
    lv_obj_set_style_text_font(sharesLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(sharesLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(sharesLabel, "Shares: %u", i2cData.mining.shares);
    lv_obj_align(sharesLabel, LV_ALIGN_TOP_LEFT, 16, 104);
    Serial.println("Shares Label Created");*/
   
    //accepted shares label
    lv_obj_t* acceptedSharesLabel = lv_label_create(miningContainer);
    lv_obj_set_style_text_font(acceptedSharesLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(acceptedSharesLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(acceptedSharesLabel, "Accepted Shares: %u", i2cData.mining.acceptedShares);
    lv_obj_align(acceptedSharesLabel, LV_ALIGN_TOP_LEFT, 16, 104);
    Serial.println("Accepted Shares Label Created");
    //rejected shares label
    lv_obj_t* rejectedSharesLabel = lv_label_create(miningContainer);
    lv_obj_set_style_text_font(rejectedSharesLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(rejectedSharesLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(rejectedSharesLabel, "Rejected Shares: %u", i2cData.mining.rejectedShares);
    lv_obj_align(rejectedSharesLabel, LV_ALIGN_TOP_LEFT, 16, 128);
    Serial.println("Rejected Shares Label Created");

    /*// rejected shares percent label
    lv_obj_t* rejectedSharesPercentLabel = lv_label_create(miningContainer);
    lv_obj_set_style_text_font(rejectedSharesPercentLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(rejectedSharesPercentLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(rejectedSharesPercentLabel, "Reject Rate: %d.%02d%%", (int)i2cData.mining.rejectRatePercent, (int)(i2cData.mining.rejectRatePercent * 100) % 100);
    lv_obj_align(rejectedSharesPercentLabel, LV_ALIGN_TOP_LEFT, 16, 176);
    Serial.println("Rejected Shares Percent Label Created");*/


    // Monitoring Container 
    lv_obj_t* monitoringContainer = lv_obj_create(screenObjs.activityMainContainer);
    lv_obj_set_size(monitoringContainer, 304, 152);
    lv_obj_align(monitoringContainer, LV_ALIGN_BOTTOM_RIGHT, 16, 0);
    Serial.println("Monitoring Container Created");
    lv_obj_set_style_bg_opa(monitoringContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(monitoringContainer, 0, LV_PART_MAIN);
    //lv_obj_set_style_border_color(monitoringContainer, lv_color_hex(0xA7F3D0), LV_PART_MAIN);

    // Monitoring Container Label
    lv_obj_t* monitoringContainerLabel = lv_label_create(monitoringContainer);
    lv_label_set_text(monitoringContainerLabel, "Monitoring");
    lv_obj_set_style_text_font(monitoringContainerLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(monitoringContainerLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(monitoringContainerLabel, LV_ALIGN_TOP_LEFT, 0, -24);
    Serial.println("Monitoring Container Label Created");
    // Temperature Label
    lv_obj_t* tempLabel = lv_label_create(monitoringContainer);
    lv_obj_set_style_text_font(tempLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(tempLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(tempLabel, "Temp: %d°C", (int)i2cData.monitoring.temperatures[0]);
    lv_obj_align(tempLabel, LV_ALIGN_TOP_LEFT, 16, 8);
    Serial.println("Temperature Label Created");
    // Fan Speed Label
    lv_obj_t* fanSpeedLabel = lv_label_create(monitoringContainer);
    lv_obj_set_style_text_font(fanSpeedLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(fanSpeedLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(fanSpeedLabel, "Fan: %d RPM", i2cData.monitoring.fanSpeed);
    lv_obj_align(fanSpeedLabel, LV_ALIGN_TOP_LEFT, 16, 32);
    Serial.println("Fan Speed Label Created");
    // asic Frequency Label
    lv_obj_t* asicFreqLabel = lv_label_create(monitoringContainer);
    lv_obj_set_style_text_font(asicFreqLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(asicFreqLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(asicFreqLabel, "ASIC: %d MHz", i2cData.monitoring.asicFrequency);
    lv_obj_align(asicFreqLabel, LV_ALIGN_TOP_LEFT, 16, 56);
    Serial.println("ASIC Frequency Label Created");
    // uptime label
    lv_obj_t* uptimeLabel = lv_label_create(monitoringContainer);
    lv_obj_set_style_text_font(uptimeLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(uptimeLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(uptimeLabel, "Uptime: %d:%02d:%02d", (i2cData.monitoring.uptime / 3600), (i2cData.monitoring.uptime % 3600) / 60, i2cData.monitoring.uptime % 60);
    lv_obj_align(uptimeLabel, LV_ALIGN_TOP_LEFT, 16, 80);
    Serial.println("Uptime Label Created");


    // Power Container
    lv_obj_t* powerContainer = lv_obj_create(screenObjs.activityMainContainer);
    lv_obj_set_size(powerContainer, 360, 152);
    lv_obj_align(powerContainer, LV_ALIGN_BOTTOM_LEFT, -24, 0);
    lv_obj_set_style_bg_opa(powerContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(powerContainer, 0, LV_PART_MAIN);
    //lv_obj_set_style_border_color(powerContainer, lv_color_hex(0xA7F3D0), LV_PART_MAIN);    
    Serial.println("Power Container Created");
    // Power Container Label
    lv_obj_t* powerContainerLabel = lv_label_create(powerContainer);
    lv_label_set_text(powerContainerLabel, "Power");
    lv_obj_set_style_text_font(powerContainerLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(powerContainerLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(powerContainerLabel, LV_ALIGN_TOP_LEFT, 0, -24);
    Serial.println("Power Container Label Created");
    // voltage label
    lv_obj_t* voltageLabel = lv_label_create(powerContainer);
    lv_obj_set_style_text_font(voltageLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(voltageLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(voltageLabel, "Voltage: %d.%02d V", (int)(i2cData.monitoring.powerStats.voltage / 1000), (int)(i2cData.monitoring.powerStats.voltage / 10) % 100);
    lv_obj_align(voltageLabel, LV_ALIGN_TOP_LEFT, 16, 8);
    Serial.println("Voltage Label Created");
    // current label
    lv_obj_t* currentLabel = lv_label_create(powerContainer);
    lv_obj_set_style_text_font(currentLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(currentLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(currentLabel, "Current: %d.%02d A", (int)(i2cData.monitoring.powerStats.current / 1000), (int)(i2cData.monitoring.powerStats.current / 10) % 100);
    lv_obj_align(currentLabel, LV_ALIGN_TOP_LEFT, 16, 32);
    Serial.println("Current Label Created");
    // Power Label
    lv_obj_t* powerLabel = lv_label_create(powerContainer);
    lv_obj_set_style_text_font(powerLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(powerLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(powerLabel, "Power: %d.%02d W", (int)i2cData.monitoring.powerStats.power, (int)(i2cData.monitoring.powerStats.power * 100) % 100);
    lv_obj_align(powerLabel, LV_ALIGN_TOP_LEFT, 16, 56);
    Serial.println("Power Label Created");
    // Domain Voltage Label
    lv_obj_t* domainVoltageLabel = lv_label_create(powerContainer);
    lv_obj_set_style_text_font(domainVoltageLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(domainVoltageLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_label_set_text_fmt(domainVoltageLabel, "Asic Voltage: %d mV", (int)i2cData.monitoring.powerStats.domainVoltage);
    lv_obj_align(domainVoltageLabel, LV_ALIGN_TOP_LEFT, 16, 80);
    Serial.println("Domain Voltage Label Created");
    



     static lv_obj_t* allLabels[18] = 
    {
        ssidLabel, ipLabel, wifiStatusLabel, poolUrlLabel,
        hashrateLabel, efficiencyLabel, 
        bestDiffLabel, sessionDiffLabel, acceptedSharesLabel, rejectedSharesLabel,
        tempLabel, fanSpeedLabel, asicFreqLabel, uptimeLabel, voltageLabel, currentLabel, powerLabel, domainVoltageLabel
    };
    screenObjs.labelUpdateTimer = lv_timer_create(updateLabels, 2000, allLabels);

    //lv_obj_align(mainContainerLabel, LV_ALIGN_CENTER, 0, 0);  // Initial alignment 
    //lv_obj_add_style(mainContainerLabel, &mainContainerBorder, 0);
   // lvgl_port_unlock();


}

/*void miningStatusScreen() //TODO: Remove this screen
{
    activeScreen = activeScreenMining;
    //lvgl_port_lock(-1);
    lv_obj_t* miningStatusScreen = lv_obj_create(NULL);
    lv_scr_load_anim(miningStatusScreen, LV_SCR_LOAD_ANIM_FADE_ON, 50, 0, true);

    backgroundImage(miningStatusScreen);
    lvglTabIcons(miningStatusScreen);
    statusBar(miningStatusScreen);
    mainContainer(miningStatusScreen);

  hashrateGraph(screenObjs.miningMainContainer);
  

}*/



void initalizeOneScreen() 
{
    lvgl_port_lock(-1);
    activeScreen = activeScreenHome;

    // Initialize timer pointers to NULL
    screenObjs.labelUpdateTimer = NULL;
    screenObjs.chartUpdateTimer = NULL;
    screenObjs.clockTimer = NULL;
    screenObjs.apiUpdateTimer = NULL;

    Serial.println("Timer Pointers Initialized");
    // Create and load the background screen
    screenObjs.background = lv_obj_create(NULL);
    lv_scr_load(screenObjs.background);
    Serial.println("Background Screen Created");
    // Create basic UI elements
    backgroundImage(screenObjs.background);
    Serial.println("Background Image Created");
    lvglTabIcons(screenObjs.background);
    Serial.println("Tab Icons Created");
    statusBar(screenObjs.background);
    Serial.println("Status Bar Created");
    
    // Create containers but keep them hidden initially
    mainContainer(screenObjs.background);
    screenObjs.homeMainContainer = lv_obj_get_child(screenObjs.background, -1);  // Get last created object
    lv_obj_add_flag(screenObjs.homeMainContainer, LV_OBJ_FLAG_HIDDEN);
    Serial.println("Home Container Created");
    mainContainer(screenObjs.background);
    screenObjs.miningMainContainer = lv_obj_get_child(screenObjs.background, -1);  // Get last created object
    lv_obj_add_flag(screenObjs.miningMainContainer, LV_OBJ_FLAG_HIDDEN);
    Serial.println("Mining Container Created");

    mainContainer(screenObjs.background);
    screenObjs.activityMainContainer = lv_obj_get_child(screenObjs.background, -1);  // Get last created object
    lv_obj_add_flag(screenObjs.activityMainContainer, LV_OBJ_FLAG_HIDDEN);
    Serial.println("Activity Container Created");

    mainContainer(screenObjs.background);
    screenObjs.bitcoinNewsMainContainer = lv_obj_get_child(screenObjs.background, -1);  // Get last created object
    lv_obj_add_flag(screenObjs.bitcoinNewsMainContainer, LV_OBJ_FLAG_HIDDEN);
    Serial.println("Bitcoin News Container Created");

    mainContainer(screenObjs.background);
    screenObjs.settingsMainContainer = lv_obj_get_child(screenObjs.background, -1);  // Get last created object
    lv_obj_add_flag(screenObjs.settingsMainContainer, LV_OBJ_FLAG_HIDDEN);
    Serial.println("Settings Container Created");
    
    // Initialize screens without timers
    activityScreen();
    homeScreen();
    Serial.println("Home Screen Created");
    hashrateGraph(screenObjs.miningMainContainer);
    Serial.println("Hashrate Graph Created");
    bitcoinNewsScreen();
    Serial.println("Bitcoin News Screen Created");
    settingsScreen();
    Serial.println("Settings Screen Created");
    // Now show the initial screen and start its timer
    lv_obj_clear_flag(screenObjs.homeMainContainer, LV_OBJ_FLAG_HIDDEN);
    Serial.println("Home Screen Shown");
    // Create timers only after all objects are initialized
    if (screenObjs.labelUpdateTimer) 
    {
        lv_timer_pause(screenObjs.labelUpdateTimer);
        Serial.println("Label Update Timer Paused");
    }
    if (screenObjs.chartUpdateTimer) 
    {
        lv_timer_pause(screenObjs.chartUpdateTimer);
        Serial.println("Chart Update Timer Paused");
    }
    if (screenObjs.clockTimer) 
    {
        lv_timer_pause(screenObjs.clockTimer);
        Serial.println("Clock Timer Paused");
        }
    // Start home screen timer
    lv_timer_resume(screenObjs.clockTimer);
    Serial.println("Clock Timer Resumed");
    
    lvgl_port_unlock();
    Serial.println("UI Initialized");
}

// Create a helper function to calculate moving average
float calculateMovingAverage(float newValue) 
{
    hashrateBuffer[bufferIndex] = newValue;
    bufferIndex = (bufferIndex + 1) % SMOOTHING_WINDOW_SIZE;
    
    float sum = 0;
    for (int i = 0; i < SMOOTHING_WINDOW_SIZE; i++) 
    {
        sum += hashrateBuffer[i];
    }
    return sum / SMOOTHING_WINDOW_SIZE;
}

void hashrateGraph(lv_obj_t* parent)
{
    // Pool Footer Label
    miningGraphFooterPoolLabel = lv_label_create(parent);
    lv_label_set_text_fmt(miningGraphFooterPoolLabel, "Pool: %s", i2cData.network.poolUrl);
    lv_obj_set_style_text_font(miningGraphFooterPoolLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_align(miningGraphFooterPoolLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(miningGraphFooterPoolLabel, 320);
    lv_label_set_long_mode(miningGraphFooterPoolLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(miningGraphFooterPoolLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_set_style_text_opa(miningGraphFooterPoolLabel, LV_OPA_100, LV_PART_MAIN);
    //lv_obj_set_style_border_width(miningGraphFooterPoolLabel, 1, LV_PART_MAIN);
   // lv_obj_set_style_border_color(miningGraphFooterPoolLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);
    lv_obj_align(miningGraphFooterPoolLabel, LV_ALIGN_BOTTOM_LEFT,-16, 0);

    // Diff Footer Label
    miningGraphFooterDiffLabel = lv_label_create(parent);
    lv_label_set_text_fmt(miningGraphFooterDiffLabel, "Difficulty: %s", i2cData.mining.sessionDiff);
    lv_obj_set_style_text_font(miningGraphFooterDiffLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_align(miningGraphFooterDiffLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(miningGraphFooterDiffLabel, 152);
    lv_label_set_long_mode(miningGraphFooterDiffLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(miningGraphFooterDiffLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_set_style_text_opa(miningGraphFooterDiffLabel, LV_OPA_100, LV_PART_MAIN);
    //lv_obj_set_style_border_width(miningGraphFooterDiffLabel, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(miningGraphFooterDiffLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);
    lv_obj_align(miningGraphFooterDiffLabel, LV_ALIGN_BOTTOM_MID, 72, 0);

    // Reject Rate Footer Label
    miningGraphFooterRejectRatePercentLabel = lv_label_create(parent);
    lv_label_set_text_fmt(miningGraphFooterRejectRatePercentLabel, "Reject Rate: %d.%02d%%", (int)i2cData.mining.rejectRatePercent, (int)(i2cData.mining.rejectRatePercent * 100) % 100);
    lv_obj_set_style_text_font(miningGraphFooterRejectRatePercentLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_align(miningGraphFooterRejectRatePercentLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(miningGraphFooterRejectRatePercentLabel, 280);
    lv_label_set_long_mode(miningGraphFooterRejectRatePercentLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(miningGraphFooterRejectRatePercentLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_set_style_text_opa(miningGraphFooterRejectRatePercentLabel, LV_OPA_100, LV_PART_MAIN);
   // lv_obj_set_style_border_width(miningGraphFooterRejectRatePercentLabel, 1, LV_PART_MAIN);
   // lv_obj_set_style_border_color(miningGraphFooterRejectRatePercentLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);
    lv_obj_align(miningGraphFooterRejectRatePercentLabel, LV_ALIGN_BOTTOM_MID, 296, 0);

    // Hashrate Chart Label
    lv_obj_t* hashrateChartLabel = lv_label_create(parent);
    lv_label_set_text(hashrateChartLabel, "Hashrate");
    lv_obj_set_style_text_font(hashrateChartLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(hashrateChartLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(hashrateChartLabel, LV_ALIGN_TOP_LEFT, 24, -16);

     // Hashrate Label
    miningGraphHashrateLabel = lv_label_create(parent);
    lv_label_set_text_fmt(miningGraphHashrateLabel, "%d.%02d GH/s", (int)i2cData.mining.hashrate, (int)(i2cData.mining.hashrate * 100) % 100);
    lv_obj_set_style_text_font(miningGraphHashrateLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_align(miningGraphHashrateLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(miningGraphHashrateLabel, 296);
    lv_label_set_long_mode(miningGraphHashrateLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(miningGraphHashrateLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_set_style_text_opa(miningGraphHashrateLabel, LV_OPA_100, LV_PART_MAIN);
    //lv_obj_set_style_border_width(miningGraphFooterHashrateLabel, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(miningGraphFooterHashrateLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);
    lv_obj_align(miningGraphHashrateLabel, LV_ALIGN_TOP_LEFT, 136, -16);

    lv_obj_t* miningStatusHashRateChart = lv_chart_create(parent);
    lv_obj_set_size(miningStatusHashRateChart, 648, 280);
    lv_obj_align(miningStatusHashRateChart, LV_ALIGN_CENTER, 32, -16);
    lv_obj_set_style_bg_opa(miningStatusHashRateChart, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_side(miningStatusHashRateChart, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);
    lv_obj_set_style_border_width(miningStatusHashRateChart, 4, LV_PART_MAIN);
    lv_obj_set_style_border_color(miningStatusHashRateChart, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_chart_set_type(miningStatusHashRateChart, LV_CHART_TYPE_LINE);

    // Add these style configurations
    lv_obj_set_style_line_width(miningStatusHashRateChart, 4, LV_PART_ITEMS);    // Make lines thicker
    lv_obj_set_style_size(miningStatusHashRateChart, 0, LV_PART_INDICATOR);      // Make points bigger
    //lv_obj_set_style_radius(miningStatusHashRateChart, 24, LV_PART_INDICATOR);    // Make points rounder

    // Optional: Add drop shadow to make it look even better
    //lv_obj_set_style_shadow_width(miningStatusHashRateChart, 32, LV_PART_ITEMS);
    //lv_obj_set_style_shadow_color(miningStatusHashRateChart, lv_color_hex(0xA7F3D0), LV_PART_ITEMS);
    //lv_obj_set_style_shadow_opa(miningStatusHashRateChart, LV_OPA_30, LV_PART_ITEMS);

    // Style the grid lines
    //lv_obj_set_style_line_color(miningStatusHashRateChart, lv_color_hex(0xBBBBBB), LV_PART_MAIN);  // Grid color
    //lv_obj_set_style_line_width(miningStatusHashRateChart, 4, LV_PART_MAIN);  // Grid line thickness
    lv_obj_set_style_line_opa(miningStatusHashRateChart, LV_OPA_TRANSP, LV_PART_MAIN);  // Grid line opacity
    //lv_obj_set_style_shadow_width(miningStatusHashRateChart, 12, LV_PART_MAIN);
    //lv_obj_set_style_shadow_color(miningStatusHashRateChart, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    //lv_obj_set_style_shadow_opa(miningStatusHashRateChart, LV_OPA_30, LV_PART_MAIN);

    // Set chart properties with buffer of 100 on each end
    float baseHashrate = abs(i2cData.mining.hashrate);
    //lv_chart_set_range(miningStatusHashRateChart, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);

    // Set the number of points to display
    lv_chart_set_point_count(miningStatusHashRateChart, 50);  // How many points to display

    // Add series
    lv_chart_series_t* hashRateSeries = lv_chart_add_series(miningStatusHashRateChart, lv_color_hex(0xA7F3D0), LV_CHART_AXIS_PRIMARY_Y);

    // Only add initial points if we have valid data

        for(int i = 0; i < 50; i++) 
        {
            lv_chart_set_next_value(miningStatusHashRateChart, hashRateSeries, i2cData.mining.hashrate);
        }

    // Update timer to use actual hashrate
    screenObjs.chartUpdateTimer = lv_timer_create([](lv_timer_t* timer) 
    {
        // Get values outside the lock
        float currentHashrate = i2cData.mining.hashrate;
        char poolUrl[64];
        uint16_t poolPort = i2cData.network.poolPort;
        strncpy(poolUrl, i2cData.network.poolUrl, sizeof(poolUrl) - 1);
        
        // Calculate derived values
        float smoothedHashrate = calculateMovingAverage(currentHashrate);
        float rejectRatePercent = i2cData.mining.rejectRatePercent;
        // Lock for LVGL operations
        if (lvgl_port_lock(10)) 
        {  // 10ms timeout
            lv_obj_t* chart = (lv_obj_t*)timer->user_data;
            lv_chart_series_t* series = lv_chart_get_series_next(chart, NULL);
            
            // Batch all LVGL operations together
            lv_label_set_text_fmt(miningGraphFooterPoolLabel, "Pool: %s:%d", poolUrl, poolPort);
            lv_label_set_text_fmt(miningGraphHashrateLabel, "%d.%02d GH/s", (int)currentHashrate, (int)(currentHashrate * 100) % 100);
            lv_label_set_text_fmt(miningGraphFooterRejectRatePercentLabel, "Reject Rate: %d.%02d%%", (int)rejectRatePercent, (int)(rejectRatePercent * 100) % 100);
            lv_label_set_text_fmt(miningGraphFooterDiffLabel, "Difficulty: %s", i2cData.mining.sessionDiff);
            
            if (smoothedHashrate < minHashrateSeen || smoothedHashrate > maxHashrateSeen) {
                if (smoothedHashrate < minHashrateSeen) minHashrateSeen = smoothedHashrate;
                if (smoothedHashrate > maxHashrateSeen) maxHashrateSeen = smoothedHashrate;
                
                lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 
                    (minHashrateSeen > 100) ? minHashrateSeen - 100 : 0,
                    maxHashrateSeen + 100);
            }
            
            lv_chart_set_next_value(chart, series, smoothedHashrate);
            lvgl_port_unlock();
        }
    }, 2000, miningStatusHashRateChart);

    // Optional: Add axis labels
    lv_chart_set_axis_tick(miningStatusHashRateChart, LV_CHART_AXIS_PRIMARY_Y, 5, 5, 6, 4, true, 50);
    lv_obj_clear_flag(miningStatusHashRateChart, LV_OBJ_FLAG_SCROLLABLE);
    //lvgl_port_unlock();

}

static void updateMempoolAPIInfo(lv_timer_t* timer) 
{
    auto* labels = (lv_obj_t**)timer->user_data;
    
    // Get values outside the lock
    uint32_t btcPriceUSD = i2cData.api.btcPriceUSD;
    double networkHashrate = i2cData.api.networkHashrate;
    double networkDifficulty = i2cData.api.networkDifficulty;
    uint32_t blockHeight = i2cData.api.blockHeight;
    uint32_t remainingTimeToDifficultyAdjustment = i2cData.api.remainingTimeToDifficultyAdjustment;
    uint32_t remainingBlocksToDifficultyAdjustment = i2cData.api.remainingBlocksToDifficultyAdjustment;
    double difficultyProgressPercent = i2cData.api.difficultyProgressPercent;
    double difficultyChangePercent = i2cData.api.difficultyChangePercent;
    uint32_t fastestFee = i2cData.api.fastestFee;
    uint32_t halfHourFee = i2cData.api.halfHourFee;
    uint32_t hourFee = i2cData.api.hourFee;
    uint32_t economyFee = i2cData.api.economyFee;
    uint32_t minimumFee = i2cData.api.minimumFee;

    if (lvgl_port_lock(10)) 
    {
        lv_label_set_text_fmt(labels[0], "$%d,%03d", (int)(btcPriceUSD/1000), (int)(btcPriceUSD) % 1000);
        lv_label_set_text_fmt(labels[1], "Hashrate: %d.%02d EH/s", (int)(networkHashrate / 1e18), (int)(networkHashrate / 1e18) % 100);
        lv_label_set_text_fmt(labels[2], "Difficulty: %d.%02d T", (int)(networkDifficulty / 1e12), (int)(networkDifficulty / 1e12) % 100);
        lv_label_set_text_fmt(labels[3], "Block Height: %d", blockHeight);
        lv_label_set_text_fmt(labels[4], "Blocks to Halving: %d", 1050000 - blockHeight);
        lv_bar_set_value(labels[5], (int)(difficultyProgressPercent), LV_ANIM_OFF);
        lv_label_set_text_fmt(labels[6], "PROGRESS: %d.%02d%%\nREMAINING BLOCKS: %d\nREMAINING TIME: %dd:%02dh\nESTIMATED CHANGE: %d.%02d%%", 
                (int)(difficultyProgressPercent), (int)(difficultyProgressPercent * 100) % 100,
                remainingBlocksToDifficultyAdjustment,
                remainingTimeToDifficultyAdjustment / 86400, (remainingTimeToDifficultyAdjustment / 3600) % 24,
                (int)(difficultyChangePercent), (int)(difficultyChangePercent * 100) % 100);
        lv_label_set_text_fmt(labels[7], "Fastest Fee: %d SAT/B", fastestFee);
        lv_label_set_text_fmt(labels[8], "Half Hour Fee: %d SAT/B", halfHourFee);
        lv_label_set_text_fmt(labels[9], "Hour Fee: %d SAT/B", hourFee);
        lv_label_set_text_fmt(labels[10], "Economy Fee: %d SAT/B", economyFee);
        lv_label_set_text_fmt(labels[11], "Minimum Fee: %d SAT/B", minimumFee);
        lvgl_port_unlock();
    }
}

void bitcoinNewsScreen()
{
    activeScreen = activeScreenBitcoinNews;

    lv_obj_t* btcPriceContainer = lv_obj_create(screenObjs.bitcoinNewsMainContainer);
    lv_obj_set_size(btcPriceContainer, 320, 192 );
    lv_obj_align(btcPriceContainer, LV_ALIGN_TOP_LEFT, -16, -16);
    lv_obj_set_style_bg_opa(btcPriceContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_opa(btcPriceContainer, LV_OPA_0, LV_PART_MAIN);
    //lv_obj_set_style_border_width(btcPriceContainer, 2, LV_PART_MAIN);
    //lv_obj_set_style_border_color(btcPriceContainer, lv_color_hex(0xA7F3D0), LV_PART_MAIN);

    // BTC Price Label
    lv_obj_t* btcPriceLabel = lv_label_create(btcPriceContainer);
    lv_label_set_text(btcPriceLabel, "BTC PRICE");
    lv_obj_set_style_text_font(btcPriceLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcPriceLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcPriceLabel, LV_ALIGN_TOP_LEFT, 0, -16);
    lv_obj_clear_flag(btcPriceContainer, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Price Value Label
    lv_obj_t* btcPriceValueLabel = lv_label_create(btcPriceContainer);
    lv_label_set_text_fmt(btcPriceValueLabel, "$%d,%03d", (int)(i2cData.api.btcPriceUSD/1000), (int)(i2cData.api.btcPriceUSD) % 1000);
    lv_obj_set_style_text_font(btcPriceValueLabel, &interExtraBold56, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcPriceValueLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcPriceValueLabel, LV_ALIGN_TOP_LEFT, 0, 56);

    // BTC Hashrate and Difficulty Container
    lv_obj_t* btcHashrateDiffContainer = lv_obj_create(screenObjs.bitcoinNewsMainContainer);
    lv_obj_set_size(btcHashrateDiffContainer, 320, 192);
    lv_obj_align(btcHashrateDiffContainer, LV_ALIGN_TOP_RIGHT, 16, -16);
    lv_obj_set_style_bg_opa(btcHashrateDiffContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_opa(btcHashrateDiffContainer, LV_OPA_0, LV_PART_MAIN);
    //lv_obj_set_style_border_width(btcHashrateDiffContainer, 2, LV_PART_MAIN);
    //lv_obj_set_style_border_color(btcHashrateDiffContainer, lv_color_hex(0xA7F3D0), LV_PART_MAIN);

    // BTC Network Hashrate and Difficulty Container Label
    lv_obj_t* btcNetworkMiningContainerLabel = lv_label_create(btcHashrateDiffContainer);
    lv_label_set_text(btcNetworkMiningContainerLabel, "BTC NETWORK");
    lv_obj_set_style_text_font(btcNetworkMiningContainerLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcNetworkMiningContainerLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcNetworkMiningContainerLabel, LV_ALIGN_TOP_LEFT, 0, -16);
    lv_obj_clear_flag(btcNetworkMiningContainerLabel, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Network Hashrate Label
    lv_obj_t* btcNetworkHashrateLabel = lv_label_create(btcHashrateDiffContainer);
    lv_label_set_text_fmt(btcNetworkHashrateLabel, "Hashrate: %d.%02d EH/s", (int)(i2cData.api.networkHashrate / 1e18), (int)(i2cData.api.networkHashrate / 1e18) % 100);
    lv_obj_set_style_text_font(btcNetworkHashrateLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcNetworkHashrateLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcNetworkHashrateLabel, LV_ALIGN_TOP_LEFT, 0, 32);

    // BTC Network Difficulty Label
    lv_obj_t* btcNetworkDifficultyLabel = lv_label_create(btcHashrateDiffContainer);
    lv_label_set_text_fmt(btcNetworkDifficultyLabel, "Difficulty: %d.%02d T", (int)(i2cData.api.networkDifficulty / 1e12), (int)(i2cData.api.networkDifficulty / 1e12) % 100);
    lv_obj_set_style_text_font(btcNetworkDifficultyLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcNetworkDifficultyLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcNetworkDifficultyLabel, LV_ALIGN_TOP_LEFT, 0, 56);

    // BTC Block Height Label
    lv_obj_t* btcBlockHeightLabel = lv_label_create(btcHashrateDiffContainer);
    lv_label_set_text_fmt(btcBlockHeightLabel, "Block Height: %d", i2cData.api.blockHeight);
    lv_obj_set_style_text_font(btcBlockHeightLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcBlockHeightLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcBlockHeightLabel, LV_ALIGN_TOP_LEFT, 0, 80);

    // BTC Remaining Blocks to Halving Label
    lv_obj_t* btcRemainingBlocksToHalvingLabel = lv_label_create(btcHashrateDiffContainer);
    lv_label_set_text_fmt(btcRemainingBlocksToHalvingLabel, "Blocks to Halving: %d", 1050000 - i2cData.api.blockHeight);
    lv_obj_set_style_text_font(btcRemainingBlocksToHalvingLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcRemainingBlocksToHalvingLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcRemainingBlocksToHalvingLabel, LV_ALIGN_TOP_LEFT, 0, 104);

    // BTC Difficulty Stats Container
    lv_obj_t* btcDifficultyStatsContainer = lv_obj_create(screenObjs.bitcoinNewsMainContainer);
    lv_obj_set_size(btcDifficultyStatsContainer, 320, 184);
    lv_obj_align(btcDifficultyStatsContainer, LV_ALIGN_BOTTOM_LEFT, -16, 16);
    lv_obj_set_style_bg_opa(btcDifficultyStatsContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(btcDifficultyStatsContainer, 2, LV_PART_MAIN);
    lv_obj_set_style_border_opa(btcDifficultyStatsContainer, LV_OPA_0, LV_PART_MAIN);
    //lv_obj_set_style_border_color(btcDifficultyStatsContainer, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_clear_flag(btcDifficultyStatsContainer, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Difficulty Progress Label
    lv_obj_t* btcDifficultyProgressLabel = lv_label_create(btcDifficultyStatsContainer);
    lv_label_set_text(btcDifficultyProgressLabel, "DIFFICULTY PROGRESS");
    lv_obj_set_style_text_font(btcDifficultyProgressLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcDifficultyProgressLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_set_style_text_align(btcDifficultyProgressLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_align(btcDifficultyProgressLabel, LV_ALIGN_TOP_LEFT, 0, -16);
    lv_obj_clear_flag(btcDifficultyProgressLabel, LV_OBJ_FLAG_SCROLLABLE);

    //Difficulty Progress Bar
    static lv_style_t styleBarBG;
    static lv_style_t styleBarIndicator;
    lv_style_init(&styleBarIndicator);
    //lv_style_set_radius(&styleBar, LV_RADIUS_CIRCLE);
    lv_style_set_border_color(&styleBarIndicator, lv_color_hex(0xA7F3D0));
    lv_style_set_bg_opa(&styleBarIndicator, LV_OPA_70);
    lv_style_set_bg_color(&styleBarIndicator, lv_color_hex(0xA7F3D0));
    lv_style_set_radius(&styleBarIndicator, 8);

    lv_style_init(&styleBarBG);
    lv_style_set_border_color(&styleBarBG, lv_color_hex(0xA7F3D0));
    lv_style_set_bg_opa(&styleBarBG, LV_OPA_30);
    lv_style_set_bg_color(&styleBarBG, lv_color_hex(0xA7F3D0));
    lv_style_set_radius(&styleBarBG, 12);

    lv_obj_t* btcDifficultyProgressBar = lv_bar_create(btcDifficultyStatsContainer);
    lv_obj_set_size(btcDifficultyProgressBar, 304, 144);
    lv_obj_align(btcDifficultyProgressBar, LV_ALIGN_TOP_LEFT, -16, 16);
    lv_obj_add_style(btcDifficultyProgressBar, &styleBarBG, LV_PART_MAIN);
    lv_obj_add_style(btcDifficultyProgressBar, &styleBarIndicator, LV_PART_INDICATOR);
    lv_bar_set_value(btcDifficultyProgressBar, (int)(i2cData.api.difficultyProgressPercent), LV_ANIM_OFF);
    //lv_obj_set_style_bg_opa(btcDifficultyProgressBar, LV_OPA_80, LV_PART_MAIN);
    
    // BTC Difficulty Progress Percent Label
    lv_obj_t* btcDifficultyProgressPercentLabel = lv_label_create(btcDifficultyProgressBar);
    lv_label_set_text_fmt(btcDifficultyProgressPercentLabel, "PROGRESS: %d.%02d%%\nREMAINING BLOCKS: %d\nREMAINING TIME: %dd:%02dh\nESTIMATED CHANGE: %d.%02d%%", 
        (int)(i2cData.api.difficultyProgressPercent), (int)(i2cData.api.difficultyProgressPercent * 100) % 100,
        i2cData.api.remainingBlocksToDifficultyAdjustment,
        i2cData.api.remainingTimeToDifficultyAdjustment / 86400, (i2cData.api.remainingTimeToDifficultyAdjustment / 3600) % 24,
        (int)(i2cData.api.difficultyChangePercent), (int)(i2cData.api.difficultyChangePercent * 100) % 100);
    lv_obj_set_style_text_font(btcDifficultyProgressPercentLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcDifficultyProgressPercentLabel, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(btcDifficultyProgressPercentLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(btcDifficultyProgressPercentLabel, LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_long_mode(btcDifficultyProgressPercentLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(btcDifficultyProgressPercentLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(btcDifficultyProgressPercentLabel, 288);
    //lv_obj_set_style_border_width(btcDifficultyProgressPercentLabel, 2, LV_PART_MAIN);
    //lv_obj_set_style_border_color(btcDifficultyProgressPercentLabel, lv_color_hex(0x000000), LV_PART_MAIN);

    // BTC Fees Container
    lv_obj_t* btcFeesContainer = lv_obj_create(screenObjs.bitcoinNewsMainContainer);
    lv_obj_set_size(btcFeesContainer, 320, 184);
    lv_obj_align(btcFeesContainer, LV_ALIGN_BOTTOM_RIGHT, 16, 16);
    lv_obj_set_style_bg_opa(btcFeesContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(btcFeesContainer, 2, LV_PART_MAIN);
    lv_obj_set_style_border_opa(btcFeesContainer, LV_OPA_0, LV_PART_MAIN);
    //lv_obj_set_style_border_color(btcFeesContainer, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_clear_flag(btcFeesContainer, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Fees Label
    lv_obj_t* btcFeesLabel = lv_label_create(btcFeesContainer);
    lv_label_set_text(btcFeesLabel, "NETWORK FEES");
    lv_obj_set_style_text_font(btcFeesLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcFeesLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcFeesLabel, LV_ALIGN_TOP_LEFT, 0, -16);
    lv_obj_clear_flag(btcFeesLabel, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Fastest Fee Label
    lv_obj_t* btcFastestFeeLabel = lv_label_create(btcFeesContainer);
    lv_label_set_text_fmt(btcFastestFeeLabel, "Fastest Fee: %d SAT/B", (int)(i2cData.api.fastestFee));
    lv_obj_set_style_text_font(btcFastestFeeLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcFastestFeeLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcFastestFeeLabel, LV_ALIGN_TOP_LEFT, 0, 32);
    lv_obj_clear_flag(btcFastestFeeLabel, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Half Hour Fee Label
    lv_obj_t* btcHalfHourFeeLabel = lv_label_create(btcFeesContainer);
    lv_label_set_text_fmt(btcHalfHourFeeLabel, "Half Hour Fee: %d SAT/B", (int)(i2cData.api.halfHourFee));
    lv_obj_set_style_text_font(btcHalfHourFeeLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcHalfHourFeeLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcHalfHourFeeLabel, LV_ALIGN_TOP_LEFT, 0, 56);
    lv_obj_clear_flag(btcHalfHourFeeLabel, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Hour Fee Label
    lv_obj_t* btcHourFeeLabel = lv_label_create(btcFeesContainer);
    lv_label_set_text_fmt(btcHourFeeLabel, "Hour Fee: %d SAT/B", (int)(i2cData.api.hourFee));
    lv_obj_set_style_text_font(btcHourFeeLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcHourFeeLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcHourFeeLabel, LV_ALIGN_TOP_LEFT, 0, 80);
    lv_obj_clear_flag(btcHourFeeLabel, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Economy Fee Label   
    lv_obj_t* btcEconomyFeeLabel = lv_label_create(btcFeesContainer);
    lv_label_set_text_fmt(btcEconomyFeeLabel, "Economy Fee: %d SAT/B", (int)(i2cData.api.economyFee));
    lv_obj_set_style_text_font(btcEconomyFeeLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcEconomyFeeLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcEconomyFeeLabel, LV_ALIGN_TOP_LEFT, 0, 104);
    lv_obj_clear_flag(btcEconomyFeeLabel, LV_OBJ_FLAG_SCROLLABLE);

    // BTC Minimum Fee Label
    lv_obj_t* btcMinimumFeeLabel = lv_label_create(btcFeesContainer);
    lv_label_set_text_fmt(btcMinimumFeeLabel, "Minimum Fee: %d SAT/B", (int)(i2cData.api.minimumFee));
    lv_obj_set_style_text_font(btcMinimumFeeLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcMinimumFeeLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(btcMinimumFeeLabel, LV_ALIGN_TOP_LEFT, 0, 128);
    lv_obj_clear_flag(btcMinimumFeeLabel, LV_OBJ_FLAG_SCROLLABLE);

    static lv_obj_t* mempoolAPILabels[12]; // Adjust size as needed
    mempoolAPILabels[0] = btcPriceValueLabel;
    mempoolAPILabels[1] = btcNetworkHashrateLabel;
    mempoolAPILabels[2] = btcNetworkDifficultyLabel;
    mempoolAPILabels[3] = btcBlockHeightLabel;
    mempoolAPILabels[4] = btcRemainingBlocksToHalvingLabel;
    mempoolAPILabels[5] = btcDifficultyProgressBar;
    mempoolAPILabels[6] = btcDifficultyProgressPercentLabel;
    mempoolAPILabels[7] = btcFastestFeeLabel;
    mempoolAPILabels[8] = btcHalfHourFeeLabel;
    mempoolAPILabels[9] = btcHourFeeLabel;
    mempoolAPILabels[10] = btcEconomyFeeLabel;
    mempoolAPILabels[11] = btcMinimumFeeLabel;

    screenObjs.apiUpdateTimer = lv_timer_create(updateMempoolAPIInfo, 3000, mempoolAPILabels);
}
static lv_obj_t * kb;
static void ta_event_cb(lv_event_t * e);

static void ta_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
        if(kb != NULL) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            // Disable animation for showing keyboard
            lv_obj_set_style_anim_time(kb, 0, 0);
        }
    }
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL || code == LV_EVENT_DEFOCUSED) {
        LV_LOG_USER("Ready, current text: %s", lv_textarea_get_text(ta));
        // Disable animation for hiding keyboard
        lv_obj_set_style_anim_time(kb, 0, 0);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

void settingsScreen()
{
    activeScreen = activeScreenSettings;

    lv_obj_t* settingsContainer = lv_obj_create(screenObjs.settingsMainContainer);
    lv_obj_set_size(settingsContainer, 320, 192 );
    lv_obj_align(settingsContainer, LV_ALIGN_TOP_LEFT, -16, -16);
    lv_obj_set_style_bg_opa(settingsContainer, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_opa(settingsContainer, LV_OPA_100, LV_PART_MAIN);

    // Settings Label
    lv_obj_t* settingsLabel = lv_label_create(settingsContainer);
    lv_label_set_text(settingsLabel, "SETTINGS");
    lv_obj_set_style_text_font(settingsLabel, &interMedium24, LV_PART_MAIN);
    lv_obj_set_style_text_color(settingsLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(settingsLabel, LV_ALIGN_TOP_LEFT, 0, -16);
    lv_obj_clear_flag(settingsLabel, LV_OBJ_FLAG_SCROLLABLE);

    // Wifi Text Area
    lv_obj_t* wifiTextArea = lv_textarea_create(settingsContainer);
    lv_textarea_set_one_line(wifiTextArea, true);
    lv_textarea_set_text(wifiTextArea, "WIFI");
    lv_obj_align(wifiTextArea, LV_ALIGN_TOP_LEFT, 0, 16);
    lv_obj_set_width(wifiTextArea, lv_pct(40));
    lv_obj_add_event_cb(wifiTextArea, ta_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_clear_flag(wifiTextArea, LV_OBJ_FLAG_SCROLLABLE);

    kb = lv_keyboard_create(lv_scr_act());
    lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);

    // Disable animations
    lv_obj_set_style_anim_time(kb, 0, 0);

    // Remove button matrix press state animations
    lv_obj_set_style_bg_color(kb, lv_color_hex(0x161f1b), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(kb, lv_color_hex(0xA7F3D0), LV_PART_ITEMS | LV_STATE_PRESSED);

    // Disable state transitions
    lv_obj_remove_style_all(kb);  // Remove default styles
    lv_obj_set_style_bg_opa(kb, LV_OPA_80, 0);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_keyboard_set_textarea(kb, wifiTextArea);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
}
