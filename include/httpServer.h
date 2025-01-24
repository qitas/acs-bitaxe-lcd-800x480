#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

extern WebServer* setupServer;
extern String setupBTCAddress;

extern void setupWebServer();
extern void handleRoot();
extern void handleSubmit();

