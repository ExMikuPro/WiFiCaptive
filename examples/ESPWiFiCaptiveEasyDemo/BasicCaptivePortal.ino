/*
  示例：BasicCaptivePortal
  功能：默认使用 EEPROM 中的 WiFi 配置进行连接，若失败则开启名为 "ESP-Setup" 的热点，
        并提供网页供用户配置 WiFi。
*/

#include <WiFiCaptive.h>
WiFiCaptive captive;

void setup() {
  captive.begin(); // 使用默认热点 ESP-Setup
}

void loop() {
  captive.loop();
}