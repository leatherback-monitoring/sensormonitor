

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
  Wire.write(data, len);
  /*for (int i = 0; i++; i < len) {
    Wire.write(data[i]);
    //delay(1);
  }*/
  
  uint8_t txStatus;
  txStatus = Wire.endTransmission();

  Serial.print("txStatus: ");
  Serial.println(txStatus);
  
  for (uint8_t i=50; i; --i) {
            delayMicroseconds(500);
            Wire.beginTransmission(EEPROM_ADDR);
            // Try to write to address 0, and see if it is successful
            Wire.write(0);
            Wire.write(0);
            txStatus = Wire.endTransmission();
            if (txStatus == 0) break;
        }
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
  char empty[BLOCK_SIZE];
  for (int i=0; i<BLOCK_SIZE; i++) empty[i] = 0xff;
  Serial.print("Erasing");
  
  for (int i=HEADER_LEN/BLOCK_SIZE; i<200; i++) { // 500=512000/8/128
    writeEEPROMBlock(i*BLOCK_SIZE, empty, BLOCK_SIZE);
    if (i%50==0) Serial.print(".");
    delay(1);
  }/*/
  for (int i=HEADER_LEN; i<64000; i++) { // 6400=500*128
    writeEEPROM(i, 0xff);
    if (i%50==0) Serial.print(".");
  }
  Serial.println("Done");*/
}
