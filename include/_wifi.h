
#ifndef _wifi_h
#define _wifi_h

#include <Arduino.h>
#include <WiFi.h>

char wifiname[30] = "";
char wifipassword[30] = "";
String localIp;

void wifiAccesPointConnect(String wifiName, String wifiPassword)
{

  wifiName.toCharArray(wifiname, 20);
  wifiPassword.toCharArray(wifipassword, 20);

  Serial.println("wifiAPname: " + String(wifiname));
  Serial.println("wifiAPpassword: " + String(wifipassword));
  Serial.println("Connecting");

  WiFi.mode(WIFI_AP); 
  WiFi.softAP(wifiname, wifipassword);

  Serial.println("Connected!");
  Serial.print("WIFINODE IP address: ");
  localIp = WiFi.softAPIP().toString();
  backendJson["system"]["directions"]["tcpip"] = localIp;
  saveFile("/backend.json", jsonToString(backendJson));
  Serial.println("Local ip: " + localIp);
}

void wifiClientConnect(String wifiName, String wifiPassword)
{

  wifiName.toCharArray(wifiname, 20);
  wifiPassword.toCharArray(wifipassword, 20);

  Serial.println("\nwifiname: " + String(wifiname));
  Serial.println("wifipassword: " + String(wifipassword));
  Serial.println("Connecting");

  WiFi.begin(wifiname, wifipassword);

  int count = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    count++;
    Serial.print(".");
    if (count == 20)
    {
      return wifiAccesPointConnect(backendJson["system"]["name"], backendJson["system"]["password"]);
    }
  }

  Serial.println("Connected!");
  Serial.print("WIFINODE IP address: ");
  localIp = WiFi.localIP().toString();
  backendJson["system"]["directions"]["tcpip"] = localIp;
  saveFile("/backend.json", jsonToString(backendJson));
  Serial.println("Local ip: " + localIp);
}

#endif