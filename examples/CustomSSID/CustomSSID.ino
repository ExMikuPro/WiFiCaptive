/*
  示例：CustomSSID
  功能：指定自定义的热点名称（无密码），当设备无法连接 WiFi 时开启该 AP 并进入配网页面。
*/

#include "WiFiCaptive.h"
WiFiCaptive captive;

void setup() {
  captive.begin("MyCaptiveAP");
}

void loop() {
  captive.loop();
}