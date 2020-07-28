#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include "Data.h"
#include <OneWire.h>
#include <DallasTemperature.h>

//#define ONE_WIRE_BUS 4
//#define TEMPERATURE_PRECISION 9

class Temperature_data: public Data_interface {
  
  private:
	  DallasTemperature sensors;
    DeviceAddress temp1, temp2;
    OneWire oneWire;
    int pin,precision;
  public:
    Temperature_data(int pin,int precision);
	  String read_data();	
};

#endif
