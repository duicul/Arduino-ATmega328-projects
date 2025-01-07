#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "PowMr_data.h"
#include "Credentials.h"


ESP8266WebServer server(80);
boolean access_point = false;


Credentials cred=Credentials();

//Temperature_data temp=Temperature_data(D8,9);
//Voltage_data volt=Voltage_data(A0,cred.getratio()); //5
//AC_data ac=AC_data(D6,D5); // 12 14

volatile uint32_t access_point_start = 0;

void handleRestart() {
  server.send(200, "text/plain", "restart");
  delay(2000);
  ESP.restart();
}

/*void handleFindDevices()
{ Serial.println("handleFindDevices");
  OneWire ow(D8);
  ow.reset_search();
  uint8_t address[8];
  uint8_t count = 0;

  String mess="";
  
  if (ow.search(address))
  {
    Serial.print("\nuint8_t pin");
    mess+="\nuint8_t pin";
    Serial.print(DEC);
    mess+=DEC;
    Serial.println("[][8] = {");
    mess+="[][8] = {\n";
    {
      count++;
      Serial.println("  {");
      mess+="  {\n";
      for (uint8_t i = 0; i < 8; i++)
      {
        Serial.print("0x");
        mess+="0x";
        if (address[i] < 0x10) Serial.print("0");
        Serial.print(address[i], HEX);
        mess+=address[i];
        mess+=HEX;
        if (i < 7) 
          {Serial.print(", ");
          mess+=", ";}
      }
      Serial.print("  },");
      mess+="  },";
      // CHECK CRC
      if (ow.crc8(address, 7) == address[7])
      {
        Serial.println("\t\t// CRC OK");
        mess+="\t\t// CRC OK\n";
      }
      else
      {
        Serial.println("\t\t// CRC FAILED");
        mess+="\t\t// CRC FAILED\n";
      }
    } while (ow.search(address));

    Serial.println("};");
    mess+="};\n";
    Serial.print("// nr devices found: ");
    mess+="// nr devices found: ";
    Serial.println(count);
    mess+=count;
    mess+="\n";
  }
  else{
    mess+="No devices found \n";
  }
  ow.reset_search();
  server.send(200, "text/plain", mess);
}*/

/*void handleVoltage() {
  Voltage_data volt_aux=Voltage_data(A0,cred.getratio()); //5
  String message = volt_aux.read_data();
  server.send(200, "application/json", message);
}

void handleAC() {
  String message = ac.read_data();
  server.send(200, "application/json", message);
}

void handleTemperature() {
  Temperature_data temp_aux=Temperature_data(D8,9);
  String message = temp_aux.read_data();
  server.send(200, "application/json", message);
}*/

void handlechangecredentials() {
  String message = "";
  String s = server.arg("SSID"), p = server.arg("PSK"),ratio=server.arg("ratio");
  char ssid[20],password[20];
  s.toCharArray(ssid, 20);
  p.toCharArray(password, 20);
  cred.setCredentials(ssid,password,ratio.toFloat());
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
  mess += "<form method='get' action='change'><label>SSID: </label><input name='SSID' length=32><label>Paswoord: </label><input name='PSK' length=64><label>Ratio:</label><input name=\"ratio\" type=\"number\" step=\"0.1\" value=\""+(String)cred.getratio()+"\"><input type='submit'></form>";
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
  while (WiFi.status() != WL_CONNECTED && count++ < 50) {
    delay(2000);
    Serial.print(".");}
    
  Serial.println();
  if (WiFi.status() != WL_CONNECTED)
  { Serial.println("Not connected creating acces point : ");
    Serial.println(APSSID);
    WiFi.softAP(APSSID);
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("AP IP address: ");
    Serial.println(myIP);
    access_point = true;
    access_point_start = millis();
  }

  //temp=Temperature_data(D8,9);//4 now 15
  //volt=Voltage_data(A0,50); //5
  //ac=AC_data(D6,D5); // 12 14
  
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  /*server.on("/temperature", handleTemperature);
  server.on("/voltage", handleVoltage);
  server.on("/ac", handleAC);*/
  server.on("/change", handlechangecredentials);
  //server.on("/scanonewire", handleFindDevices);
  server.on("/restart", handleRestart);
  server.begin();
  Serial.println("HTTP server started");
  // Start up the library
  
  /*Serial.println(temp.read_data());
  Serial.println(volt.read_data());
  Serial.println(ac.read_data());*/
  
}

void loop() {
  if (!access_point && WiFi.status() != WL_CONNECTED)
    ESP.restart();
  if (access_point){
    volatile uint32_t access_point_interval = millis() - access_point_start;
    if(access_point_interval > 300000 || access_point_interval <0)
        ESP.restart();
  }
    server.handleClient();
}
