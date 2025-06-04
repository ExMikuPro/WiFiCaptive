/*
  示例：CustomSSIDAndPassword
  功能：指定自定义的热点名称和密码，启动带加密的 AP 并提供 WiFi 配置入口。
*/

#include <WiFiCaptive.h>
WiFiCaptive captive;

void setup() {
  captive.begin("SecureAP", "mypassword");
}

void loop() {
  captive.loop();
}