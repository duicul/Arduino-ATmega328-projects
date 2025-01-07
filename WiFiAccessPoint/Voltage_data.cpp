#include "Voltage_data.h"
Voltage_data::Voltage_data(int _pin,float _ratio){
  pin=_pin;
  ratio=_ratio;
  //pinMode(pin, OUTPUT);
  //digitalWrite(pin, LOW);
}

String Voltage_data::read_data(){
  int i=0;
  float sum_value = 0;
  float last_volt1digit = 0;
  float last_volt1 = 0;
  for(i=0;i<10;i++){
    //digitalWrite(pin, HIGH);
    delay(50);
    float volt1digit = analogRead(pin);
    float volt1 = volt1digit * ratio * 3.3 / 1023.0;
    sum_value += volt1;
    last_volt1digit = volt1digit;
    last_volt1 = volt1;
    //digitalWrite(pin, LOW);
    }
  String message = "{";
  message += "\"volt1\":";
  message += sum_value/10;
  message += ",\"last_volt1digit\":";
  message += last_volt1digit;
  message += ",\"last_volt1\":";
  message += last_volt1;
  message += ",\"ratio\":";
  message += ratio;
  message += "}";
  return message;
}
