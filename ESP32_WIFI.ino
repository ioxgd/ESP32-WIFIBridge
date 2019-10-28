#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include "time.h"

#define WIFI_LED_PIN 2

uint8_t WiFiStatus = 0;
void LEDWiFi_Task(void *p){
  pinMode(WIFI_LED_PIN, OUTPUT);
  
  while(1) {
    if (WiFiStatus == 0) {
      digitalWrite(WIFI_LED_PIN, LOW);
    } else if (WiFiStatus == 1) {
      digitalWrite(WIFI_LED_PIN, !digitalRead(WIFI_LED_PIN));
    } else if (WiFiStatus == 2) {
      digitalWrite(WIFI_LED_PIN, HIGH);
    }
    delay(100);
  }
}

void setup() {

  Serial.begin(115200);
  Serial.println("Start !");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    WiFiStatus = 1;
  }, WiFiEvent_t::SYSTEM_EVENT_STA_START);

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    WiFiStatus = 2;
  }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
  
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    WiFiStatus = 1;
  }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

  xTaskCreate(&LEDWiFi_Task, "LEDWiFi_Task", 1024, NULL, 10, NULL);

  delay(100);

  Serial1.begin(512000, SERIAL_8N1, 25, 27);
  Serial1.setTimeout(200);
}

uint8_t state = 0;
uint8_t command = 0;

void loop() {
  if (Serial1.available()) {
    uint8_t c = Serial1.read();
    Serial.print("0x");
    Serial.print(c, HEX);
    Serial.print("   ");

    if (state == 0) {
      if (c == 0x1F) {
        state = 1;
      }
    } else if (state == 1) {
      if (c == 0xF1) {
        state = 2;
      } else {
        state = 0;
      }
    } else if (state == 2) {
      command = c;
      state = 3;
    } else if (state == 3) {
      if (CommandProcess(command, c)) {
        state = 0;
        Serial.println();
      }
    }
  }
  delay(0);
}
