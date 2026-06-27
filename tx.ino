#include <WiFi.h>

const char* ssid = "ESP32_Transmitter";
const char* password = "12345678";

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password, 1);
  Serial.println("TX Started!");
}

void loop() {
  Serial.print("Connected Devices: ");
  Serial.println(WiFi.softAPgetStationNum());
  delay(1000);
}
