#include "UISharedAssets.h"
#include "UIScreens.h"
#include "UIHandler.h"
#include "I2CData.h"
#include "fonts.h"
#include <lvgl.h>

lv_timer_t* statusBarTimer = NULL;
lv_obj_t* statusBarTempLabel = NULL;
lv_obj_t* statusBarHashrateLabel = NULL;
lv_obj_t* statusBarPowerLabel = NULL;


void lvglTabIcons(lv_obj_t*parent)
{

    lv_obj_t* tab = lv_img_create(parent);
    lv_obj_set_size(tab, 128, 400);
    //lv_obj_set_style_border_width(tab, 2, LV_PART_MAIN);
    //lv_obj_set_style_border_color(tab, lv_color_hex(0xFF0000), LV_PART_MAIN);
    //lv_obj_set_style_border_opa(tab, LV_OPA_50, LV_PART_MAIN);
    lv_obj_align(tab, LV_ALIGN_LEFT_MID, -16, 16);

    tabHome = lv_img_create(tab);
    lv_img_set_src(tabHome, "S:/house.png");
    //lv_img_set_zoom(tabHome, 597);
    lv_obj_align(tabHome, LV_ALIGN_CENTER, 0,-164);
    lv_obj_add_flag(tabHome, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(tabHome, 24);
    // Add these lines for debug border
    //lv_obj_set_style_border_width(tabHome, 2, LV_PART_MAIN);
    //lv_obj_set_style_border_color(tabHome, lv_color_hex(0xFF0000), LV_PART_MAIN);
    //lv_obj_set_style_border_opa(tabHome, LV_OPA_50, LV_PART_MAIN);
    lv_obj_add_event_cb(tabHome, tabIconEventHandler, LV_EVENT_CLICKED, NULL);
    
    tabMining = lv_img_create(tab);
    lv_img_set_src(tabMining, "S:/pickaxe.png");
   // lv_img_set_zoom(tabMining, 597);
    lv_obj_align(tabMining, LV_ALIGN_CENTER, 0, -56);
    lv_obj_add_flag(tabMining, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(tabMining, 24);
    // Add these lines for debug border
    //lv_obj_set_style_border_width(tabMining, 2, LV_PART_MAIN);
    //lv_obj_set_style_border_color(tabMining, lv_color_hex(0xFF0000), LV_PART_MAIN);
    //lv_obj_set_style_border_opa(tabMining, LV_OPA_50, LV_PART_MAIN);
    lv_obj_add_event_cb(tabMining, tabIconEventHandler, LV_EVENT_CLICKED, NULL);
    
    tabActivity = lv_img_create(tab);
    lv_img_set_src(tabActivity, "S:/activity.png");
   // lv_img_set_zoom(tabActivity, 597);
    lv_obj_align(tabActivity, LV_ALIGN_CENTER, 0, 56);
    lv_obj_add_flag(tabActivity, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(tabActivity, 24);
    // Add these lines for debug border
   // lv_obj_set_style_border_width(tabActivity, 2, LV_PART_MAIN);
   // lv_obj_set_style_border_color(tabActivity, lv_color_hex(0xFF0000), LV_PART_MAIN);
  //  lv_obj_set_style_border_opa(tabActivity, LV_OPA_50, LV_PART_MAIN);
    lv_obj_add_event_cb(tabActivity, tabIconEventHandler, LV_EVENT_CLICKED, NULL);

    
    tabSettings = lv_img_create(tab);
    lv_img_set_src(tabSettings, "S:/bitcoin.png");
   // lv_img_set_zoom(tabSettings, 597);
    lv_obj_align(tabSettings, LV_ALIGN_CENTER, 0, 164);
    lv_obj_add_flag(tabSettings, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(tabSettings, 24);
    // Add these lines for debug border
   // lv_obj_set_style_border_width(tabSettings, 2, LV_PART_MAIN);
   // lv_obj_set_style_border_color(tabSettings, lv_color_hex(0xFF0000), LV_PART_MAIN);
   // lv_obj_set_style_border_opa(tabSettings, LV_OPA_50, LV_PART_MAIN);
    lv_obj_add_event_cb(tabSettings, tabIconEventHandler, LV_EVENT_CLICKED, NULL);
}

void backgroundImage(lv_obj_t* parent)
{
    lv_obj_t* background = lv_img_create(parent);
    lv_img_set_src(background, "S:/UIBackground.png");
    lv_obj_center(background);
    lv_obj_set_style_bg_opa(background, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(background, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(background, LV_OPA_40, LV_PART_MAIN);
}



void statusBar(lv_obj_t* parent)
{
    lv_obj_t* statusBar = lv_img_create(parent);
    lv_obj_set_size(statusBar, 696, 64);
    lv_obj_align(statusBar, LV_ALIGN_TOP_RIGHT, 0, 8);
    //lv_obj_set_style_border_width(statusBar, 4, LV_PART_MAIN);
    //lv_obj_set_style_border_color(statusBar, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    //lv_obj_set_style_border_opa(statusBar, LV_OPA_50, LV_PART_MAIN);

    lv_obj_t* wifiIcon = lv_img_create(statusBar);
    lv_img_set_src(wifiIcon, "S:/wifi40x40.png");
    //lv_img_set_zoom(wifiIcon, 512);
    lv_obj_align(wifiIcon, LV_ALIGN_TOP_RIGHT, -8, 0);

    lv_obj_t* tempIcon = lv_img_create(statusBar);
    lv_img_set_src(tempIcon, "S:/temp40x40.png");
    lv_obj_align(tempIcon, LV_ALIGN_TOP_RIGHT, -192, 8);

    statusBarTempLabel = lv_label_create(statusBar);
    lv_label_set_text_fmt(statusBarTempLabel, "%d°C", (int)i2cData.monitoring.temperatures[0]);
    lv_obj_set_style_text_font(statusBarTempLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_align(statusBarTempLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(statusBarTempLabel, 80);
    lv_label_set_long_mode(statusBarTempLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(statusBarTempLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    //lv_obj_set_style_border_width(statusBarTempLabel, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(statusBarTempLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(statusBarTempLabel, LV_ALIGN_TOP_RIGHT, -104, 0);


    lv_obj_t* cpuIcon = lv_img_create(statusBar);
    lv_img_set_src(cpuIcon, "S:/cpu40x40.png");
    //lv_img_set_zoom(cpuIcon, 512);
    lv_obj_align(cpuIcon, LV_ALIGN_TOP_MID, 8, 0);

    statusBarHashrateLabel = lv_label_create(statusBar);
    lv_label_set_text_fmt(statusBarHashrateLabel, "%d GH/s\n%d W/TH", (int)i2cData.mining.hashrate, (int)i2cData.mining.efficiency);
    lv_obj_set_style_text_font(statusBarHashrateLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_align(statusBarHashrateLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(statusBarHashrateLabel, 96);
    lv_label_set_long_mode(statusBarHashrateLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(statusBarHashrateLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    //lv_obj_set_style_border_width(statusBarHashrateLabel, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(statusBarHashrateLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(statusBarHashrateLabel, LV_ALIGN_TOP_MID, 80, 0);

    lv_obj_t* pwrIcon = lv_img_create(statusBar);
    lv_img_set_src(pwrIcon, "S:/plugZap40x40.png");
    //lv_img_set_zoom(pwrIcon, 512);
    lv_obj_align(pwrIcon, LV_ALIGN_TOP_MID, -128, 0);

    statusBarPowerLabel = lv_label_create(statusBar);
    lv_label_set_text_fmt(statusBarPowerLabel, "%d.%03d V\n%d.%02d W", 
        (int)(i2cData.monitoring.powerStats.domainVoltage/1000), (int)(i2cData.monitoring.powerStats.domainVoltage) % 1000, 
        (int)(i2cData.monitoring.powerStats.power), (int)(i2cData.monitoring.powerStats.power * 100) % 100);
    lv_obj_set_style_text_font(statusBarPowerLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_align(statusBarPowerLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(statusBarPowerLabel, 88);
    lv_label_set_long_mode(statusBarPowerLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(statusBarPowerLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    //lv_obj_set_style_border_width(statusBarPowerLabel, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(statusBarPowerLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    lv_obj_align(statusBarPowerLabel, LV_ALIGN_TOP_MID, -64, 0);

    lv_obj_t* btcPrice = lv_img_create(statusBar);
    lv_img_set_src(btcPrice, "S:/bitcoin40x40.png");
    //lv_img_set_zoom(btcPrice, 512);
    lv_obj_align(btcPrice, LV_ALIGN_TOP_LEFT, 16, 0);

    lv_obj_t* btcPriceLabel = lv_label_create(statusBar);
    lv_label_set_text(btcPriceLabel, "$98,395.25");
    lv_obj_set_style_text_align(btcPriceLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_width(btcPriceLabel, 144);
    lv_label_set_long_mode(btcPriceLabel, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(btcPriceLabel, &interMedium16_19px, LV_PART_MAIN);
    lv_obj_set_style_text_color(btcPriceLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    //lv_obj_set_style_border_width(btcPriceLabel, 1, LV_PART_MAIN);
    //lv_obj_set_style_border_color(btcPriceLabel, lv_color_hex(0xA7F3D0), LV_PART_MAIN);
    //lv_obj_set_style_text_opa(btcPriceLabel, 224, LV_PART_MAIN);
    lv_obj_align(btcPriceLabel, LV_ALIGN_TOP_LEFT, 56, 0);

    

    screenObjs.statusBarUpdateTimer = lv_timer_create([](lv_timer_t* timer)
    {
        int temp = i2cData.monitoring.temperatures[0];
        int hashrate = i2cData.mining.hashrate;
        int efficiency = i2cData.mining.efficiency;
        float domainVoltage = i2cData.monitoring.powerStats.domainVoltage;
        float power = i2cData.monitoring.powerStats.power;

        if (lvgl_port_lock(10))
        {
            lv_label_set_text_fmt(statusBarTempLabel, "%d°C", temp);
            lv_label_set_text_fmt(statusBarHashrateLabel, "%d GH/s\n%d W/TH", hashrate, efficiency);
            lv_label_set_text_fmt(statusBarPowerLabel, "%d.%03d V\n%d.%02d W", 
                (int)(domainVoltage/1000), (int)(domainVoltage) % 1000, 
                (int)(power), (int)(power * 100) % 100);
            lvgl_port_unlock();
        }
    }, 2500, NULL);
}

lv_style_t mainContainerBorder;
lv_obj_t* mainContainerObj;
void mainContainer(lv_obj_t* parent)
{
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(container, LV_OPA_0, LV_PART_MAIN);
    static lv_style_t mainContainerBorder;
    lv_style_init(&mainContainerBorder);
    lv_style_set_border_width(&mainContainerBorder, 4);
    lv_style_set_radius(&mainContainerBorder, 36);
    lv_style_set_border_color(&mainContainerBorder, lv_color_hex(0xA7F3D0));
    lv_style_set_border_opa(&mainContainerBorder, LV_OPA_0);
    //lv_style_set_border_side(&mainContainerBorder, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP);
    lv_style_set_bg_opa(&mainContainerBorder, LV_OPA_0);
    lv_style_set_shadow_width(&mainContainerBorder, 80);
    lv_style_set_shadow_color(&mainContainerBorder, lv_color_hex(0xA7F3D0));
    lv_style_set_shadow_opa(&mainContainerBorder, LV_OPA_30);
    lv_style_set_shadow_spread(&mainContainerBorder, 16);
    lv_style_set_shadow_ofs_x(&mainContainerBorder, 0);
    lv_style_set_shadow_ofs_y(&mainContainerBorder, 0);
    lv_obj_add_style(container, &mainContainerBorder, 0);
    lv_obj_set_size(container, 672, 392);
    lv_obj_align(container, LV_ALIGN_CENTER, 40, 28);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
}






