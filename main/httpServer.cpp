#include "httpServer.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include "spiffsDriver.h"
#include "esp_ota_ops.h"
#include <Update.h>
#include <algorithm>
#include <string>
#include <esp_partition.h>

WebServer* setupServer = nullptr;

static void rootPage() {
   String html = "<html><head><style>";
   html += "body { font-family: Arial, sans-serif; background-color: #111316; color: #00B093; }";
   html += ".container { max-width: 600px; margin: 40px auto; padding: 20px; }";
   html += "input[type='text'] { width: 100%; padding: 12px; margin: 8px 0; ";
   html += "border: 2px solid #01544A; border-radius: 4px; background: #111316; color: #21CCAB; }";
   html += "button { background-color: #006D62; color: white; padding: 14px 20px; margin: 8px 0; ";
   html += "border: none; border-radius: 4px; cursor: pointer; width: 100%; }";
   html += "button:hover { background-color: #45a049; }";
   html += ".logo { display: block; margin: 0 auto 20px auto; max-width: 200px; }";
   html += "</style></head><body>";
   html += "<div class='container'>";
   html += "<img src='/publicPool200x200.png' alt='Logo' class='logo'>";
   html += "<h1>BTC Address Input</h1>";
   html += "<form action='/submitAddress' method='POST'>";
   html += "<input type='text' name='btcAddress' placeholder='Enter BTC Address' maxlength='64'>";
   html += "<button type='submit'>Send</button>";
   html += "</form></div></body></html>";
   setupServer->send(200, "text/html", html);
}

String setupBTCAddress;

void handleSubmit() {
    if (setupServer->hasArg("btcAddress")) {
        String btcAddress = setupServer->arg("btcAddress");
        setupBTCAddress = btcAddress;
        setupServer->sendHeader("Location", "/");
        setupServer->send(303);
    }
}

static void handleImageRequest() {
    if (!SPIFFS.exists("/publicPool200x200.png")) {
        setupServer->send(404, "text/plain", "Image not found");
        return;
    }

    File file = SPIFFS.open("/publicPool200x200.png", "r");
    if (!file) {
        setupServer->send(500, "text/plain", "Failed to open file");
        return;
    }

    if (file.size() == 0) {
        file.close();
        setupServer->send(500, "text/plain", "File is empty");
        return;
    }

    setupServer->sendHeader("Content-Type", "image/png");
    bool success = setupServer->streamFile(file, "image/png");
    file.close();

    if (!success) {
        Serial0.println("Failed to stream file");
    }
}

static void handleOTA() {
    if (setupServer->method() != HTTP_POST) {
        setupServer->send(400, "text/plain", "Only POST method allowed");
        return;
    }

    HTTPUpload& upload = setupServer->upload();
    static esp_ota_handle_t otaHandle = 0;
    static const esp_partition_t* otaPartition = nullptr;
    static bool isSpiffsUpdate = false;

    switch (upload.status) {
        case UPLOAD_FILE_START:
            ESP_LOGI("OTA", "Update starting: %s", upload.filename.c_str());
            
            // Reset handle at the start of each upload
            otaHandle = 0;
            otaPartition = nullptr;
            isSpiffsUpdate = false;

            {
                std::string filename_lower = upload.filename.c_str();
                std::transform(filename_lower.begin(), filename_lower.end(), filename_lower.begin(), ::tolower);
                if (filename_lower.find("spiffs") != std::string::npos) {
                    isSpiffsUpdate = true;
                    const esp_partition_t* spiffsPartition = esp_partition_find_first(
                        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
                    if (!spiffsPartition) {
                        ESP_LOGE("OTA", "SPIFFS partition not found");
                        setupServer->send(500, "text/plain", "SPIFFS Partition not found");
                        return;
                    }
                    
                    if (!Update.begin(spiffsPartition->size, U_SPIFFS)) {
                        ESP_LOGE("OTA", "SPIFFS update begin failed: %s", Update.errorString());
                        setupServer->send(500, "text/plain", "SPIFFS Update Begin Failed");
                        return;
                    }
                    ESP_LOGI("OTA", "SPIFFS update started successfully");
                } 
                else if (filename_lower.find("lcdfirmware") != std::string::npos) {
                    isSpiffsUpdate = false;
                    otaPartition = esp_ota_get_next_update_partition(NULL);
                    if (!otaPartition) {
                        ESP_LOGE("OTA", "Failed to find next OTA partition");
                        setupServer->send(500, "text/plain", "OTA Partition not found");
                        return;
                    }
                    esp_err_t err = esp_ota_begin(otaPartition, OTA_SIZE_UNKNOWN, &otaHandle);
                    if (err != ESP_OK) {
                        ESP_LOGE("OTA", "OTA begin failed: %d", err);
                        setupServer->send(500, "text/plain", "OTA Begin Failed");
                        return;
                    }
                    ESP_LOGI("OTA", "Firmware update started with handle: %lu", otaHandle);
                }
                else {
                    ESP_LOGE("OTA", "Invalid firmware file name: %s", upload.filename.c_str());
                    setupServer->send(500, "text/plain", "Invalid firmware file. Filename must contain 'spiffs' or 'lcdfirmware'");
                    return;
                }
            }
            break;

        case UPLOAD_FILE_WRITE:
            if (isSpiffsUpdate) {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial0);
                    return;
                }
                Serial0.printf("SPIFFS Written: %d bytes\n", upload.currentSize);
            } else {
                if (esp_ota_write(otaHandle, upload.buf, upload.currentSize) != ESP_OK) {
                    esp_ota_abort(otaHandle);
                    otaHandle = 0;
                    setupServer->send(500, "text/plain", "Flash Write Error");
                    return;
                }
                Serial0.printf("Firmware Written: %d bytes\n", upload.currentSize);
            }
            break;

        case UPLOAD_FILE_END:
            if (isSpiffsUpdate) {
                if (!Update.end(true)) {
                    Update.printError(Serial0);
                    setupServer->send(500, "text/plain", "SPIFFS Update Failed");
                    return;
                }
                Serial0.println("SPIFFS update complete");
                setupServer->send(200, "text/plain", "SPIFFS Update Success! Rebooting...");
                delay(1000);
                ESP.restart();
            } else {
                if (esp_ota_end(otaHandle) != ESP_OK || 
                    esp_ota_set_boot_partition(otaPartition) != ESP_OK) {
                    setupServer->send(500, "text/plain", "OTA End Failed");
                    return;
                }
                otaHandle = 0;
                Serial0.println("Firmware update complete");
                setupServer->send(200, "text/plain", "Firmware Update Success! Rebooting...");
                delay(1000);
                ESP.restart();
            }
            break;

        case UPLOAD_FILE_ABORTED:
            if (isSpiffsUpdate) {
                Update.end();
                Serial0.println("SPIFFS update aborted");
            } else {
                if (otaHandle) {
                    esp_ota_abort(otaHandle);
                    otaHandle = 0;
                }
                Serial0.println("Firmware update aborted");
            }
            break;
    }
}

