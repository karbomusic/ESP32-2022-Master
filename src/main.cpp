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

  Kary Wall 1/20/2022.
===================================================================+*/

#include <Arduino.h>
#include <localWiFi.h> 
#include <localUpdateServer.h>

extern WebServer httpServer;
extern void startWifi();
extern void startUpdateServer();

void setup()
{
    /*--------------------------------------------------------------------
     Boot and I/O initialization.
    ---------------------------------------------------------------------*/
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting up...");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
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