#include <WiFi.h>
#include <HTTPClient.h>

void setup() {
  
  Serial.begin(115200);
  Serial.println("Start !");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial1.begin(512000, SERIAL_8N1, 25, 27);
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
