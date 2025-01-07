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
    uint8_t address_temp1[8],address_temp2[8];
    //DeviceAddress temp1, temp2;
    OneWire oneWire;
    int pin,precision;
    bool t1,t2;
  public:
    Temperature_data(int pin,int precision);
	  String read_data();
    void init_addr();
};

#endif
