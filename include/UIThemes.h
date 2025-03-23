#pragma once

#include "lvgl.h"
#include "modelConfig.h"
// Theme presets
typedef enum {
    THEME_DEFAULT = 0, 
    THEME_BITAXE_RED = 1,
    
    #if (BlockStreamJade == 1)
    THEME_BLOCKSTREAM_JADE = 2,
    THEME_BLOCKSTREAM_BLUE = 3,
    #endif
    #if (SoloSatoshi == 1)
    THEME_SOLO_SATOSHI = 4,
    #endif
    #if (ALTAIR == 1)
    THEME_ALTAIR = 5
    #endif
    #if (SoloMiningCo == 1)
    THEME_SOLO_MINING_CO = 6
    #endif
    #if (BTCMagazine == 1)
    THEME_BTCMAGAZINE = 7
    #endif
} themePreset_t;


// Theme structure to hold all colors
typedef struct {
    lv_color_t primaryColor;      // Main brand color
    lv_color_t secondaryColor;    // Secondary brand color
    lv_color_t backgroundColor;   // Background color
    lv_color_t textColor;         // Text color
    lv_color_t borderColor;       // Border color
    uint8_t defaultOpacity;       // Default opacity for elements
    uint8_t backgroundOpacity;    // Background opacity
    lv_font_t* fontExtraBold144;
    lv_font_t* fontExtraBold72;
    lv_font_t* fontExtraBold56;
    lv_font_t* fontExtraBold32;
    lv_font_t* fontMedium24;
    lv_font_t* fontMedium16;
    const char* background;
    const char* logo1;
    const char* logo2;
    const char* themePreview;
    themePreset_t themePreset;

} uiTheme_t;



// Function declarations
void initializeTheme(themePreset_t preset);
void setCustomTheme(uiTheme_t newTheme);
themePreset_t getCurrentThemePreset(void);
uiTheme_t* getCurrentTheme(void);
void applyThemeToObject(lv_obj_t* obj, bool includeBorder);

extern uiTheme_t currentTheme;
