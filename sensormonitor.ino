
// Copyright Erik Meike 2016
// Released under MIT licence
// https://github.com/leatherback-monitoring/sensormonitor
// Donations 15ASUWToUKy9PuJSPfkz54zYnBrtqC565i


#define EEPROM_ADDR 0x50    //Address of 24LC256 eeprom chip
#define DEBUG
#define LED1 13

#define EEPROM_SIZE 128000  // Size in bytes
#define MAXCOUNT (EEPROM_SIZE-HEADER_LEN)/sizeof(DataPoint)


#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

#include <Wire.h>
#include "SparkFunHTU21D.h"

char magic[6] = "turtle";

#define BLOCK_SIZE 256
#define HEADER_LEN 1*BLOCK_SIZE // must be a multiple of BLOCK_SIZE

struct EEPROMHeader {
  byte magic[6];
  unsigned long id;
  unsigned int hversion;
  unsigned long sversion;
  unsigned int batch;
};  // length must be less than BLOCK_SIZE bytes

struct DataPoint {
  unsigned long time;
  float temperature;
  float humidity;
  float filler;
};  // length must be factor of BLOCK_SIZE bytes

struct Header {
  unsigned long id;
};

unsigned long time_elapsed = 0;


HTU21D myHumidity;

void setup(void) {
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  Wire.begin();
  Serial.begin(115200);
  Wire.setClock(400000L);
  //TWBR = 1;//((CPU_FREQ / TWI_FREQ_NUNCHUCK) - 16) / 2;
  myHumidity.begin();
  //digitalWrite(9, !digitalRead(9));
  Serial.println("Starting");
  unsigned int address = 0;
  writeEEPROM(address, 170);
  Serial.print(readEEPROM(address), DEC);
  Serial.println("RUNNING");

  Serial.println("Waiting for computer connection...");
  while (millis() < 10000) handle_serial();
  Serial.println("Continuing");

  initTCNT2();
}


void loop() {
  unsigned int start_count = find_last_point();

  // start overwriting data if at end of memory
  if (start_count >= MAXCOUNT) start_count = 0;

#ifdef DEBUG
  Serial.print("Starting at: ");
  Serial.println(start_count);
#endif

  unsigned int target_time = time_elapsed;
  for (unsigned int reading_count = start_count; reading_count <= MAXCOUNT; reading_count++) {
#ifdef DEBUG
    Serial.flush();
#endif

    digitalWrite(LED1, LOW);

    // Sleep and break from loop if recieved serial input
    if (!sleep_timer2_count(target_time)) break;
    target_time += 4; // 75=10 minutes  // 225=30*60/8

#ifdef DEBUG
    Serial.println("A");
#endif
    digitalWrite(LED1, HIGH);
    DataPoint reading = read_sensor();
    write_point(reading, reading_count);
    

#ifdef DEBUG
    Serial.println("S");
#endif
    //delay(500);
  }
}

void handle_serial() {

  while (Serial.available()) {
    unsigned int i = 0;
    switch (Serial.read()) {
      case 'e':
        chip_erase();
        break;
      case 'r':
        while (display_reading(i) && i < MAXCOUNT) i++;
        //while (display_readings(i,2) && i < MAXCOUNT) i+=2;
        break;
      case 't':

  byte block[BLOCK_SIZE];
  for (int i=0; i<BLOCK_SIZE; i++) block[i] = i;
  byte block2[BLOCK_SIZE];
 Serial.println("TESTING");
  
    writeEEPROMBlock(0, block, 128);
    readEEPROMBlock(0, block2, 128);
  for (int i=0; i<BLOCK_SIZE; i++) {
    Serial.print(i);
    Serial.print(", ");
    Serial.println(block2[i]);
}
      default:
        break;
    }
  }
}

