#ifndef VOLTAGE_H
#define VOLTAGE_H

#include <Arduino.h>
#include "Data.h"

//#define DIVIDER 50
//#define VOLTAGE_CONTROL1 5

class Voltage_data: public Data_interface {
  
  private:
    int pin,divider;
  public:
    Voltage_data(int pin,int divider);
    String read_data(); 
};

#endif
