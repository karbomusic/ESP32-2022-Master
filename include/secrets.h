/*+===================================================================
  File:      secrets.h

  Summary:   Device and WiFi specfic details go here such as 
             SSID, password and hostname. Also some are used
             for OTA updates and the about page.

  Notes:     deviceFamily and softwareVersion are used by your
             remote update server to determine if a new update is
             available for this device. If you need multiple devices
             with identical firmware you can use the same for all.

             It wouldn't be a bad idea to combine these strings as the
             filename: mydevice-12.10.21.bin. This would allow the 
             server to match mydevice-12.10.21.bin to a file on the 
             server named mydevice-12.22.21.bin. Assuming you are
             using automatic updates, since manual is kind of better.

  Kary Wall 1/20/2022.            
===================================================================+*/

#include <Arduino.h>

#ifndef STASSID
#define STASSID "IOTNET2G"
#define STAPSK  "creekvalley124"
#endif

String ssid = STASSID;                                             // WiFi ssid
String password = STAPSK;                                          // WiFi password
String hostName = "ESP32-Test";                                    // hostname as seen on network
String softwareVersion = "1.12.22";                                // used for OTA updates & about page
String deviceFamily = "ESP32-Test-Device";                         // used for OTA updates & about page
String description = "I'm a test device for ESP32 Template";       // used for about page
String globalIP = "";