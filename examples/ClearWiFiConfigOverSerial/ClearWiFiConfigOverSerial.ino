/*
  示例：ClearWiFiConfigOverSerial
  功能：通过串口输入命令 "clear" 清除 EEPROM 中保存的 WiFi 配置数据，用于恢复出厂状态。
*/

#include "WiFiCaptive.h"
WiFiCaptive captive;

void setup() {
  Serial.begin(115200);
  captive.begin();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "clear") {
      captive.clearWiFiConfig();
      Serial.println("配置已清除，设备将重启...");
      delay(2000);
      ESP.restart();
    }
  }
  captive.loop();
}
