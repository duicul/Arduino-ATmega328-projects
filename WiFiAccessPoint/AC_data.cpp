#include "AC_data.h"
AC_data::AC_data(){
  
  //pin_rec=_pin_rec;
  //pin_send=_pin_send;
  
  
  //IPAddress ip(192,168,1,1);
  //pzem->setAddress(ip);
}

String AC_data::read_data(){
  HardwareSerial PzemSerial2(2);     // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
  PZEM004T pzem(&PzemSerial2);
  int tries = 0;
  IPAddress ip(192,168,1,1);
  while(tries<5){
      Serial.println("Connecting to PZEM...");
      if(pzem.setAddress(ip))
        break;
      tries++;
      delay(1000);
  }
  float v = pzem.voltage(ip);
  float i = pzem.current(ip);
  float p = pzem.power(ip);
  float e = pzem.energy(ip);
  //float f = pzem->frequency();
  //float pf = pzem->pf();
  String message = "{";
  message += "\"voltage\":";
  message += v;
  message += ",\"current\":";
  message += i;
  /*message += ",\"frequency\":";
  message += f;
  message += ",\"pf\":";
  message += pf;*/
  message += ",\"power\":";
  message += p;
  message += ",\"energy\":";
  message += e;
  message += "}";
  return message;
}
