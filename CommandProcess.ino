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

    case 0x20: // HTTP Request
      res = HTTPRequest(data);
      break;

    case 0x30: // NTP Config
      res = NTPConfig(data);
      break;
    case 0x31: // NTP Get Time
      res = NTPGetTime(data);
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
