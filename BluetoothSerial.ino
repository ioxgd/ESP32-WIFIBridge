#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

bool BTSerialBegin(uint8_t data) {
  bool res = false;
  static int state = 0;
  static uint8_t nameSize = 0;
  static String btName = "";

  if (state == 0) {
    nameSize = data;
    state = 1;
  } else if (state == 1) {
    btName += (char)data;
    nameSize--;
    if (nameSize == 0) {
      SerialBT.begin(btName);
    
      Serial1.write(0x1F);
      Serial1.write(0xF1);
      Serial1.write(0x40); // Bluetooth Serial begin response
  
      state = 0;
      res = true;
    }
  }

  return res;
}

bool BTSerialAvailable(uint8_t data) {
  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x41); // Bluetooth Serial available response
  Serial1.write((uint8_t)(min(SerialBT.available(), 0xFF)));
    
  return true;
}

bool BTSerialRead(uint8_t data) {
  uint8_t readSize = data;

  uint8_t dataBuffer[readSize];
  SerialBT.readBytes(dataBuffer, readSize);
  
  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x42); // Bluetooth Serial read response
  Serial1.write(dataBuffer, readSize);

  return true;
}

bool BTSerialWrite(uint8_t data) {
  bool res = false;
  static int state = 0;
  static uint8_t writeSize = 0;

  if (state == 0) {
    writeSize = data;
    state = 1;
  } else if (state == 1) {
    SerialBT.write(data);
    writeSize--;
    if (writeSize == 0) {
      Serial1.write(0x1F);
      Serial1.write(0xF1);
      Serial1.write(0x43); // Bluetooth Serial write response

      state = 0;
      res = true;
    }
  }

  return res;
}
