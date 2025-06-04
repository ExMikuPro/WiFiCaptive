/*
  示例：CustomHTMLTemplate
  功能：设置带有占位符 {options} 的自定义 HTML 页面，替代默认配网页面。
*/

#include "WiFiCaptive.h"
WiFiCaptive captive;

void setup() {
  captive.setCustomHtml(R"rawliteral(
    <html><body>
      <h1>欢迎回来！</h1>
      <form method='POST' action='/connect'>
        <select onchange="document.getElementById('ssidInput').value=this.value">{options}</select>
        <input type='text' id='ssidInput' name='ssid' placeholder='手动输入SSID'>
        <input type='password' name='password' placeholder='密码'>
        <input type='submit' value='连接'>
      </form>
    </body></html>
  )rawliteral");

  captive.begin("CustomAP");
}

void loop() {
  captive.loop();
}