#include "Credentials.h"

Credentials::Credentials(){
  this->loadCredentials();
}

void Credentials::loadCredentials(){
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0 + sizeof(ssid), password);
  EEPROM.get(0 + sizeof(ssid) + sizeof(password), ratio);
  char ok[2 + 1];
  EEPROM.get(0 + sizeof(ssid) + sizeof(password)+sizeof(ratio), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(ratio);
}
void Credentials::saveCredentials(){
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0 + sizeof(ssid), password);
  EEPROM.put(0 + sizeof(ssid) + sizeof(password), ratio);
  char ok[2 + 1] = "OK";
  EEPROM.put(0 + sizeof(ssid) + sizeof(password)+sizeof(ratio), ok);
  EEPROM.commit();
  EEPROM.end();
}

void Credentials::setCredentials(char *s,char *p,float r){
  strcpy(ssid,s);
  strcpy(password,p);
  ratio = r;
}

char* Credentials::getssid(){
  return ssid;
}

char* Credentials::getpassword(){
  return password;
}

float Credentials::getratio(){
  return ratio;
}
