
bool HTTPRequest(uint8_t data) {
  bool res = false;

  static uint8_t state = 0;
  static uint8_t url_len = 0;
  static String url = "";
  static uint8_t method = 0; // 0: GET, 1: PATCH, 2: POST, 3: PUT
  static uint8_t payload_len = 0;
  static String payload = "";

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
  } else if (state == 5) {
    Serial1.write(0x1F);
    Serial1.write(0xF1);
    Serial1.write(0x20); // HTTP Request response

    HTTPClient http;
    http.begin(url);
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
    String rosPayload = http.getString();
    uint16_t rosPayload_len = rosPayload.length();
    Serial1.write((uint8_t)rosPayload_len >> 8);
    Serial1.write((uint8_t)rosPayload_len & 0xFF);
    Serial1.print(rosPayload);

    state = 0;
    res = true;
  }

  return res;
}
