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

WebServer httpServer(80);

// Prototypes
void handleAbout();
void bangLED(int);
String getUpdateHTML();

// Start the server
void startUpdateServer()
{
    Serial.println("mDNS responder started");

    // handlers for url paths
    httpServer.on("/", HTTP_GET, []()
                  { handleAbout(); });
    httpServer.on("/about", handleAbout);
    httpServer.on(
        "/update", HTTP_ANY, []()
        {
            bangLED(HIGH);
            if (httpServer.method() == HTTP_GET)
            {
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/html", getUpdateHTML());
            }
            else if (httpServer.method() == HTTP_POST)
            {
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/plain", (Update.hasError()) ? "FAIL... Restarting in 5 seconds" : "OK... Restarting in 5 seconds");
                delay(5000);
                ESP.restart();
            }
            else
            {
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/html", "Option not allowed.");
            }
            digitalWrite(LED_BUILTIN, LOW);
            bangLED(LOW);
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
    aboutResponse += "&nbsp;&nbsp;<button onclick=\"window.location.href='/update'\">Update</button></body>";
    httpServer.send(200, "text/html", aboutResponse);
    httpServer.sendHeader("Connection", "close");
    aboutResponse.clear();
}

// update.html must have previously been uploaded to the SPIFFs partition via Arduino IDE
// before uploading this sketch. This is only needed once per device.
String getUpdateHTML()
{
    String updateHTML = "";
    File file = SPIFFS.open("/update.html");
    if (file && file.available() && file.size() > 0)
    {
        updateHTML = file.readString();
    }
    else
    {
        updateHTML = "Failed to open /update.html for reading. Update.html must be uploaded to SPIFFs partition before uploading this sketch.";
        Serial.println(updateHTML);
    }
    file.close();
    return updateHTML;
}

void bangLED(int state)
{
    if (state == HIGH)
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
}
