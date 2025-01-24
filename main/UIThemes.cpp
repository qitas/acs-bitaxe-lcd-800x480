#include "UIThemes.h"
#include "fonts.h"

uiTheme_t currentTheme;

// Default theme values (Original ACS green theme)
static const uiTheme_t defaultTheme = {
    .primaryColor = lv_color_hex(0xA7F3D0),
    .secondaryColor = lv_color_hex(0xA7F3D0),
    .backgroundColor = lv_color_hex(0x161f1b),
    .textColor = lv_color_hex(0xA7F3D0),
    .borderColor = lv_color_hex(0xA7F3D0),
    .defaultOpacity = 80,
    .backgroundOpacity = 40,
    .fontExtraBold144 = &interExtraBold144,
    .fontExtraBold56 = &interExtraBold56,
    .fontExtraBold32 = &interExtraBold32,
    .fontMedium24 = &interMedium24,
    .fontMedium16 = &interMedium16_19px,
    .background = "S:/UIBackgroundACSDEFAULT.png",
    .logo1 = "S:/Logos.png",
    .logo2 = "S:/openSourceBitcoinMining.png",
    .themePreview = "S:/acsGreenUIPreview.png",
    .themePreset = THEME_DEFAULT
};

uiTheme_t* getCurrentTheme(void) {
    return &currentTheme;
}

themePreset_t getCurrentThemePreset(void) {
    return currentTheme.themePreset;
}


void initializeTheme(themePreset_t preset) {
    switch (preset) {
        
        case THEME_BITAXE_RED:
            currentTheme = (uiTheme_t){
                .primaryColor = lv_color_hex(0xF80421),
                .secondaryColor = lv_color_hex(0xfc4d62),
                .backgroundColor = lv_color_hex(0x070D17),
                .textColor = lv_color_hex(0xF80421),
                .borderColor = lv_color_hex(0xfc4d62),
                .defaultOpacity = 80,
                .backgroundOpacity = 40,
                .fontExtraBold144 = &interExtraBold144,
                .fontExtraBold56 = &interExtraBold56,
                .fontExtraBold32 = &interExtraBold32,
                .fontMedium24 = &interMedium24,
                .fontMedium16 = &interMedium16_19px,
                .background = "S:/UIBackgroundBITAXERED.png",
                .logo1 = "S:/Logos.png",
                .logo2 = "S:/openSourceBitcoinMining.png",
                .themePreview = "S:/bitaxeRedUIPreview.png",
                .themePreset = THEME_BITAXE_RED
            };
            break;
        #if (BlockStreamJade == 1)
        case THEME_BLOCKSTREAM_JADE:
            currentTheme = (uiTheme_t){
                .primaryColor = lv_color_hex(0x00B093),
                .secondaryColor = lv_color_hex(0x006D62),
                .backgroundColor = lv_color_hex(0x111316),
                .textColor = lv_color_hex(0x21CCAB),
                .borderColor = lv_color_hex(0x01544A),
                .defaultOpacity = 80,
                .backgroundOpacity = 40,
                .fontExtraBold144 = &interExtraBold144,
                .fontExtraBold56 = &interExtraBold56,
                .fontExtraBold32 = &interExtraBold32,
                .fontMedium24 = &interMedium24,
                .fontMedium16 = &interMedium16_19px,
                .background = "S:/UIBackgroundJADEGREEN.png",
                .logo1 = "S:/Logos.png",
                .logo2 = "S:/UILogoJADE.png",
                .themePreview = "S:/jadeUIPreview.png",
                .themePreset = THEME_BLOCKSTREAM_JADE
            };
            break;
       

        case THEME_BLOCKSTREAM_BLUE:
            currentTheme = (uiTheme_t){
                .primaryColor = lv_color_hex(0x00C3FF),
                .secondaryColor = lv_color_hex(0x00C3FF),
                .backgroundColor = lv_color_hex(0x111316),
                .textColor = lv_color_hex(0x00C3FF),
                .borderColor = lv_color_hex(0x00C3FF),
                .defaultOpacity = 80,
                .backgroundOpacity = 40,
                .fontExtraBold144 = &interExtraBold144,
                .fontExtraBold56 = &interExtraBold56,
                .fontExtraBold32 = &interExtraBold32,
                .fontMedium24 = &interMedium24,
                .fontMedium16 = &interMedium16_19px,
                .background = "S:/UIBackgroundBLKSTRMBLUE.png",
                .logo1 = "S:/Logos.png",
                .logo2 = "S:/bsBlueLogo.png",
                .themePreview = "S:/bsBlueUIPreview.png",
                .themePreset = THEME_BLOCKSTREAM_BLUE
            };
            break;
        #endif
        
        default:
            currentTheme = defaultTheme;
            break;
    }
}