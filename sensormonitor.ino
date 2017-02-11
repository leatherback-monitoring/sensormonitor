#define EEPROM_ADDR 0x50    //Address of 24LC256 eeprom chip
#define LED1 13
#define BLOCK_SIZE 128
#define HEADER_BLOCKS 1
#define HEADER_LEN HEADER_BLOCKS * BLOCK_SIZE

#include <Wire.h>
#include "SparkFunHTU21D.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

struct DataPoint {
  unsigned long time;
  float temperature;
  float humidity;
  float filler;
};  // length must be factor of BLOCK_SIZE

struct Header {
  unsigned long id;
};

unsigned long time_elapsed = 0;

unsigned long readingCount = 0;

#define MAXCOUNT 4000
HTU21D myHumidity;
unsigned int startcount = 0;

void setup(void) {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(115200);
  Wire.begin();
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
  while (millis() < 5000) handle_serial();
  Serial.println("Continuing");

  init_TCNT2();
}

unsigned long target_time = 0;
void loop() {
  find_last_point();

  // start overwriting data if at end of memory
  if (startcount >= MAXCOUNT) startcount = 0;

  Serial.print("Starting at: ");
  Serial.println(startcount);
  for (int i = startcount; i < MAXCOUNT; i++) {
    Serial.flush();
    digitalWrite(13, LOW);

    // Sleep and break from loop if recieved serial input
    if (!sleep_timer2_count(target_time)) break;

    target_time += 20; // 75=10 minutes  // 225=30*60/8

    Serial.println("A");
    digitalWrite(13, HIGH);
    read_sensor();

    Serial.println("S");
    //delay(500);
  }
}

void handle_serial() {
  long i = 0;
  while (Serial.available()) {
    switch (Serial.read()) {
      case 'e':
        chip_erase();
        break;
      case 'r':
        while (display_reading(i) && i < MAXCOUNT) i++;
        //while (displayReadings(i,2) && i < MAXCOUNT) i+=2;
        break;
      default:
        break;
    }
  }
}

