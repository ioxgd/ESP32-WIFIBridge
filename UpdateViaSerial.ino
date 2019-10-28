
bool UpdateViaSerial(uint8_t data) {
  bool res = false;

  static uint8_t state = 0;
  static uint32_t updateSize = 0;

  if (state == 0) {
    updateSize = data << 16;
    state = 1;
  } else if (state == 1) {
    updateSize |= data << 8;
    state = 2;
  } else if (state == 2) {
    updateSize |= data;

    Serial1.write(0x1F);
    Serial1.write(0xF1);
    Serial1.write(0xFF); // Update via Serial response
    if (Update.begin(updateSize, U_FLASH, WIFI_LED_PIN, HIGH)) {
      Serial1.write(1);

      uint8_t respondFlag = 0;

      while(Serial1.available() == 0) delay(10);

      size_t written = Update.writeStream(Serial1);
      if (written == updateSize) {
        Serial.println("Written : " + String(written) + " successfully");
        respondFlag |= 1 << 0;
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
        respondFlag |= 0 << 0;
      }
      if (Update.end()) {
        respondFlag |= 1 << 1;
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          respondFlag |= 1 << 2;
          Serial.println("Update successfully completed. Rebooting.");
        }
        else {
          respondFlag |= 0 << 2;
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        respondFlag |= 0 << 1;
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }

      Serial1.write(0x1F);
      Serial1.write(0xF1);
      Serial1.write(0xFF); // Update via Serial response
      Serial1.write(respondFlag);
    } else {
      Serial1.write((uint8_t)0);
    }
    state = 0;
    res = true;
  }

  return res;
}
