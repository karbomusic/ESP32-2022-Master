/*+===================================================================
  File:      localUpdateServer.h

  Summary:   Provides an HTTP server and HTTPUploader for OTA updates,
             both automatic and manual, and a detailed about page.

  Kary Wall 2022.
===================================================================+*/

#include <WebServer.h>
#include <Update.h>
#include <strings.h>

// externs
extern String ssid;            // WiFi ssid.
extern String password;        // WiFi password.
extern String hostName;        // hostname as seen on network.
extern String softwareVersion; // used for auto OTA updates & about page.
extern String deviceFamily;    // used for auto OTA updates & about page.
extern String description;     // used for about page.
extern String globalIP;        // needed for about page.
extern String style;           // css string for web pages.
extern String loginIndex;      // html string for login page (not used, need to remove).
extern String serverIndex;     // html string for update page.

WebServer httpServer(80);

// Prototypes
void handleAbout();

// Start the server
void startUpdateServer()
{

    Serial.println("mDNS responder started");
    /* return index page which is stored in serverIndex */
    httpServer.on("/", HTTP_GET, []()
                  { handleAbout(); });
    httpServer.on("/about", handleAbout);
    httpServer.on("/serverIndex", HTTP_GET, []()
                  {
                      httpServer.sendHeader("Connection", "close");
                      httpServer.send(200, "text/html", serverIndex);
                  });
    
    httpServer.on( /* If there is a file, upload it, if a GET redirect to upload firmware page */
        "/update", HTTP_ANY, []()
        {
            if (httpServer.method() == HTTP_GET)
            {
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/html", serverIndex);
            }
            else if (httpServer.method() == HTTP_POST)
            {
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/plain", (Update.hasError()) ? "FAIL... Restarting in 5 seconds" : "OK... Restarting in 5 seconds");
                delay(5000);
                ESP.restart();
            }
            else{
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/html", "Option not allowed.");
            }
        },
        []()
        {
            HTTPUpload &upload = httpServer.upload();
            if (upload.status == UPLOAD_FILE_START)
            {
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                { //start with max available size
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE)
            {
                /* flashing firmware to ESP*/
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END)
            {
                if (Update.end(true))
                { //true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else
                {
                    Update.printError(Serial);
                }
            }
        });
    httpServer.begin();
    Serial.println("HTTP server started! Open your browser and go to http://" + globalIP + "/about");
}

// return the about page as HTML.
void handleAbout()
{
    String aboutResponse = "<body style=\"background-color:#3498db;color:#ffffff;font-family:arial\"><b>[About ESP32]</b><br><br>";
    aboutResponse += "Device Family: " + deviceFamily + "<br>";
    aboutResponse += "ESP Chip Model: " + String(ESP.getChipModel()) + "<br>";
    aboutResponse += "CPU Frequency: " + String(ESP.getCpuFreqMHz()) + "<br>";
    aboutResponse += "Free Heap Mem: " + String(ESP.getFreeHeap()) + "<br>";
    aboutResponse += "Flash Mem Size: " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB<br>";
    aboutResponse += "Hostname: " + hostName + "<br>";
    aboutResponse += "IPAddress: " + globalIP + "<br>";
    aboutResponse += "MAC Address: " + String(WiFi.macAddress()) + "<br>";
    aboutResponse += "Software Version: " + softwareVersion + "<br>";
    aboutResponse += "SSID: " + ssid + "<br>";
    aboutResponse += "Description: " + description + "<br>";
    aboutResponse += "Update: http://" + hostName + ".ra.local/update<br><br>";
    aboutResponse += "<button onclick=\"window.location.href='/restart'\">Restart</button></body>";
    aboutResponse += "&nbsp;&nbsp;<button onclick=\"window.location.href='/serverIndex'\">Update</button></body>";
    httpServer.send(200, "text/html", aboutResponse);
    aboutResponse.clear();
}
