// Defines for setting and clearing register bits.
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup() {                
  // Initialize the digital pins as outputs.
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);    
}

void loop() {
  // This part is to calibrate the fastest prescaler.
  for ( int i = 1; i < 50; i++ )
  {
    sbi(PORTB,PORTB4);    // Set the LED on.
    delayMicroseconds(i); // Wait for i microseconds.
    cbi(PORTB,PORTB4);    // Set the LED off.
    delayMicroseconds(10);// Wait for 10 microseconds.
  }
  
  // This part is to calibrate all the others.
  for ( int i = 1; i < 10; i++ )
  {
    sbi(PORTB,PORTB5);    // Set the LED on.
    delay(i);             // Wait for i milliseconds.
    cbi(PORTB,PORTB5);    // Set the LED off.
    delay(1);             // Wait for a millisecond.
  }
  
  delay(100);             // Wait for 100 milliseconds.
}
