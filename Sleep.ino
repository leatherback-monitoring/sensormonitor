#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>


// time_elapsed is defined in main file

ISR(WDT_vect)
{
}

void low_powerize() {
  DDRD &= B00000011;       // set Arduino pins 2 to 7 as inputs, leaves 0 & 1 (RX & TX) as is
  DDRB = B00000000;        // set pins 8 to 13 as inputs
  PORTD |= B11111100;      // enable pullups on pins 2 to 7, leave pins 0 and 1 alone
  PORTB |= B11111111;      // enable pullups on pins 8 to 13
}

boolean sleep_timer2_count(unsigned long endTime) {
  Serial.print("sleeping...");
  Serial.print(" wake up at ");
  Serial.print(endTime);
  Serial.print(". now is ");
  Serial.print(time_elapsed);
  Serial.println();
  Serial.flush();

  CLKPR = 0b10000100;

  while (time_elapsed < endTime) {
    Serial.println(TCNT2);
    Serial.println("sleeping...");
    
    Serial.flush();
    
    
    // page 39 of manual for sleep mode table
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    
    // disable unused peripherals
    power_adc_disable();
    power_spi_disable();
    power_timer0_disable();
    power_timer1_disable();
    //power_timer2_disable();
    power_twi_disable();
    
    
    cli();
    /*if (TCNT2 == 254) {
      TCNT2 = 0;
      time_elapsed++;
    }*/
    sleep_enable();
    sleep_bod_disable();
    sei();
    
    sleep_cpu();
    
    sleep_disable();
    
    power_timer0_enable();
    power_twi_enable();
    Serial.println("awake");
  }

  CLKPR = 0b10000000;

  //if (Serial.available()) return false;
  return true;
}

ISR(TIMER2_OVF_vect) {
  time_elapsed++;
  // flash LED every 16 seconds
  if (1 || time_elapsed % 2 == 0) digitalWrite(13, HIGH);
  
  Serial.print("Time is ");
  Serial.println(time_elapsed);
  digitalWrite(13, LOW);
  //TIFR2 = 0;
}

/* 
ISR(TIMER2_COMPA_vect) {
  digitalWrite(13, !digitalRead(13));
  Serial.println("compare ");
  //TIFR2 = 0;
  TCNT2 = 0;
  //Serial.print("INTERRUPT!!!\t\t");
  Serial.println(millis());
}*/

void init_TCNT2() {
  // *tries to* initialize timer2 in accoranace with datasheet page 151 for asynchronous operation
  // http://www.atmel.com/images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet_Complete.pdf
  
  // 1 / (1800 seconds) (half hours per second) = crystal input / 1024 (prescaler) / timer counts
  // 1/(32760/1024/57586)
  // should let timer count to 57586 = 1,800.0019536 seconds, meaning an error of 0.00011%, which is accurate enough
  


  // Clear interrupts
  TIMSK2 = (0 << TOIE2) | (0 << OCIE2A) | (0 << OCIE2B);

  // Set clock source
  ASSR &= ~(1 << EXCLK);  // disable arbitrary signal input
  ASSR |= (1 << AS2);     // run timer2 in asynchronous mode, inputting from 32khz crystal
  
  
  // disable pin waveform output
  TCCR2A = 0;
  // stop timer and do not set up waveform generation mode
  TCCR2B = 0;

  // Init counter value
  TCNT2 = 0;
  
  // set output compare registers
  OCR2A = 17;
  OCR2B = 0;


  // check ASSR to ensure writes worked
  Serial.print("ASSR: ");
  Serial.println(ASSR, BIN);
  
  
  // clear interrupt flags by writing 1s
  TIFR2 = (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);
  
  // wait for TCN2xUB OCR2xUB and TCR2xUB
  // be lazy and just wait a while instead of checking
  delay(10);


  // Start counter
  
  // enable timer and set prescaler to /1024
  //TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
  TCCR2B = (0 << CS22) | (1 << CS21) | (1 << CS20); // /256 for debug
  
  // wait for clock to stabilize
  Serial.println("Waiting for clock...");
  delay(1000);
  
  
  Serial.println("enabled interrupts");
  Serial.flush();
  // Start interrupts
  TIMSK2 |= (1 << TOIE2);
  //delay(10000);

  // Stop interrupts
  //TIMSK2 = 0;
  //Serial.println("disabled interrupts");
}
