#include "Temperature_data.h"
Temperature_data::Temperature_data(int _pin,int _precision){
  oneWire=OneWire(_pin);
  sensors=DallasTemperature(&oneWire);
  pin=_pin;
  precision=_precision;
  sensors.begin();
  this->init_addr();
}

void Temperature_data::init_addr(){
  t1=true;
  t2=true;
  if (!sensors.getAddress(temp1, 0)) {
    t1 = false;
    Serial.println("Unable to find address for Device 0");
  }
  if (!sensors.getAddress(temp2, 1)) {
    t2 = false;
    Serial.println("Unable to find address for Device 1");
  }
  sensors.setResolution(temp1, precision);
  sensors.setResolution(temp2, precision);
}

String Temperature_data::read_data(){
  this->init_addr();
  sensors.requestTemperatures();
  float tempC1 = sensors.getTempC(temp1);
  float tempC2 = sensors.getTempC(temp2);
  String message = "{";
  message += "\"temp1\":";
  message += tempC1;
  message += ",\"temp1_addr\":";
  message += t1?"\"true\"":"\"false\"";
  message += ",\"temp2\":";
  message += tempC2;
  message += ",\"temp2_addr\":";
  message += t2?"\"true\"":"\"false\"";
  message += "}";
  return message;
}
