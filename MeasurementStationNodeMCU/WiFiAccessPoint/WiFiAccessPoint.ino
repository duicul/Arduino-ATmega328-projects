#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#ifndef APSSID
#define APSSID "HomeMeasurement_1"
#define APPSK  ""
#endif

/* Set these to your desired credentials. */
const char *ssid_ap = APSSID;
const char *password_ap = APPSK;
char ssid[20],password[20];

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
/*void handleRoot() {
  Serial.println("Request on /");
  server.send(200, "text/html", "<h1>Use /change?SSID=newssid&PSK=newpsk to change credentials</h1>");
}*/

void handlechangecredentials() { 
    String message = "";
    String s=server.arg("SSID"),p=server.arg("PSK");
    s.toCharArray(ssid,20);
    p.toCharArray(password,20);
    message=s+"  "+p;
    server.send(200, "text/plain", message);
    saveCredentials();
    delay(2000);
    ESP.restart();
}

void handleRoot(){
  Serial.println("scan start");
  int n = 0;
  n = WiFi.scanNetworks();
  String mess="<!DOCTYPE HTML>\r\n<html>";
  mess+="<head><meta content=\"text/html;charset=utf-8\" http-equiv=\"Content-Type\"><meta content=\"utf-8\" http-equiv=\"encoding\"></head>\r\n";
  mess+="<body>";
  mess+=n;
  mess+=" networks found </br>";
  for (int q = 0; q < n; ++q) {
    mess+=(q+1);
    mess+=": ";
    mess+=WiFi.SSID(q);
    mess+=" <-> ";
    mess+=WiFi.RSSI(q);
    mess+=" ";
    mess+=WiFi.encryptionType(q);
    mess+="</br>";
    delay(10);
  }
  mess+="<form method='get' action='change'><label>SSID: </label><input name='SSID' length=32><label>Paswoord: </label><input name='PSK' length=64><input type='submit'></form>";
  mess+="</body></html>";
  server.send(200, "text/html", mess);
}

void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(password);
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.commit();
  EEPROM.end();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  loadCredentials();
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  boolean connect=false;
  int count=0;
  while (WiFi.status() != WL_CONNECTED && count++<50) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println();
  if(WiFi.status() != WL_CONNECTED)
    {Serial.println("Not connected creating acces point : ");
    WiFi.softAP(ssid_ap);
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("AP IP address: ");
    Serial.println(myIP);}
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/change", handlechangecredentials);
  //server.on("/scan", handlescan);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
