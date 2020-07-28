#include "AC_data.h"

AC_data::AC_data(int _pin_rec,int _pin_send){
  
  pin_rec=_pin_rec;
  pin_send=_pin_send;
  pzem=new PZEM004T(pin_rec,pin_send);
}

String AC_data::read_data(){
  float v = pzem->voltage(ip);
  float i = pzem->current(ip);
  float p = pzem->power(ip);
  float e = pzem->energy(ip);
  String message = "{";
  message += "\"voltage\":";
  message += v;
  message += ",\"current\":";
  message += i;
  message += ",\"power\":";
  message += p;
  message += ",\"energy\":";
  message += e;
  message += "}";
  return message;
}
