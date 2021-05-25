#include "Voltage_data.h"
Voltage_data::Voltage_data(int _pin,float _ratio){
  pin=_pin;
  ratio=_ratio;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

String Voltage_data::read_data(){
  digitalWrite(pin, HIGH);
  delay(500);
  float volt1digit = analogRead(A0);
  float volt1 = volt1digit * ratio * 3.3 / 1023.0;
  digitalWrite(pin, LOW);
  String message = "{";
  message += "\"volt1\":";
  message += volt1;
  message += ",\"volt1digit\":";
  message += volt1digit;
  message += ",\"ratio\":";
  message += ratio;
  message += "}";
  return message;
}