static void handleOTAPage() {
    String html = F(
        "<html><head>"
        "<title>Firmware Update</title>"
        "<meta charset='utf-8' />"
        "<meta name='viewport' content='width=device-width,initial-scale=1' />"
        "<style>"
        "body { font-family: Arial, sans-serif; background-color: #111316; color: #00b093; }"
        "* { box-sizing: border-box; }"
        ".container { max-width: 600px; margin: 40px auto; padding: 20px; }"
        ".section { background: rgba(1, 84, 74, 0.1); border: 1px solid #01544a; "
        "border-radius: 8px; padding: 20px; margin-bottom: 30px; overflow: hidden; }"
        "h1 { color: #00b093; margin-top: 0; }"
        "input[type='file'] { width: 100%; padding: 10px; margin: 8px 0; "
        "border: 2px solid #01544a; border-radius: 4px; background: #111316; "
        "color: #21ccab; display: block; }"
        ".btn { background-color: #006d62; color: white; padding: 14px 20px; "
        "margin: 8px 0; border: none; border-radius: 4px; cursor: pointer; "
        "width: 100%; display: block; }"
        ".btn:hover { background-color: #45a049; }"
        ".section-description { color: #21ccab; margin-bottom: 20px; font-size: 0.9em; }"
        "</style>"
        "<script>"
        "function showUpdateMessage(formElement) {"
        "   if(confirm('Start firmware update? Device will restart when complete.')) {"
        "      alert('Update started. Please wait until the device restarts.');"
        "      return true;"
        "   }"
        "   return false;"
        "}"
        "</script>"
        "</head><body>"
        "<div class='container'>"
        "<div class='section'>"
        "<h1>LCD Firmware Update</h1>"
        "<p class='section-description'>Upload your lcdFirmware.bin file to update the LCD firmware</p>"
        "<form method='POST' action='/update' enctype='multipart/form-data' onsubmit='return showUpdateMessage(this);'>"
        "<input type='file' name='image' accept='.bin'><br>"
        "<button type='submit' class='btn'>Update LCD Firmware</button>"
        "</form>"
        "</div>"
        "<div class='section'>"
        "<h1>SPIFFS Update</h1>"
        "<p class='section-description'>Upload your spiffs.bin file to update the SPIFFS filesystem</p>"
        "<form method='POST' action='/update' enctype='multipart/form-data' onsubmit='return showUpdateMessage(this);'>"
        "<input type='file' name='image' accept='.bin'><br>"
        "<button type='submit' class='btn'>Update SPIFFS</button>"
        "</form>"
        "</div>"
        "<div style='margin-top: 30px; text-align: center;'>"
        "<p>Need help with your Bitaxe? "
        "<a href='https://www.advancedcryptoservices.com/bitaxe-support' "
        "style='color: #21ccab; text-decoration: underline;'>"
        "Visit our Bitaxe support page here"
        "</a></p>"
        "</div>"
        "</div></body></html>"
    );
    setupServer->send(200, "text/html", html);
}

void setupWebServer() {
    if (!setupServer) {
        setupServer = new WebServer(80);
    }
    setupServer->on("/", rootPage);  
    setupServer->on("/submitAddress", HTTP_POST, handleSubmit); 
    setupServer->on("/publicPool200x200.png", HTTP_GET, handleImageRequest);
    setupServer->on("/ota", HTTP_GET, handleOTAPage);
    setupServer->on("/update", HTTP_POST, []() {
        // The actual update is handled by handleOTA
    }, handleOTA);
    setupServer->begin();
    Serial0.println("Web Server Started");
}
