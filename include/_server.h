#ifndef _server_h
#define _server_h

#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <DNSServer.h>
#include "_json.h"
#include "_interpreter.h"

DNSServer dnsServer;
AsyncWebServer server(80);
String contentBody = "";

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        // request->addInterestingHeader("ANY");
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html");
    }
};

void serverStart()
{
    server.serveStatic("/", SPIFFS, "/");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html"); });

    server.on(
        "/data", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            for (size_t i = 0; i < len; i++)
            {
                contentBody += char(data[i]);
                
            }

            if (!index)
            {
                //Serial.println(contentBody);
            }

            if (index + len == total)
            {
                DynamicJsonDocument contentJson(1024*20);
                Serial.println("--> INPUT: " + contentBody);
                contentJson = stringToJson(contentBody);
                contentJson = unparse(contentJson);
                contentBody = jsonToString(contentJson);
                Serial.println("--> OUTPUT: " + contentBody);
                String responseString = contentBody;
                contentBody = "";
                AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", responseString);
                response->addHeader("Access-Control-Allow-Origin", "*");
                response->addHeader("Access-Control-Allow-Credentials", "true");
                request->send(response);
            } });

    if (backendJson["wifi"]["accessPoint"]["activated"] == true)
    {
        Serial.println("--> Activated DNS Handler");
        dnsServer.start(53, "*", WiFi.softAPIP());
        server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP
    }

    Serial.println("-->Starting server");
    server.begin();
}

#endif