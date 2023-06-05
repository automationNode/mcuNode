#ifndef _spiffs_h
#define _spiffs_h

#include <Arduino.h>
#include <SPIFFS.h>
#include "SPI.h"

void spiffsStart()
{
  if (!SPIFFS.begin())
  { // Aqui se declara el pin SPI "ss"
    delay(1000);
    Serial.println("--> Inicializacion de SPIFFS fallida");
  }
  else
  {
    delay(1000);
    Serial.println("--> Initializacion de SPIFFS exitosa.");
  }
}

String readFile(String path)
{
  String content = "";
  File file = SPIFFS.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("--> Failed to open file for reading");
  }
  while (file.available())
  {
    content += char(file.read());
  }
  return content;
}

bool saveFile(String path, String content)
{

  File file = SPIFFS.open(path, FILE_WRITE);
  if (!file)
  {
    return false;
  }
  if (file.print(content))
  {
    return true;
  }
  else
  {
    return false;
  }
}

#endif