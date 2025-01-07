#include "PowMr_data.h"
#include "PZEM004Tv30.h"
PowMr_data::PowMr_data(int _pin_rec,int _pin_send){

}

String PowMr_data::read_data(){
  /*IPAddress ip(192,168,1,1);
  float v = pzem->voltage(ip);
  float i = pzem->current(ip);
  float p = pzem->power(ip);
  float e = pzem->energy(ip);*/
  //float f = pzem->frequency();
  //float pf = pzem->pf();
  /*String message = "{";
  message += "\"voltage\":";
  message += v;
  message += ",\"current\":";
  message += i;
  message += ",\"frequency\":";
  message += f;
  message += ",\"pf\":";
  message += pf;
  message += ",\"power\":";
  message += p;
  message += ",\"energy\":";
  message += e;
  message += "}";
  return message;*/
  return "";
}
