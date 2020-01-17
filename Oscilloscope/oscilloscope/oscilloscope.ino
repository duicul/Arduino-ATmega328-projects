#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 
#define MAX_READINGS 20
uint16_t i=0,del_time=100,inc=3;
uint16_t samples=100;
uint16_t settings[10][10],setting_ind=0;
uint16_t readings[100],read_index=0;
boolean read_on=true;

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

      OCR1A = 0x3D08;

      // OCIE1A interrupt flag set
      TIMSK1 |= (1<<OCIE1A);

      // Start counter at 0, not that it would matter much in this case...
      TCNT1 = 0;
    sei();
    // enable interrupts
}

void adc_init(void)
{
    ADMUX = (1<<REFS0);     //select AVCC as reference
    ADCSRA = (1<<ADEN) | 7;  //enable and prescale = 128 (16MHz/128 = 125kHz)
}

uint16_t readAdc(char chan)
{
    ADMUX = (1<<REFS0) | (chan & 0x0f);  //select input and ref
    ADCSRA |= (1<<ADSC);                 //start the conversion
    while (ADCSRA & (1<<ADSC));          //wait for end of conversion
    return ADCW;
}

void UART_init()
{ cli();
  UCSR0B=0;
  UCSR0A=0;
  UCSR0C |=  (1 << UCSZ00) | (1 << UCSZ01);/* Use 8-bit char size */
  
  UBRR0L = BAUD_PRESCALE;      /* Load lower 8-bits of the baud rate */
  UBRR0H = (BAUD_PRESCALE >> 8);   /* Load upper 8-bits*/
  
  UCSR0B |= (1 << RXCIE0)|(1 << RXEN0) | (1 << TXEN0); /* Turn on transmission and reception (1 << RXCIE0)*/
  sei();
}

void USART_Flush(void)
{
  unsigned char dummy;
  while (UCSR0A & (1<<RXC0)) dummy = UDR0;
  //PORTB^=(1<<5);
}

unsigned char UART_RxChar()
{ //USART_Flush();
  while (!(UCSR0A & (1 << RXC0)));/* Wait till data is received */
  PORTB^=(1<<5);
  return(UDR0);    /* Return the byte */
}

void UART_TxChar(char ch)
{ while (! (UCSR0A & (1<<UDRE0)));  /* Wait for empty transmit buffer */
  PORTB^=(1<<5);
  UDR0 = ch ;
}

ISR(USART_RX_vect)//USART_RX_vecta
{ char ReceivedByte;
  ReceivedByte=UDR0;
  UDR0=ReceivedByte;
  PORTB^=(1<<5);
}

ISR (TIMER1_COMPA_vect){
  if(read_on)
    if(read_index>=MAX_READINGS)
      read_on=false;
    else{
      readings[read_index++]=readAdc(0);
       PORTB^=(1<<5);}
 
}

void setup() {
  DDRB|=(1<<5);
  PORTB&=~(1<<5);
  UART_init();
  //UART_TxChar('2');
  DDRC|=(1<<0);
  PORTC&=~(1<<0);
  timer1init();
  
}

void sendint(uint16_t t){
  uint16_t aux = t;
  uint8_t ind=0,i;
  char val[10],c;
  do{
    c=aux%10+'0';
    val[ind++]=c;
    aux/=10;
  }while(aux!=0);
  for(i=0;i<ind;i++)
     UART_TxChar(val[i]);
}

void loop() {
  uint16_t i=0;
  if(!read_on){
    for(i=0;i<read_index;i++){
      //sendint(readings[i]);
      UART_TxChar(readings[i]>>8);
      UART_TxChar(readings[i]);
      //UART_TxChar('\n');
      }
    read_index=0;
    read_on=true;}
    delay(10000);
  /*
  PORTC&=~(1<<0);
  uint16_t readdata=readAdc(0);
  sendint(readdata);
  UART_TxChar('\n');
  delay(1000);*/
  //PORTB^=(1<<5);
  /*PORTB^=(1<<5);
  delay(del_time);
  del_time-=inc;
  inc=(del_time<10||del_time>100)?(-inc):inc;*/
    
}
