#ifndef AC_H
#define AC_H

#include <Arduino.h>
#include "Data.h"
#include <SoftwareSerial.h>
#include <ModbusMaster.h>

#include <PZEM004T.h>
//#define ONE_WIRE_BUS 4
//#define TEMPERATURE_PRECISION 9

class PowMr_data: public Data_interface {
  private:    
    int pin_rec,pin_send;
    ModbusMaster node;
  public:
    PowMr_data(int pin_rec, int pin_send);
    String read_data(); 
};

#endif
