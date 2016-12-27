

void writeEEPROM(unsigned int eeaddress, byte data ) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(5);
}

void writeEEPROMBlock(unsigned int eeaddress, char *data, int len) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  for (int i = 0; i++; i < len) {
    Wire.write(data[i]);
    delay(1);
  }
  Wire.endTransmission();
  delay(5);
}

byte readEEPROM(unsigned int eeaddress ) {
  byte rdata = 0xFF;

  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_ADDR, 1);

  if (Wire.available()) rdata = Wire.read();

  return rdata;
}

void readEEPROMBlock(unsigned int eeaddress, char *data, int len) {

  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_ADDR, len);

  int i = 0;
  while (Wire.available()) {
    data[i++] = Wire.read();
    //delayMicroseconds(0);
  }
}

void chip_erase() {
  char empty[128];
  for (int i=0; i<128; i++) empty[i] = 0xff;
  Serial.print("Erasing");
  /*
  for (int i=0; i<500; i++) { // 500=512000/8/128
    writeEEPROMBlock(i*128, empty, 128);
    if (i%50==0) Serial.print(".");
  }*/
  for (int i=0; i<64000; i++) { // 6400=500*128
    writeEEPROM(i, 0xff);
    if (i%50==0) Serial.print(".");
  }
  Serial.println("Done");
}
