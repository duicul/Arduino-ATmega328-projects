#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
//#define PRESC1 1
//#define PRESC8 2
//#define PRESC64 3
//#define PRESC256 4
#define PRESC1024 5
#define ARRAY_LENGTH 101
#define Total_time 16175
#define Frequency 50

volatile uint8_t pres=5;

float counter_time=1024.0/16000000;
float adjust_fact=((1.0/Frequency/2.0)/Total_time)*1000000;

boolean positive_amp=true;
int x=0;



int sinus[]={0,8,16,23,31,39,47,55,63,71,78,86,93,101,108,115,122,129,
136,143,149,156,162,168,174,180,185,191,196,201,206,210,215,219,223,
227,230,234,237,239,242,244,246,248,250,251,252,253,254,254,255,254,
254,253,252,251,250,248,246,244,242,239,237,234,230,227,223,219,215,
210,206,201,196,191,185,180,174,168,162,156,149,143,136,129,122,115,
108,101,93,86,78,71,63,55,47,39,31,23,16,8,0};

boolean output=false;

float counter_value(uint8_t xval){
  return sinus[xval]*adjust_fact/counter_time;
}

void timer1init(){
    cli();
    TCCR1A &= ~(1<<COM1A1) &   // Clearing bits
            ~(1<<COM1A0) &
            ~(1<<COM1B1) &
            ~(1<<COM1B0) &
            ~(1<<WGM11) &
            ~(1<<WGM10);


     TCCR1B &= ~(1<<ICNC1) &    // Clearing bits
            ~(1<<ICES1) &
            ~(1<<WGM13) &
            ~(1<<CS11);

     TCCR1B |= (1<<WGM12) |    // Setting bits
            (1<<CS12) |
            (1<<CS10);

      OCR1A = 0x3D08;//0x9C;//0xA; 

      // OCIE1A interrupt flag set
      TIMSK1 |= (1<<OCIE1A);

      // Start counter at 0, not that it would matter much in this case...
      TCNT1 = 0;
    sei();
    // enable interrupts
}

void writeconfig(){
  cli();
  TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));//cleaning bytes
  TCCR1B |= (pres&0x7);
  OCR1A = counter_value(x);//timer period
  TIMSK1 |= (1<<OCIE1A);
  TCNT1 = 0;
  sei();
}

ISR (TIMER1_COMPA_vect){
  if(x==ARRAY_LENGTH){
    digitalWrite(9,LOW);
    digitalWrite(10,LOW);
    positive_amp=!positive_amp;
    output=false;
  }

  int out_value=output?HIGH:LOW;
  if(positive_amp)
      digitalWrite(9,out_value);
  else 
      digitalWrite(10,out_value);
  x++;
  writeconfig();
  
}


void setup() {
 pinMode(9,OUTPUT); 
 pinMode(10,OUTPUT);
 timer1init();
 writeconfig();
}

void loop() {
}
