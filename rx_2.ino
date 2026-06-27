#include <WiFi.h>
#include <esp_now.h>

const char* ssid = "ESP32_Transmitter";
const char* password = "12345678";
const int ledPin = 32;

int rx1RSSI = -100;
int rx2RSSI = -100;

uint8_t rx1Mac[] = {0x00, 0x70, 0x07, 0xE3, 0x15, 0xFC};

typedef struct struct_message {
  int rssi;
} struct_message;

struct_message myData;

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  struct_message received;
  memcpy(&received, incomingData, sizeof(received));
  rx1RSSI = received.rssi;
  Serial.print("Received RX1 RSSI: ");
  Serial.println(rx1RSSI);
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
  memcpy(peerInfo.peer_addr, rx1Mac, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  WiFi.begin(ssid, password);
  Serial.println("RX2 Connecting...");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nRX2 Connected!");
  } else {
    Serial.println("\nRX2 Failed to connect!");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, LOW);
    Serial.println("RX2 DISCONNECTED");
    delay(1000);
    return;
  }

  rx2RSSI = WiFi.RSSI();
  Serial.print("RX2 RSSI: ");
  Serial.println(rx2RSSI);

  myData.rssi = rx2RSSI;
  esp_now_send(rx1Mac, (uint8_t *)&myData, sizeof(myData));

  if (rx2RSSI > rx1RSSI) {
    digitalWrite(ledPin, HIGH);
    Serial.println("RX2 STRONGEST - LED ON");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("RX2 WEAK - LED OFF");
  }

  delay(1000);
}
