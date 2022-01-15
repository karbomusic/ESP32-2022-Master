/*+===================================================================
  Project:  ESP32 Project template with builtin OTA,
            HTTP Server, WiFi connectivity and About page.
            Only manual OTA updates (/update) are supported.

            There is a second build config that contains the oled
            display: heltec_wifi_kit_32
  
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

  Building:  pio run -t <target> -e envName

             Examples:
                pio run -t upload, monitor -e heltec_wifi_kit_32
                pio run -t upload, monitor -e fm-dev-kit
                pio run -t upload -e heltec_wifi_kit_32 --upload-port COM6
                pio run -t upload -e fm-dev-kit
                pio run -t upload -e fm-dev-kit --upload-port COM6

             List targets: pio run --list-targets

  Kary Wall 1/20/2022.
===================================================================+*/

#include <Arduino.h>
#include "SPIFFS.h"
#include <localWiFi.h>
#include <localUpdateServer.h>

#if defined(heltec_wifi_kit_32)  // OLED display
#include <oled.h>
#endif

extern WebServer httpServer;
extern void startWifi();
extern void startUpdateServer();
extern bool isWiFiConnected();
extern int g_lineHeight;

const int activityLED = 12;
unsigned long lastUpdate = 0;

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
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

#if defined(heltec_wifi_kit_32)  
    /*--------------------------------------------------------------------
     Heltec OLED display initialization.
    ---------------------------------------------------------------------*/
    g_OLED.begin();
    g_OLED.clear();
    g_OLED.setFont(u8g2_font_profont15_tf);
    g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // Descent is a negative number so we add it to the total
    g_OLED.clearBuffer();
    g_OLED.setCursor(0, g_lineHeight);
    g_OLED.printf("Connecting to WiFi...");
    g_OLED.sendBuffer();
#endif  

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

#if defined(heltec_wifi_kit_32)  
    /*--------------------------------------------------------------------
        Update oled every second
    ---------------------------------------------------------------------*/
    if (millis() - lastUpdate > 1000)
    {
        g_OLED.clearBuffer();
        g_OLED.setCursor(0, g_lineHeight);
        g_OLED.printf("IP: %s", globalIP.c_str());
        g_OLED.setCursor(0, g_lineHeight * 2);
        g_OLED.printf("%s", hostName.c_str());
        g_OLED.setCursor(0, g_lineHeight * 3);
        g_OLED.printf("SSID: %s", ssid.c_str());
        g_OLED.setCursor(0, g_lineHeight * 4);
        g_OLED.printf("Connected: %s", String(isWiFiConnected()).c_str());
        g_OLED.sendBuffer();
        lastUpdate = millis();
    }
#endif  

    /*--------------------------------------------------------------------
     Project specific loop code
    ---------------------------------------------------------------------*/

     // Your code here.

    /*--------------------------------------------------------------------
     Required for web server and OTA updates
    ---------------------------------------------------------------------*/
    httpServer.handleClient();
    delay(1);
}