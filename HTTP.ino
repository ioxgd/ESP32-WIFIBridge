
bool HTTPRequest(uint8_t data) {
  bool res = false;

  static uint8_t state = 0;
  static uint8_t url_len = 0;
  static String url = "";
  static uint8_t method = 0; // 0: GET, 1: PATCH, 2: POST, 3: PUT
  static uint8_t payload_len = 0;
  static String payload = "";
  static uint8_t headerCount = 0;
  static uint8_t headerNameLen = 0;
  static uint8_t headerValueLen = 0;
  static uint8_t headerInx = 0;
  static String headerName[5];
  static String headerValue[5];

  if (state == 0) {
    url_len = data;
    url = "";
    state = 1;
  } else if (state == 1) {
    url += (char)data;
    url_len--;
    if (url_len <= 0) {
      state = 2;
    }
  } else if (state == 2) {
    method = data;
    state = 3;
    // Payload
  } else if (state == 3) {
    payload_len = data;
    payload = "";
    if (payload_len <= 0) {
      state = 5;
    }
  } else if (state == 4) {
    payload += (char)data;
    payload_len--;
    if (payload_len <= 0) {
      state = 5;
    }
    // Add Header
  } else if (state == 5) {
    headerCount = data;
    if (headerCount == 0) {
      state = 20; // go to do state
    } else {
      headerInx = 0;
      state = 6;
    }
  } else if (state == 6) {
    headerNameLen = data;
    headerName[headerInx] = "";
    state = 7;
  } else if (state == 7) {
    headerName[headerInx] += (char)data;
    headerNameLen--;
    if (headerNameLen <= 0) {
      state = 8;
    }
  } else if (state == 8) {
    headerValueLen = data;
    headerValue[headerInx] = "";
    state = 7;
  } else if (state == 9) {
    headerValue[headerInx] += (char)data;
    headerValueLen--;
    if (headerValueLen <= 0) {
      headerInx++;
      if (headerInx >= headerCount) {
        state = 20;
      } else {
        state = 6;
      }
    }
  } else if (state == 20) { // dumy data
    Serial1.write(0x1F);
    Serial1.write(0xF1);
    Serial1.write(0x20); // HTTP Request response

    WiFiClientSecure *client;
    HTTPClient http;

    if (url.startsWith("https")) {
      client = new WiFiClientSecure;
      http.begin(*client, url);
    } else {
      http.begin(url);
    }

    for (int i = 0; i < headerCount; i++) {
      http.addHeader(headerName[i], headerValue[i]);
    }

    uint16_t httpCode;
    if (method == 0) {
      httpCode = http.GET();
    } else if (method == 1) {
      httpCode = http.PATCH(payload);
    } else if (method == 2) {
      httpCode = http.POST(payload);
    } else if (method == 3) {
      httpCode = http.PUT(payload);
    }
    Serial1.write((uint8_t)httpCode >> 8);
    Serial1.write((uint8_t)httpCode & 0xFF);

    //    String rosPayload = http.getString();
    //    uint16_t rosPayload_len = rosPayload.length();
    //    Serial1.write((uint8_t)rosPayload_len >> 8);
    //    Serial1.write((uint8_t)rosPayload_len & 0xFF);
    //    Serial1.print(rosPayload);

    uint16_t rosPayload_len = http.getSize();
//    Serial.print("Payload: ");
//    Serial.println(rosPayload_len);
    if (rosPayload_len == -1) {
      String rosPayload = http.getString();
      rosPayload_len = rosPayload.length();
      Serial1.write((uint8_t)(rosPayload_len >> 8));
      Serial1.write((uint8_t)(rosPayload_len & 0xFF));
      Serial1.print(rosPayload);
    } else {
      Serial1.write((uint8_t)(rosPayload_len >> 8));
      Serial1.write((uint8_t)(rosPayload_len & 0xFF));

      uint8_t buff[128];
      WiFiClient * stream = http.getStreamPtr();

      while (http.connected() && rosPayload_len > 0) {
        size_t size = stream->available();
        if (size > 0) {
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          Serial1.write(buff, c);
          // Serial.print("Start write ");
          // Serial.println(c);
          // while (Serial1.availableForWrite() > 0) delay(0);
          if (rosPayload_len > 0) {
            rosPayload_len -= c;
          }
          // Serial.print("writed ");
          // Serial.println(c);
        }
        delay(1);
      }
    }
    http.end();
    state = 0;
    res = true;
  }

  return res;
}
