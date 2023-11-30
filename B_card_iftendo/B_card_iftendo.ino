#include <esp_now.h>
#include <WiFi.h>

#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SDA_PIN 5
#define SCL_PIN 6

String messageRcv = "Waiting for a connection";
String messageToSend = "";

esp_now_peer_info_t peerInfo;

// fake mac address
uint8_t address[] = {0x00, 0x00, 0x79, 0x06, 0x10, 0x98};



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    if (status == ESP_NOW_SEND_SUCCESS) {
        Serial.println("Success");
    } else {
        Serial.println("Fail");
    }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    char buf[len + 1];
    memcpy(buf, incomingData, len);
    buf[len] = '\0';
    messageRcv = String(buf);
    Serial.print("Message received: ");
    Serial.println(messageRcv);
}


void setup() {

  Serial.begin(115200);
  
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  updateDisplay();

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  
  esp_now_register_recv_cb(OnDataRecv);
}


void loop() {
    messageToSend = "ding message from card B";

    esp_err_t result = esp_now_send(address, (uint8_t *)messageToSend.c_str(), messageToSend.length() + 1);

    if (result == ESP_OK) {
        Serial.println("Sent with success");
    }
    else {
        Serial.println("Error sending the data");
    }

    updateDisplay();
    delay(1000);
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(messageRcv);
  display.display();
}