#ifndef _interpreter_h
#define _interpreter_h
#include <Arduino.h>
#include <analogWrite.h>
#include "_json.h"
#include "_spiffs.h"
#include "_wifi.h"
#include "_display.h"
#include "_json.h"

// #define DEBUG
// #define DEBUG_VERBOSE

DynamicJsonDocument variablesJson(1024 * 5);

DynamicJsonDocument instructionsJson(1024 * 15);
int instructionsSetupLength = 0;
int instructionsLoopLength = 0;

bool startup = true;
bool restart = false;

DynamicJsonDocument unparse(DynamicJsonDocument contentJson)
{

    if (contentJson["get"]["backendJson"] == true)
    {
        contentJson["return"] = true;
        contentJson["content"] = backendJson;
        return contentJson;
    }
    if (contentJson["get"]["programmingFile"] == true)
    {
        DynamicJsonDocument contentJson(1024 * 20);
        contentJson["return"] = true;
        contentJson["content"] = readFile("/programmingFile.js");
        return contentJson;
    }
    if (contentJson["set"]["programmingFile"] == true)
    {
        String content = contentJson["content"];
#ifdef DEBUG_VERBOSE
        Serial.println();
        Serial.println();
        Serial.println();
        Serial.println("--> Content: " + content);
#endif
        saveFile("/programmingFile.js", content);
        contentJson["return"] = true;
        return contentJson;
    }
    if (contentJson["set"]["wifi"]["configuration"] == true)
    {
        String client = contentJson["client"];
#ifdef DEBUG_VERBOSE
        Serial.println("--> contentJson[set][wifi][configuration]: " + client);
#endif
        backendJson["wifi"]["client"] = contentJson["client"];
        backendJson["wifi"]["accessPoint"]["activated"] = !contentJson["client"]["activated"];
        saveFile("/backend.json", jsonToString(backendJson));
        contentJson["return"] = true;
        restart = true;
        return contentJson;
    }
    if (contentJson["execute"] == true)
    {

        if (contentJson["instruction"]["action"] == "pinMode")
        {

            DynamicJsonDocument parameters(1024);
            parameters = stringToJson(contentJson["instruction"]["parameters"]);

            uint8_t gpio = parameters[0];
            String value = parameters[1];

            if (value == "OUTPUT")
            {
                pinMode(gpio, OUTPUT);
            }
            else if (value == "INPUT")
            {
                pinMode(gpio, INPUT);
            }
            contentJson["instruction"]["executed"] = true;
            contentJson["instruction"]["return"] = value;
            contentJson["return"] = value;
            return contentJson;
        }

        if (contentJson["instruction"]["action"] == "digitalWrite")
        {
            DynamicJsonDocument parameters(1024);
            parameters = stringToJson(contentJson["instruction"]["parameters"]);

            uint8_t gpio = parameters[0];
            String value = parameters[1];
            if (value == "HIGH")
            {
                digitalWrite(gpio, HIGH);
            }
            else if (value == "LOW")
            {
                digitalWrite(gpio, LOW);
            }
            contentJson["instruction"]["executed"] = true;
            contentJson["instruction"]["return"] = value;
            contentJson["return"] = value;
            return contentJson;
        }
        if (contentJson["instruction"]["action"] == "analogWrite")
        {
            DynamicJsonDocument parameters(1024);
            parameters = stringToJson(contentJson["instruction"]["parameters"]);

            uint8_t gpio = parameters[0];
            float value = parameters[1];
            analogWrite(gpio, int32_t((value / 100) * 255));
            contentJson["instruction"]["executed"] = true;
            contentJson["instruction"]["return"] = value;
            contentJson["return"] = value;
            return contentJson;
        }
        if (contentJson["instruction"]["action"] == "digitalRead")
        {

            DynamicJsonDocument parameters(1024);
            parameters = stringToJson(contentJson["instruction"]["parameters"]);

            int gpio = parameters[0];
            contentJson["instruction"]["executed"] = true;
            contentJson["instruction"]["return"] = bool(digitalRead(gpio));
            contentJson["return"] = bool(digitalRead(gpio));
            return contentJson;
        }
        if (contentJson["instruction"]["action"] == "analogRead")
        {
            DynamicJsonDocument parameters(1024);
            parameters = stringToJson(contentJson["instruction"]["parameters"]);

            int gpio = parameters[0];
            float lecture = float(analogRead(gpio));
            contentJson["instruction"]["executed"] = true;
            contentJson["instruction"]["return"] = int((lecture / 4095) * 100);
            contentJson["return"] = int((lecture / 4095) * 100);
            return contentJson;
        }
        contentJson["return"] = false;
        return contentJson;
    }
    contentJson["return"] = NULL;
    return contentJson;
}

#endif