#include "httpServer.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include "spiffsDriver.h"
#include "esp_ota_ops.h"

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
    /* // Remove for now. Causes hangs if authentification is not done first.
    if (!setupServer->authenticate("admin", "admin")) {  // Basic authentication
        return setupServer->requestAuthentication();
    }
    */

    if (setupServer->method() != HTTP_POST) {
        setupServer->send(400, "text/plain", "Only POST method allowed");
        return;
    }

    HTTPUpload& upload = setupServer->upload();
    static esp_ota_handle_t otaHandle = 0;
    static const esp_partition_t* otaPartition = nullptr;

    switch (upload.status) {
        case UPLOAD_FILE_START:
            Serial0.printf("Update: %s\n", upload.filename.c_str());
            if (!upload.filename.endsWith(".bin")) {
                setupServer->send(400, "text/plain", "Only .bin files allowed");
                return;
            }

            otaPartition = esp_ota_get_next_update_partition(NULL);
            if (!otaPartition) {
                setupServer->send(500, "text/plain", "OTA Partition not found");
                return;
            }

            if (esp_ota_begin(otaPartition, OTA_SIZE_UNKNOWN, &otaHandle) != ESP_OK) {
                setupServer->send(500, "text/plain", "OTA Begin Failed");
                return;
            }
            break;

        case UPLOAD_FILE_WRITE:
            if (otaHandle) {
                if (esp_ota_write(otaHandle, upload.buf, upload.currentSize) != ESP_OK) {
                    esp_ota_abort(otaHandle);
                    otaHandle = 0;
                    setupServer->send(500, "text/plain", "Flash Write Error");
                    return;
                }
            }
            break;

        case UPLOAD_FILE_END:
            if (otaHandle) {
                if (esp_ota_end(otaHandle) != ESP_OK || 
                    esp_ota_set_boot_partition(otaPartition) != ESP_OK) {
                    setupServer->send(500, "text/plain", "OTA End Failed");
                    return;
                }
                otaHandle = 0;
                setupServer->send(200, "text/plain", "Update Success! Rebooting...");
                delay(1000);
                ESP.restart();
            }
            break;

        case UPLOAD_FILE_ABORTED:
            if (otaHandle) {
                esp_ota_abort(otaHandle);
                otaHandle = 0;
            }
            break;
    }
}

static void handleOTAPage() {
    String html = F(
        "<html><head>"
        "<title>Firmware Update</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; background-color: #111316; color: #00B093; }"
        ".container { max-width: 600px; margin: 40px auto; padding: 20px; }"
        "h2 { color: #00B093; }"
        "input[type='file'] { width: 100%; padding: 12px; margin: 8px 0; "
        "border: 2px solid #01544A; border-radius: 4px; background: #111316; color: #21CCAB; }"
        ".btn { background-color: #006D62; color: white; padding: 14px 20px; margin: 8px 0; "
        "border: none; border-radius: 4px; cursor: pointer; width: 100%; }"
        ".btn:hover { background-color: #45a049; }"
        "</style>"
        "</head><body>"
        "<div class='container'>"
        "<h2>Upload lcdFirmware.bin</h2>"
        "<form method='POST' action='/update' enctype='multipart/form-data'>"
        "<input type='file' name='image' accept='.bin'><br>"
        "<button type='submit' class='btn'>Update Firmware</button>"
        "</form>"
        "</div>"
        "</body></html>"
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
