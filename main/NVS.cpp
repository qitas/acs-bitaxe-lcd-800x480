#include "NVS.h"
#include "Arduino.h"
#include <nvs_flash.h>
#include <nvs.h>
#include "UIThemes.h"

void initializeNVS(void);
themePreset_t loadThemeFromNVS(void);
void saveThemeToNVS(themePreset_t preset);


void initializeNVS(void) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

themePreset_t loadThemeFromNVS(void) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE_THEMES, NVS_READONLY, &nvsHandle);

    #if (BlockStreamJade == 1)
    uint8_t themeValue = 2; // Make default theme the Jade Theme
    #else
    uint8_t themeValue = 0; // Make default theme the ACS Theme
    #endif
    if (err == ESP_OK) {
        err = nvs_get_u8(nvsHandle, NVS_KEY_THEME, &themeValue);
        nvs_close(nvsHandle);
    }
    
    return (themePreset_t)themeValue;
}

void saveThemeToNVS(themePreset_t preset) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE_THEMES, NVS_READWRITE, &nvsHandle);
    
    if (err == ESP_OK) {
        err = nvs_set_u8(nvsHandle, NVS_KEY_THEME, (uint8_t)preset);
        if (err == ESP_OK) {
            nvs_commit(nvsHandle);
        }
        nvs_close(nvsHandle);
    }
}

bool isFirstBoot(void) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE_FLAGS, NVS_READONLY, &nvsHandle);
    
    uint8_t firstBoot = 1; // Default to true if not found
    Serial0.printf("isFirstBoot: %d\n", firstBoot);
    if (err == ESP_OK) {
        err = nvs_get_u8(nvsHandle, NVS_KEY_FIRST_BOOT, &firstBoot);
        nvs_close(nvsHandle);
    }
    Serial0.printf("isFirstBoot: %d\n", firstBoot);
    return firstBoot;
}

void setFirstBootComplete(void) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE_FLAGS, NVS_READWRITE, &nvsHandle);
    
    if (err == ESP_OK) {
        err = nvs_set_u8(nvsHandle, NVS_KEY_FIRST_BOOT, 0);
        if (err == ESP_OK) {
            nvs_commit(nvsHandle);
        }
        nvs_close(nvsHandle);
    }
    Serial0.printf("setFirstBootComplete: %d\n", 0);
}

void saveSettingsToNVSasString(const char* key, const char* value, size_t size)  {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE_SETTINGS, NVS_READWRITE, &nvsHandle);

    if (err == ESP_OK) {
        err = nvs_set_str(nvsHandle, key, value);
        if (err == ESP_OK) {
            nvs_commit(nvsHandle);
        }
        nvs_close(nvsHandle);
    }
    Serial0.printf("saveSettingsToNVSasString: %s\n", key);
    Serial0.printf("saveSettingsToNVSasString: %s\n", value);
}

void loadSettingsFromNVSasString(const char* key, char* value, size_t size) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE_SETTINGS, NVS_READONLY, &nvsHandle);
    
    // Set default value (empty string) if read fails
    value[0] = '\0';
    
    if (err == ESP_OK) {
        err = nvs_get_str(nvsHandle, key, value, &size);
        nvs_close(nvsHandle);
    }
    Serial0.printf("loadSettingsFromNVSasString: %s\n", key);
    Serial0.printf("loadSettingsFromNVSasString: %s\n", value);
}

void factoryResetNVS(void) {
    nvs_flash_erase();
    initializeNVS();
    delay(20);
    esp_restart();
}
