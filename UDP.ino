WiFiUDP udp;

bool UDPBegin(uint8_t data) {
  bool res = false;

  static uint8_t state = 0;
  static uint16_t udpPort = 0;

  if (state == 0) {
    udpPort = 0;
    udpPort = data << 8;
    state = 1;
  } else if (state == 1) {
    udpPort |= data;
    udp.begin(udpPort);
    
    Serial1.write(0x1F);
    Serial1.write(0xF1);
    Serial1.write(0x16);
    
    state = 0;
    res = true;
  }

  return res;
}

bool UDPSend(uint8_t data) {
  bool res = false;

  static uint8_t state = 0;
  static uint8_t hostSize = 0;
  static String host = "";
  static uint16_t port = 0;
  static uint8_t dataSize = 0;
  static uint8_t *dataBuffer = NULL;
  static uint8_t dataIndex = 0;

  if (state == 0) {
    hostSize = data;
    host = "";
    state = 1;
  } else if (state == 1) {
    host += (char)data;
    hostSize--;
    if (hostSize == 0) {
      state = 2;
    }
  } else if (state == 2) {
    port = 0;
    port = data << 8;
    state = 3;
  } else if (state == 3) {
    port |= data;
    state = 4;
  } else if (state == 4) {
    dataSize = data;
    dataBuffer = (uint8_t*)malloc(dataSize);
    dataIndex = 0;
    state = 5;
  } else if (state == 5) {
    dataBuffer[dataIndex++] = data;
    if (dataIndex >= dataSize) {
      udp.beginPacket(host.c_str(), port);
      udp.write(dataBuffer, dataSize);
      udp.endPacket();

      free(dataBuffer);
      res = true;

      Serial1.write(0x1F);
      Serial1.write(0xF1);
      Serial1.write(0x17);

      state = 0;
    }
  }

  return res;
}

bool UDPReceive(uint8_t data) {
  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x18);
        
  uint16_t packetSize = udp.parsePacket();
  Serial1.write((uint8_t)(packetSize >> 8));
  Serial1.write((uint8_t)(packetSize & 0xFF));

  if (packetSize > 0) {
    IPAddress remote = udp.remoteIP();
    Serial1.write(remote[0]);
    Serial1.write(remote[1]);
    Serial1.write(remote[2]);
    Serial1.write(remote[3]);
    
    uint16_t port = udp.remotePort();
    Serial1.write((uint8_t)(port >> 8));
    Serial1.write((uint8_t)(port & 0xFF));
  
    uint8_t *dataBuffer = (uint8_t*)malloc(packetSize);
    udp.read(dataBuffer, packetSize);
    Serial1.write(dataBuffer, packetSize);
  }

  return true;
}
