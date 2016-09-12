#include <Wire.h>

#include "SparkFunHTU21D.h"
#define EEPROM_ADDR 0x50    //Address of 24LC256 eeprom chip

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

struct DataPoint {
  unsigned long time;
  float temperature;
  float humidity;
  float filler;
};  // length must be factor of 128

struct Header {
  unsigned long id;
};


unsigned long readingCount = 0;

#define MAXCOUNT 4000
HTU21D myHumidity;
int startcount = 0;

void setup(void) {
  pinMode(9, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000L);
  //TWBR = 1;//((CPU_FREQ / TWI_FREQ_NUNCHUCK) - 16) / 2;
  myHumidity.begin();
  digitalWrite(9, !digitalRead(9));
  Serial.println("Starting");
  unsigned int address = 0;
  writeEEPROM(address, 123);
  Serial.print(readEEPROM(address), DEC);
  Serial.println("RUNNING");
  //initTCNT2();
}

void loop() {
  findLastPoint();
  // start overwriting data if at end of memory
  if (!(startcount < MAXCOUNT)) startcount = 0;
  Serial.println(startcount);
  for (int i = startcount; i < MAXCOUNT; i++) {
    Serial.flush();
    digitalWrite(9, LOW);
    if (!sleepWatchdogCount(millis() + 180000)) break;
    
    Serial.println("A");
    digitalWrite(9, HIGH);
    readSensor();
    
    Serial.println("S");
    //delay(500);
  }
  long i = 0;
  while (Serial.available()) if(Serial.read() == 'e') chip_erase();
  
  while (displayReading(i) && i< MAXCOUNT) i++;
  //while (displayReadings(i,2) && i < MAXCOUNT) i+=2;
  //delay(10000);
}

void findLastPoint() {
  DataPoint tmp;
  int datasize = sizeof(tmp);
  Serial.println(F("Finding start EEPROM pos"));
  while (readEEPROM(startcount * datasize) != 0xff && startcount < MAXCOUNT) {
    if (startcount % 50 == 0) Serial.print("."); 
    startcount++;
  }
  Serial.println();
  Serial.print(F("Measurement number: "));
  Serial.print(startcount);
  Serial.print(F(", Offset = "));
  Serial.print(startcount*datasize);
  Serial.print(F(" bytes"));
  Serial.println();
}
