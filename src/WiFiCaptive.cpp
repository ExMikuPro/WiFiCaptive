#include "WiFiCaptive.h"
#include <EEPROM.h>

WiFiCaptive::WiFiCaptive(uint8_t size) : server(80), eepromSize(size) {}

void WiFiCaptive::begin() {
  EEPROM.begin(eepromSize);
  soft_AP_SSID = "ESP-Setup";
  soft_AP_PSK = "12345678";
  tryConnectWiFi();
}

void WiFiCaptive::begin(const String &ssid) {
  EEPROM.begin(eepromSize);
  soft_AP_SSID = ssid;
  soft_AP_PSK = "";
  tryConnectWiFi();
}

void WiFiCaptive::begin(const String &ssid, const String &password) {
  EEPROM.begin(eepromSize);
  soft_AP_SSID = ssid;
  soft_AP_PSK = password;
  tryConnectWiFi();
}

void WiFiCaptive::loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}

void WiFiCaptive::clearWiFiConfig() {
  EEPROM.begin(eepromSize);
  for (int i = 0; i < 64; i++) EEPROM.write(i, 0);
  EEPROM.commit();
  Serial.println("WiFi 配置已清除");
}

void WiFiCaptive::tryConnectWiFi() {
  String ssid, pass;
  readWiFiConfig(ssid, pass);

  if (ssid.length()) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    startCaptivePortal();
  } else {
    Serial.println(WiFi.localIP());
  }
}

void WiFiCaptive::startCaptivePortal() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(soft_AP_SSID, soft_AP_PSK);
  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/", HTTP_GET, [this]() {
      server.send(200, "text/html", generateConfigPage());
  });

  server.on("/connect", HTTP_POST, [this]() {
      handleConnect();
  });

  server.on("/generate_204", []() {
      ESP8266WebServer().sendHeader("Location", "/", true);
      ESP8266WebServer().send(302, "text/plain", "");
  });
  server.on("/hotspot-detect.html", [this]() {
      server.send(200, "text/html", generateConfigPage());
  });
  server.on("/ncsi.txt", []() {
      ESP8266WebServer().send(200, "text/plain", "Microsoft NCSI");
  });

  server.begin();
}

String WiFiCaptive::generateWiFiOptions() {
  int n = WiFi.scanNetworks();
  String options = "";
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    options += "<option value='" + ssid + "'>" + ssid + "</option>";
  }
  return options;
}

String WiFiCaptive::generateConfigPage() {
  String options = generateWiFiOptions();

  if (customHtml.length()) {
    String result = customHtml;
    result.replace("{options}", options);
    return result;
  }

  return
      "<!DOCTYPE html><html lang='zh-CN'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
      "<title>WiFi 配网</title><style>body{display:flex;justify-content:center;align-items:center;flex-direction:column;min-height:100vh;margin:0;font-family:sans-serif;background-color:#f4f4f4}.container{background:#fff;padding:20px;border-radius:12px;box-shadow:0 2px 10px rgba(0,0,0,0.1);width:90%;max-width:400px;text-align:center}select,input[type='text'],input[type='password'],input[type='submit']{width:100%;padding:10px;margin-top:10px;border-radius:6px;border:1px solid #ccc;box-sizing:border-box;font-size:16px;outline:none;transition:border-color 0.3s,box-shadow 0.3s}input[type='password']:focus,input[type='text']:focus,select:focus{border-color:#4CAF50;box-shadow:0 0 4px rgba(76,175,80,0.5)}input[type='submit']{background-color:#4CAF50;color:white;border:none;cursor:pointer}input[type='submit']:hover{background-color:#45a049}</style></head><body><div class='container'><h2>WiFi 配置</h2><form method='POST' action='/connect'><select onchange=\"document.getElementById('ssidInput').value=this.value\">" +
      options +
      "</select><input type='text' id='ssidInput' name='ssid' placeholder='或手动输入WiFi名称'><input type='password' name='password' placeholder='密码'><input type='submit' value='保存并连接'></form></div></body></html>";
}

void WiFiCaptive::handleConnect() {
  if (!server.hasArg("ssid") || !server.hasArg("password")) {
    server.send(400, "text/html", "<meta charset='UTF-8'><b>缺少参数</b>");
    return;
  }

  String ssid = server.arg("ssid");
  String pass = server.arg("password");

  EEPROM.begin(eepromSize);
  for (int i = 0; i < 32; i++) {
    EEPROM.write(SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
    EEPROM.write(PASS_ADDR + i, i < pass.length() ? pass[i] : 0);
  }
  EEPROM.commit();

  server.send(200, "text/html", "<meta charset='UTF-8'><b>WiFi信息已保存，正在重启连接...</b>");
  delay(1500);
  ESP.restart();
}

void WiFiCaptive::readWiFiConfig(String &ssid, String &pass) {
  char buf[33];
  for (int i = 0; i < 32; i++) buf[i] = EEPROM.read(SSID_ADDR + i);
  buf[32] = 0;
  ssid = String(buf);
  for (int i = 0; i < 32; i++) buf[i] = EEPROM.read(PASS_ADDR + i);
  buf[32] = 0;
  pass = String(buf);
}

void WiFiCaptive::setCustomHtml(const String &html) {
  customHtml = html;
}
