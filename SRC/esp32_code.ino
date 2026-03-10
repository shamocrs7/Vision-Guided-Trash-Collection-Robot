#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>

const char* ssid = "B1-New";
const char* password = "Ejust@1234";
#define ARDUINO_I2C_ADDR 0x08 

WebServer server(80);

String currentObject = "NONE";
int trashFlag = 0; 
unsigned long lastHeartbeat = 0;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>MISSION CONTROL</title>";
  html += "<style>body{margin:0;background:#050505;color:#00ff41;font-family:monospace;height:100vh;display:flex;flex-direction:column;justify-content:center;align-items:center;}";
  html += ".box{border:5px solid #1a1a1a;padding:60px;background:rgba(0,255,65,0.05);width:80%;text-align:center;border-radius:20px;}";
  html += ".val{font-size:8vw;font-weight:bold;margin:20px 0;} .label{font-size:2vw;color:#888;}</style>";
  html += "<script>setInterval(() => { location.reload(); }, 800);</script></head><body>";
  html += "<div class='box'><div class='label'>AI RECOGNITION</div><div class='val'>" + currentObject + "</div>";
  html += "<div class='label'>I2C COMMAND</div><div class='val' style='color:" + String(trashFlag == 1 ? "#00ff41" : "#ff3e3e") + "'>";
  html += String(trashFlag == 1 ? "[ 1 : COLLECT ]" : "[ 0 : AVOID ]") + "</div></div></body></html>";
  server.send(200, "text/html", html);
}

void sendI2C(int value) {
  Wire.beginTransmission(ARDUINO_I2C_ADDR);
  Wire.write(value == 1 ? '1' : '0'); 
  Wire.endTransmission();
}

void handlePostData() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> doc;
    deserializeJson(doc, server.arg("plain"));
    String obj = doc["object"] | "none";
    obj.toLowerCase();
    currentObject = obj;
    
    // Explicit 1 only for trash, otherwise always 0
    trashFlag = (currentObject == "trash" || currentObject == "bottle" || currentObject == "1") ? 1 : 0;
    
    sendI2C(trashFlag);
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/set_data", HTTP_POST, handlePostData);
  server.begin();
}

void loop() {
  server.handleClient();
  // Constant Heartbeat: Ensure Arduino always receives a 0 or 1 every 200ms
  if (millis() - lastHeartbeat > 200) {
    sendI2C(trashFlag);
    lastHeartbeat = millis();
  }
}