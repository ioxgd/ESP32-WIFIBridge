WiFiClient client[3];

bool TCPConnect(uint8_t data) {
  bool res = false;

  static uint8_t state = 0;
  static uint8_t socket_id = 0;
  static uint8_t host_type = 0;
  static uint8_t host_len = 0;
  static uint8_t host_inx = 0;
  static uint8_t host[64];
  static uint16_t port = 80;

  if (state == 0) {
    socket_id = (data > 2 ? 2 : data);
    state = 1;
  } else if (state == 1) {
    host_type = data;
    state = 2;
  } else if (state == 2) {
    host_len = data;
    memset(host, 0, 64);
    host_inx = 0;
    state = 3;
  } else if (state == 3) {
    host[host_inx++] = data;
    if (host_inx >= host_len) {
      state = 4;
    }
  } else if (state == 4) {
    port = 0;
    port = data << 8;
    state = 5;
  } else if (state == 5) {
    port |= data;
    state = 6;
  } else if (state == 6) {
    Serial1.write(0x1F);
    Serial1.write(0xF1);
    Serial1.write(0x10); // Connect to Server response

    int connectCode = 0x00;
    if (host_type == 1) { // Connect by IP
      connectCode = client[socket_id].connect(IPAddress(host[0], host[1], host[2], host[3]), port);
    } else if (host_type == 2) { // Connect by Hostname
      connectCode = client[socket_id].connect((char*)host, port);
    }
    Serial1.write(connectCode == 1 ? 0x01 : 0x00);

    state = 0;

    res = true;
  }

  return res;
}

bool TCPConnected(uint8_t data) {
  uint8_t socket_id = (data > 2 ? 2 : data);

  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x11); // TCP Connected response
  Serial1.write(client[socket_id].connected() ? 0x01 : 0x0);

  return true;
}

bool TCPStop(uint8_t data) {
  uint8_t socket_id = (data > 2 ? 2 : data);

  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x12); // TCP Stop response
  client[socket_id].stop();

  return true;
}

bool TCPAvailable(uint8_t data) {
  uint8_t socket_id = (data > 2 ? 2 : data);

  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x13); // TCP Available response
  uint16_t size = client[socket_id].available();
  Serial1.write((uint8_t)(size >> 8));
  Serial1.write((uint8_t)(size & 0xFF));

  return true;
}

bool TCPRead(uint8_t data) {
  bool res = false;
  
  static uint8_t state = 0;
  static uint8_t socket_id;
  static uint16_t size = 0;

  if (state == 0) {
    socket_id = (data > 2 ? 2 : data);
    state = 1;
  } else if (state == 1) {
    size = data << 8;
    state = 2;
  } else if (state == 2) {
    size |= data;

    Serial1.write(0x1F);
    Serial1.write(0xF1);
    Serial1.write(0x14); // TCP Read response
    /*
    uint16_t realSize = client[socket_id].available();
    if (size > realSize) {
      size = realSize;
    }
    Serial1.write((uint8_t)(size >> 8));
    Serial1.write((uint8_t)(size & 0xFF));
    */
    for (uint16_t i=0;i<size;i++) {
      Serial1.write(client[socket_id].read());
    }

    state = 0;
    res = true;
  }

  return res;
}

bool TCPWrite(uint8_t data) {
  bool res = false;
  
  static uint8_t state = 0;
  static uint8_t socket_id;
  static uint16_t size = 0;

  if (state == 0) {
    socket_id = (data > 2 ? 2 : data);
    state = 1;
  } else if (state == 1) {
    size = 0;
    size = data << 8;
    state = 2;
  } else if (state == 2) {
    size |= data;
    state = 3;
  } else if (state == 3) {
    client[socket_id].write(data);
    size--;
    if (size == 0) {
      Serial1.write(0x1F);
      Serial1.write(0xF1);
      Serial1.write(0x15); // TCP Write response
    
      state = 0;
      res = true;
    }
  }

  return res;
}
