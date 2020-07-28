#include "Temperature_data.h"
Temperature_data::Temperature_data(int _pin,int _precision){
  oneWire=OneWire(_pin);
  sensors=DallasTemperature(&oneWire);
  pin=_pin;
  precision=_precision;
  sensors.begin();
  if (!sensors.getAddress(temp1, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(temp2, 1)) Serial.println("Unable to find address for Device 1");
  sensors.setResolution(temp1, precision);
  sensors.setResolution(temp2, precision);
}

String Temperature_data::read_data(){
  sensors.requestTemperatures();
  float tempC1 = sensors.getTempC(temp1);
  float tempC2 = sensors.getTempC(temp2);
  String message = "{";
  message += "\"temp1\":";
  message += tempC1;
  message += ",\"temp2\":";
  message += tempC2;
  message += "}";
  return message;
}
