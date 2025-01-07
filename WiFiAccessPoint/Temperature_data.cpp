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
  if (!sensors.getAddress(address_temp1, 0)) {
    t1 = false;
    Serial.println("Unable to find address for Device 0");
  }
  if (!sensors.getAddress(address_temp2, 1)) {
    t2 = false;
    Serial.println("Unable to find address for Device 1");
  }
  sensors.setResolution(address_temp1, precision);
  sensors.setResolution(address_temp2, precision);
}

String Temperature_data::read_data(){
  sensors.requestTemperatures();
  float tempC1 = sensors.getTempC(address_temp1);
  float tempC2 = sensors.getTempC(address_temp2);
  /*float tempC1 = sensors.getTempCByIndex(0);
  float tempC2 = sensors.getTempCByIndex(1);*/
  String addr1 ="",addr2="";
  for (uint8_t i = 0; i < 8; i++)
      {
        addr1+="0x";
        addr2+="0x";
        if (address_temp1[i] < 0x10) addr1+="0";
        if (address_temp2[i] < 0x10) addr2+="0";
        
        addr1+=address_temp1[i];
        addr1+=HEX;
        addr2+=address_temp2[i];
        addr2+=HEX;
        if (i < 7) 
          {addr1+=", ";
          addr2+=", ";}
      }
  
  String message = "{";
  message += "\"temp1\":";
  message += tempC1;
  message += ",\"temp1_addr\":";
  message += t1?"\"true\"":"\"false\"";
  message += ",\"temp1_addr_val\":";
  message += "\""+addr1+"\"";
  message += ",\"temp2\":";
  message += tempC2;
  message += ",\"temp2_addr\":";
  message += t2?"\"true\"":"\"false\"";
  message += ",\"temp2_addr_val\":";
  message += "\""+addr2+"\"";
  message += "}";
  return message;
}
