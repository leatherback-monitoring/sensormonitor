int EEPROM_address(long count) {
  DataPoint tmp;
  int datasize = sizeof(tmp);
  return count * datasize + HEADER_LEN;
}

unsigned int find_last_point() {
  Serial.println(F("Finding start EEPROM pos"));
  unsigned long count = 0;
  while (readEEPROM(EEPROM_address(count)) != 0xff && count <= MAXCOUNT) {
    if (count % 50 == 0) Serial.print(".");
    digitalWrite(LED1, !digitalRead(LED1));
    count++;
  }
  
  digitalWrite(LED1, LOW);
  Serial.println();
  Serial.print(F("Measurement number: "));
  Serial.print(count);
  Serial.print(F(", Offset = "));
  Serial.print(EEPROM_address(count));
  Serial.print(F(" bytes"));
  Serial.println();
  return count;
}

DataPoint read_sensor() {
  DataPoint reading;
  Serial.print(millis());
  Serial.println("Reading sensor");
  Serial.flush();
  reading.time = time_elapsed;
  reading.humidity = myHumidity.readHumidity();
  reading.temperature = myHumidity.readTemperature();


  Serial.print(millis());
  Serial.println("doner");
  Serial.flush();

  Serial.print("r: ");
  Serial.print(reading.time);
  Serial.print(", ");
  Serial.print(reading.temperature, 1);
  Serial.print(",");
  Serial.print(reading.humidity, 1);
  Serial.println();
  return reading;
}

void write_point(DataPoint reading, long reading_count) {
  // Store data
  int datasize = sizeof(reading);
  char b[datasize];
  memcpy(b, &reading, datasize);

  Serial.println("writing memory");
  //writeEEPROMBlock(readingCount * datasize, b, datasize);
  for (int i = 0; i < datasize; i++) {
    writeEEPROM(EEPROM_address(reading_count) + i, b[i]);
  }
  Serial.println("donew");
}

boolean display_reading(long reading) {

  DataPoint tmp; //Re-make the struct
  int datasize = sizeof(tmp);
  char b[datasize];


  /*  for (int i = 0; i < datasize; i++) {
      b[i] = readEEPROM(reading * datasize + i);
    }*/

  readEEPROMBlock(EEPROM_address(reading), b, datasize);


  memcpy(&tmp, &b, datasize);
  Serial.print(tmp.time);
  Serial.print(",");
  Serial.print(tmp.temperature);
  Serial.print(",");
  Serial.print(tmp.humidity);
  Serial.println();

  if (tmp.time >= 0xffffffff) return false;
  else return true;
}

boolean display_readings(long reading, int count) {

  DataPoint tmp[count]; //Re-make the struct
  int datasize = sizeof(tmp);
  char b[datasize];

  readEEPROMBlock(EEPROM_address(reading), b, datasize);
  /*  for (int i = 0; i < datasize; i++) {
      b[i] = readEEPROM(reading * datasize + i);
    }*/

  memcpy(&tmp, b, datasize);
  for (int i = 0; i < count; i++) {
    Serial.print(tmp[i].time);
    Serial.print(",");
    Serial.print(tmp[i].temperature);
    Serial.print(",");
    Serial.print(tmp[i].humidity);
    Serial.println();
  }

  if (tmp[count].time > 4294967294) return false;
  else return true;
}
