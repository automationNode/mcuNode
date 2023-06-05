#ifndef _json_h
#define _json_h
#include <Arduino.h>
#include <ArduinoJson.h>

DynamicJsonDocument backendJson(1024 * 20);

DynamicJsonDocument stringToJson(String jsonString)
{
    DynamicJsonDocument json(1024 * 20);
    deserializeJson(json, jsonString);
    return json;
}

String jsonToString(DynamicJsonDocument json)
{
    String jsonString = "";
    serializeJson(json, jsonString);
    return jsonString;
}

bool isNumber(DynamicJsonDocument parameter)
{
    return parameter.is<float>() ||
           parameter.is<double>() ||
           parameter.is<bool>() ||
           parameter.is<short>() ||
           parameter.is<unsigned short>() ||
           parameter.is<signed short>() ||
           parameter.is<long>() ||
           parameter.is<unsigned long>() ||
           parameter.is<signed long>() ||
           parameter.is<long long>() ||
           parameter.is<unsigned long long>() ||
           parameter.is<signed long long>() ||
           parameter.is<int>() ||
           parameter.is<unsigned int>() ||
           parameter.is<signed int>();
}

bool isString(DynamicJsonDocument parameter)
{
    return parameter.is<String>() ||
           parameter.is<const char *>() ||
           parameter.is<String>();
}

bool isArray(DynamicJsonDocument parameter)
{
    return parameter.is<JsonArray>() || parameter.is<JsonArrayConst>();
}

bool isObject(DynamicJsonDocument parameter)
{
    return parameter.is<JsonObject>() || parameter.is<JsonObjectConst>();
}

#endif