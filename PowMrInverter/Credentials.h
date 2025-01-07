#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <Arduino.h>
#include <EEPROM.h>

#ifndef APSSID
#define APSSID "HomeMeasurement_0001"
#define APPSK  ""
#endif

class Credentials {
  
  private:
    char ssid[20], password[20];
    float ratio;
  public:
    char* getssid();
    char* getpassword();
    float getratio();
    void loadCredentials();
    void saveCredentials();
    void setCredentials(char *s,char *p,float r);
    Credentials();
};

#endif
