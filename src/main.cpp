/*+===================================================================
  Project:  ESP32 Project template with builtin OTA,
            HTTP Server, WiFi connectivity and About page.
            Only manual OTA updates (/update) are supported.
  
  File:      main.cpp

  Project:   <yourprojectname>
             
  Summary:   Project template that includes plubming and code for 
             a WiFi client + OTA updates via manual update.
             Automatic updates are not yet implemented but
             may be ported over from my legacy projects.

             Architecture: ESP32 specific.

  Dependencies:

             <localWifi.h> 
                -- <WiFi.h>
                -- <WiFiClient.h>
                -- <ESPmDNS.h>  
                -- <secrets.h>            
             <localUPdateServer.h>
                -- <WebServer.h>
                -- <Update.h>
                -- <strings.h>
            
  Config:    You must update secrets.h with your WiFi credentials
             and the hostname you choose for this device.
             SPIFFs for HTML requires update.html to be uploaded first.
             https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/

  Kary Wall 1/20/2022.
===================================================================+*/

#include <Arduino.h>
#include "SPIFFS.h"
#include <localWiFi.h> 
#include <localUpdateServer.h>

extern WebServer httpServer;
extern void startWifi();
extern void startUpdateServer();

const int activityLED = 12;

void setup()
{
    /*--------------------------------------------------------------------
     Boot and I/O initialization.
    ---------------------------------------------------------------------*/
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting up...");
    pinMode(activityLED, OUTPUT);
    digitalWrite(activityLED, LOW);

    // SPIFFs support
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
        
    /*--------------------------------------------------------------------
     Start WiFi & OTA HTTP update server
    ---------------------------------------------------------------------*/
    startWifi();
    startUpdateServer();

    /*--------------------------------------------------------------------
     Project specific setup code
    ---------------------------------------------------------------------*/
}

void loop()
{
    /*--------------------------------------------------------------------
     Project specific loop code
    ---------------------------------------------------------------------*/

    
    /*--------------------------------------------------------------------
     Required for web server and OTA updates
    ---------------------------------------------------------------------*/
    httpServer.handleClient();
    delay(1);
}