#pragma once

#include <Arduino.h>

#if defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

using WiFiWebServer = ESP8266WebServer;
#elif defined(ESP32)
#include <WiFi.h>
#include <WebServer.h>
using WiFiWebServer = WebServer;
#else
#error "Unsupported platform"
#endif
#include <DNSServer.h>

class WiFiCaptive {
public:
    explicit WiFiCaptive(uint8_t eepromSize = 96);

    void begin();

    void begin(const String &ssid);

    void begin(const String &ssid, const String &password);

    void loop();

    void clearWiFiConfig();

    void setCustomHtml(const String &html);

private:
    WiFiWebServer server;
    DNSServer dnsServer;
    uint8_t eepromSize;


    const int SSID_ADDR = 0;
    const int PASS_ADDR = 32;

    String customHtml;

    String soft_AP_SSID = "";
    String soft_AP_PSK = "";

    void tryConnectWiFi();

    void startCaptivePortal();

    void handleConnect();

    String generateConfigPage();

    String generateWiFiOptions();

    void readWiFiConfig(String &ssid, String &pass);
};
