
bool WiFiScan(uint8_t data) {
  Serial.println("Start scan");
  uint8_t n = WiFi.scanNetworks();
  Serial.println("scan done");
  Serial.print(n);
  Serial.println(" networks found");

  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x01); // WiFi Scan response
  Serial1.write(n);
  for (int i = 0; i < n; ++i) {
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.print(")");
    Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");

    Serial1.write(WiFi.SSID(i).length());
    Serial1.print(WiFi.SSID(i));
    Serial1.write((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 0 : 1);
  }

  return true;
}

uint8_t WiFiConnect_state = 0;
uint8_t WiFiConnect_ssidLen = 0, WiFiConnect_passLen = 0;
String WiFiConnect_SSID, WiFiConnect_PASSWORD;
bool WiFiConnect(uint8_t data) {
  bool startConnect = false;
  if (WiFiConnect_state == 0) {
    WiFiConnect_ssidLen = data;
    WiFiConnect_SSID = "";
    WiFiConnect_state = 1;
  } else if (WiFiConnect_state == 1) {
    WiFiConnect_SSID += (char)data;
    WiFiConnect_ssidLen--;
    if (WiFiConnect_ssidLen == 0) {
      WiFiConnect_state = 2;
    }
  } else if (WiFiConnect_state == 2) {
    WiFiConnect_passLen = data;
    WiFiConnect_PASSWORD = "";
    if (WiFiConnect_passLen == 0) {
      startConnect = true;
    } else {
      WiFiConnect_state = 3;
    }
  } else if (WiFiConnect_state == 3) {
    WiFiConnect_PASSWORD += (char)data;
    WiFiConnect_passLen--;
    if (WiFiConnect_passLen == 0) {
      startConnect = true;
    }
  }
  if (startConnect) {
    Serial.print("Connect to ");
    Serial.print(WiFiConnect_SSID);
    if (WiFiConnect_PASSWORD.length() > 0) {
      Serial.write(':');
      Serial.print(WiFiConnect_PASSWORD);
    }
    Serial.println();

    WiFi.disconnect();
    if (WiFiConnect_PASSWORD.length() > 0) {
      WiFi.begin(WiFiConnect_SSID.c_str(), WiFiConnect_PASSWORD.c_str());
    } else {
      WiFi.begin(WiFiConnect_SSID.c_str());
    }

    Serial1.write(0x1F);
    Serial1.write(0xF1);
    Serial1.write(0x02); // WiFi Connect response

    WiFiConnect_state = 0;
  }

  return WiFiConnect_state == 0;
}

bool CheckConnect(uint8_t data) {
  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x03); // Check connect response
  Serial1.write(WiFi.isConnected() ? 0x01 : 0x00);

  return true;
}

bool CheckIP(uint8_t data) {
  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x04); // Get Local IP response
  
  IPAddress ip = WiFi.localIP();
  Serial1.write(ip[0]);
  Serial1.write(ip[1]);
  Serial1.write(ip[2]);
  Serial1.write(ip[3]);
}
