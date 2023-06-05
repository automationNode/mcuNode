#define DEBUG_VERBOSE true
#include <Arduino.h>
#include "_spiffs.h"
#include "_json.h"
#include "_wifi.h"
#include "_display.h"
#include "_interpreter.h"
#include "_server.h"

void setup()
{

  Serial.begin(115200); // comunicacion serial

  spiffsStart();
  displayStart();

  backendJson = stringToJson(readFile("/backend.json"));
  serializeJsonPretty(backendJson, Serial);

  instructionsParse();
  //executeSetup();

  displayPresentation();
  displayConnecting();

  if (backendJson["wifi"]["client"]["activated"] == true)
  {
    wifiClientConnect(backendJson["wifi"]["client"]["name"], backendJson["wifi"]["client"]["password"]);
  }
  else if (backendJson["wifi"]["accessPoint"]["activated"] == true)
  {
    wifiAccesPointConnect(backendJson["system"]["name"], backendJson["system"]["password"]);
  }

  displayInfo();
  serverStart();
}

void loop()
{
  if (restart)
  {
    displayRestarting();
    delay(500);
    ESP.restart();
  }
  //executeLoop();
}