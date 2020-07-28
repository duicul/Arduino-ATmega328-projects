#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <Arduino.h>
#include <EEPROM.h>

#ifndef APSSID
#define APSSID "HomeMeasurement_1"
#define APPSK  ""
#endif

class Credentials {
  
  private:
    char ssid[20], password[20];
  public:
    char* getssid();
    char* getpassword();
    void loadCredentials();
    void saveCredentials();
    void setCredentials(char *s,char *p);
};

#endif
