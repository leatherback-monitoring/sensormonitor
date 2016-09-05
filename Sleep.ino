
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

ISR(WDT_vect)
{
}

void lowPowerize() {
  DDRD &= B00000011;       // set Arduino pins 2 to 7 as inputs, leaves 0 & 1 (RX & TX) as is
  DDRB = B00000000;        // set pins 8 to 13 as inputs
  PORTD |= B11111100;      // enable pullups on pins 2 to 7, leave pins 0 and 1 alone
  PORTB |= B11111111;      // enable pullups on pins 8 to 13
}
boolean sleepWatchdogCount(unsigned long endTime) {
  Serial.print("sleeping...");
  Serial.print(" wake up at ");
  Serial.print(endTime);
  Serial.print(". now is ");
  Serial.print(millis());
  Serial.println();
  Serial.flush();
  
  CLKPR = 0b10000100;
  
  while (millis() < endTime) {
    //Serial.println("A");
    set_sleep_mode(SLEEP_MODE_IDLE);
    cli();
    sleep_enable();
    sleep_bod_disable();

    sei();
    sleep_cpu();

    sleep_disable();
    if (Serial.available()) break;
  }
  
  CLKPR = 0b10000000;
  
  if (Serial.available()) return false;
  return true;
}


void initTCNT2() {
  // manual page 151
  
  // 1/(32760/1024/57586)
  
  
  // Clear interrupts
  TIMSK2 = (0<<TOIE2) || (0<<OCIE2A) || (0<<OCIE2B);
  
  // Set clock source
  ASSR |= (0<<AS2);
  TCCR2B = 0;
  
  // Init counter
  TCNT2 = 0;
  
  OCR2A = 100;
  OCR2B = 0;
  
  TCCR2A = 0;
  TCCR2B = 0;
  
  // check ASSR to ensure writes worked
  
  // wait for TCN2xUB OCR2xUB and TCR2xUB
  
  // clear interrupt flags
  delay(10);
  TIFR2 = 0;
  
  // Start counter
  TCCR2B = (1<<CS20);
  ASSR |= (1<<EXCLK);
  
  for (int i=0; i<100; i++) {
    Serial.println(TCNT2);
    delay(100);
  }
  
}
