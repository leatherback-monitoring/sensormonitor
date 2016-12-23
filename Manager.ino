void readSensor() {

  DataPoint reading;
  Serial.print(millis());
  Serial.println("Reading sensor");
  Serial.flush();
  reading.time = millis();
  reading.humidity = myHumidity.readHumidity();
  reading.temperature = myHumidity.readTemperature();


  Serial.print(millis());
  Serial.println("doner");
  Serial.flush();
  
  Serial.print("r: ");
  Serial.print(millis());
  Serial.print(", ");
  Serial.print(reading.temperature, 1);
  Serial.print(",");
  Serial.print(reading.humidity, 1);
  Serial.println();



  // Store data
  int datasize = sizeof(reading);
  //Serial.println(datasize);
  char b[datasize];
  memcpy(b, &reading, datasize);

  Serial.print(millis());
  Serial.println("writing memory");
  Serial.flush();
  //writeEEPROMBlock(readingCount * datasize, b, datasize);
  for (int i = 0; i < datasize; i++) {
    writeEEPROM(readingCount * datasize + i, b[i]);
  }
  
  Serial.print(millis());
  Serial.println("donew");
  Serial.flush();
  readingCount++;
}

boolean displayReading(int reading) {

  DataPoint tmp; //Re-make the struct
  int datasize = sizeof(tmp);
  char b[datasize];

  readEEPROMBlock(reading * datasize, b, datasize);
  /*  for (int i = 0; i < datasize; i++) {
      b[i] = readEEPROM(reading * datasize + i);
    }*/

  memcpy(&tmp, b, datasize);
  Serial.print(tmp.time);
  Serial.print(",");
  Serial.print(tmp.temperature);
  Serial.print(",");
  Serial.print(tmp.humidity);

  Serial.println();
  if (tmp.time > 4294967294) return false;
  else return true;
}

boolean displayReadings(int reading, int count) {

  DataPoint tmp[count]; //Re-make the struct
  int datasize = sizeof(tmp);
  char b[datasize];

  readEEPROMBlock(reading * datasize, b, datasize);
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
