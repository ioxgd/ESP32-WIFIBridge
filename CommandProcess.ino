bool CommandProcess(uint8_t cmd, uint8_t data) {
  bool res = false;
  switch (cmd) {
    case 0x00: // Test
      res = TestModule(data);
      break;
    case 0x01: // WiFi Scan
      res = WiFiScan(data);
      break;
    case 0x02: // WiFi Connect
      res = WiFiConnect(data);
      break;
    case 0x03: // Check Connect
      res = CheckConnect(data);
      break;
    case 0x04: // Get Local IP
      res = CheckIP(data);
      break;
      
    case 0x10: // Connect to Server
      res = TCPConnect(data);
      break;
    case 0x11: // TCP Connected
      res = TCPConnected(data);
      break;
    case 0x12: // TCP stop
      res = TCPStop(data);
      break;
    case 0x13: // TCP available
      res = TCPAvailable(data);
      break;
    case 0x14: // TCP read
      res = TCPRead(data);
      break;
    case 0x15: // TCP write
      res = TCPWrite(data);
      break;

    case 0x16: // UDP begin
      res = UDPBegin(data);
      break;
    case 0x17: // UDP Send a packet
      res = UDPSend(data);
      break;
    case 0x18: // UDP Receive
      res = UDPReceive(data);
      break;
/*
    case 0x17: // UDP Begin Packet
      res = UDPBeginPacket(data);
      break;
    case 0x18: // UDP End Packet
      res = UDPEndPacket(data);
      break;
    case 0x19: // UDP Write
      res = UDPWrite(data);
      break;
    case 0x1A: // UDP Parse Packet
      res = UDPParsePacket(data);
      break;
    case 0x1B: // UDP Available
      res = UDPAvailable(data);
      break;
    case 0x1C: // UDP read
      res = UDPRead(data);
      break;
    */

    case 0x20: // HTTP Request
      res = HTTPRequest(data);
      break;

    case 0x30: // NTP Config
      res = NTPConfig(data);
      break;
    case 0x31: // NTP Get Time
      res = NTPGetTime(data);
      break;

    case 0x40: // Bluetooth Serial begin
      res = BTSerialBegin(data);
      break;
    case 0x41: // Bluetooth Serial available
      res = BTSerialAvailable(data);
      break;
    case 0x42: // Bluetooth Serial read
      res = BTSerialRead(data);
      break;
    case 0x43: // Bluetooth Serial write
      res = BTSerialWrite(data);
      break;

    case 0xFF: // Update via Serial
      res = UpdateViaSerial(data);
      break;

    default: // unknow command
      res = true;
      
  }
  return res;
}

bool TestModule(uint8_t data) {
  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x01);

  return true;
}
