#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <WiFiUdp.h>
#include <NTPClient.h>

#include "Credentials.h"

#define VALVE_PIN D2

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

unsigned long waterStart,waterStop;

int remainingMins = 0;
boolean water = false;
unsigned long lastNTPupdate;

ESP8266WebServer server(80);
boolean access_point = false;

Credentials cred=Credentials();

volatile uint32_t access_point_start = 0;

void handleRestart() {
  server.send(200, "text/plain", "restart");
  delay(2000);
  ESP.restart();
}

void handleWater(){
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  waterStart = timeClient.getEpochTime();
  String aux = server.arg("time");
  if(aux != nullptr && aux != NULL)
    remainingMins = aux.toInt();
  else
    remainingMins = 0;
  waterStop = waterStart + 60*remainingMins;
  openValve();
  String message = "{\"message\":\"started watering\",\"remaining\":"+(String)remainingMins+"}";
  server.send(200, "application/json", message);
}

void handleStop(){
  remainingMins = 0;
  closeValve();
  String message = "{\"message\":\"stopped watering\"}";
  server.send(200, "application/json", message);
}

void handleRemaining(){
  String message = "{\"remaining\":"+(String)remainingMins+"}";
  server.send(200, "application/json", message);
}

void handlechangecredentials() {
  String message = "";
  String s = server.arg("SSID"), p = server.arg("PSK");
  char ssid[20],password[20];
  s.toCharArray(ssid, 20);
  p.toCharArray(password, 20);
  cred.setCredentials(ssid,password);
  message = s + "  " + p;
  server.send(200, "text/plain", message);
  cred.saveCredentials();
  delay(2000);
  ESP.restart();
}

void handleRoot() {
  Serial.println("scan start");
  int n = 0;
  n = WiFi.scanNetworks();
  String mess = "<!DOCTYPE HTML>\r\n<html>";
  mess += "<head><meta content=\"text/html;charset=utf-8\" http-equiv=\"Content-Type\"><meta content=\"utf-8\" http-equiv=\"encoding\"></head>\r\n";
  mess += "<body>";
  mess += n;
  mess += " networks found </br>";
  for (int q = 0; q < n; ++q) {
    mess += (q + 1);
    mess += ": ";
    mess += WiFi.SSID(q);
    mess += " <-> ";
    mess += WiFi.RSSI(q);
    mess += " ";
    mess += WiFi.encryptionType(q);
    mess += "</br>";
    delay(10);
  }
  mess += "<form method='get' action='change'><label>SSID: </label><input name='SSID' length=32><label>Paswoord: </label><input name='PSK' length=64><input type='submit'></form>";
  mess += "</body></html>";
  server.send(200, "text/html", mess);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  
  cred.loadCredentials();
  
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(cred.getssid(), cred.getpassword());
  
  boolean connect = false;
  
  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count++ < 20) {
    delay(2000);
    Serial.print(".");}
    
  Serial.println();
  if (WiFi.status() != WL_CONNECTED)
  { Serial.println("Not connected creating acces point : ");
    WiFi.softAP(APSSID);
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("AP IP address: ");
    Serial.println(myIP);
    access_point = true;
    access_point_start = millis();
  }
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/change", handlechangecredentials);
  server.on("/restart", handleRestart);
  server.on("/startWater", handleWater);
  server.on("/stopWater", handleStop);
  server.on("/remaining", handleRemaining);
  server.begin();
  Serial.println("HTTP server started");
  // Start up the library

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(0);
   pinMode(VALVE_PIN,OUTPUT);
   closeValve();
   lastNTPupdate = millis();
   if(remainingMins>0 && water == true)
   load_time();
}

void load_time(){
  
  unsigned long period = millis() - lastNTPupdate;
  if(period < 0 || period > 20000){  
    Serial.println("Loading time "+(String)lastNTPupdate+" "+(String)period);
    while(!timeClient.update()) {
      timeClient.forceUpdate();
    }

    unsigned long epochTime = timeClient.getEpochTime();

    remainingMins =  (waterStop - epochTime) / 60;
    lastNTPupdate = millis();
    Serial.println("Load time remaining: "+(String)remainingMins);
  }
}

void closeValve(){
  Serial.println("Close Valve");
  water = false;
  digitalWrite(VALVE_PIN,0);
}

void openValve(){
  Serial.println("Open Valve");
  water = true;
  digitalWrite(VALVE_PIN,1);
}

boolean stopWatering(){
  if(remainingMins <= 0)
    return true;
  return false;
}

void loop() {  
  if (!access_point && WiFi.status() != WL_CONNECTED)
    ESP.restart();
  if (access_point){
    volatile uint32_t access_point_interval = millis() - access_point_start;
    if(access_point_interval > 300000 || access_point_interval <0)
        ESP.restart();
  }
  if(remainingMins>0 && water == true)
    load_time();
  server.handleClient();
  if(stopWatering() && water == true){
    closeValve();
  }
    
}
