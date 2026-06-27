#include <WiFi.h>
#include <esp_now.h>

const char* ssid = "ESP32_Transmitter";
const char* password = "12345678";
const int ledPin = 32;

int rx1RSSI = -100;
int rx2RSSI = -100;

uint8_t rx2Mac[] = {0x30, 0x76, 0xF5, 0xF7, 0xCA, 0x94};

typedef struct struct_message {
  int rssi;
} struct_message;

struct_message myData;

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  struct_message received;
  memcpy(&received, incomingData, sizeof(received));
  rx2RSSI = received.rssi;
  Serial.print("Received RX2 RSSI: ");
  Serial.println(rx2RSSI);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.mode(WIFI_AP_STA);
  delay(500);

  esp_now_init();
  esp_now_register_recv_cb(onDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, rx2Mac, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  WiFi.begin(ssid, password);
  Serial.println("RX1 Connecting...");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nRX1 Connected!");
  } else {
    Serial.println("\nRX1 Failed to connect!");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, LOW);
    Serial.println("RX1 DISCONNECTED");
    delay(1000);
    return;
  }

  rx1RSSI = WiFi.RSSI();
  Serial.print("RX1 RSSI: ");
  Serial.println(rx1RSSI);

  myData.rssi = rx1RSSI;
  esp_now_send(rx2Mac, (uint8_t *)&myData, sizeof(myData));

  if (rx1RSSI > rx2RSSI) {
    digitalWrite(ledPin, HIGH);
    Serial.println("RX1 STRONGEST - LED ON");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("RX1 WEAK - LED OFF");
  }

  delay(1000);
}
