#ifndef AC_H
#define AC_H

#include <Arduino.h>
#include "Data.h"
#include <SoftwareSerial.h>

#include <PZEM004T.h>
//#define ONE_WIRE_BUS 4
//#define TEMPERATURE_PRECISION 9

class AC_data: public Data_interface {
  private:    
    int pin_rec,pin_send;
    PZEM004T *pzem;
  public:
    AC_data(int pin_rec, int pin_send);
    String read_data(); 
};

#endif
