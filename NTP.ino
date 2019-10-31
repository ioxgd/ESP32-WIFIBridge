bool NTPConfig(uint8_t data) {
  configTime((int8_t)data * 3600, 0, "ntp.ku.ac.th", "fw.eng.ku.ac.th", "time.uni.net.th");

  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x30);
  
  return true;
}

bool NTPGetTime(uint8_t data) {
  struct tm timeinfo;
  memset(&timeinfo, 0, sizeof timeinfo);
  getLocalTime(&timeinfo);

  Serial1.write(0x1F);
  Serial1.write(0xF1);
  Serial1.write(0x31);
  Serial1.write(timeinfo.tm_hour);
  Serial1.write(timeinfo.tm_min);
  Serial1.write(timeinfo.tm_sec);
  Serial1.write(timeinfo.tm_mday);
  Serial1.write(timeinfo.tm_mon);
  Serial1.write(timeinfo.tm_year);
  Serial1.write(timeinfo.tm_wday);
  Serial1.write(timeinfo.tm_yday);
  
  return true;
}
