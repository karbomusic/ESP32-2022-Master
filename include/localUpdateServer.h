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
extern String ssid;                  // WiFi ssid.
extern String password;              // WiFi password.
extern String hostName;              // hostname as seen on network.
extern String softwareVersion;       // used for auto OTA updates & about page.
extern String deviceFamily;          // used for auto OTA updates & about page.
extern String description;           // used for about page.
extern String globalIP;              // needed for about page.
extern const String metaRedirect;    // needed for restart redirect.
extern const int activityLED;

WebServer httpServer(80);

// Prototypes
void handleAbout();
void bangLED(int);
void handleRestart();
String getUpdateHTML();

// Start the server
void startUpdateServer()
{
    Serial.println("mDNS responder started");

    // handlers for url paths
    httpServer.on("/", HTTP_GET, []()
                  { handleAbout(); });
    httpServer.on("/about", handleAbout);
    httpServer.on("/restart", handleRestart);
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
            bangLED(HIGH);
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
            bangLED(LOW);
        });
    httpServer.begin();
    Serial.println("HTTP server started! Open your browser and go to http://" + globalIP + "/about");
}

// return the about page as HTML.
void handleAbout()
{
    bangLED(HIGH);
    String aboutResponse;
    aboutResponse.reserve(1024);
    aboutResponse = "<body style=\"background-color:#3498db;color:#ffffff;font-family:arial\"><b>[About ESP32]</b><br><br>";
    aboutResponse += "<b>Device Family:</b> " + deviceFamily + "<br>";
    aboutResponse += "<b>ESP Chip Model:</b> " + String(ESP.getChipModel()) + "<br>";
    aboutResponse += "<b>CPU Frequency:</b> " + String(ESP.getCpuFreqMHz()) + "<br>";
    aboutResponse += "<b>Free Heap Mem:</b> " + String(ESP.getFreeHeap()) + "<br>";
    aboutResponse += "<b>Flash Mem Size:</b> " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB<br>";
    aboutResponse += "<b>Hostname:</b> " + hostName + "<br>";
    aboutResponse += "<b>IPAddress:</b> " + globalIP + "<br>";
    aboutResponse += "<b>MAC Address:</b> " + String(WiFi.macAddress()) + "<br>";
    aboutResponse += "<b>Software Version:</b> " + softwareVersion + "<br>";
    aboutResponse += "<b>SSID:</b> " + ssid + "<br>";
    aboutResponse += "<b>Description:</b> " + description + "<br>";
    aboutResponse += "<b>Uptime:</b> " + String(millis()/1000/60) + " minutes<br>";
    aboutResponse += "<b>Update:</b> http://" + hostName + ".ra.local/update<br><br>";
    aboutResponse += "<button onclick=\"window.location.href='/restart'\">Restart</button></body>";
    aboutResponse += "&nbsp;&nbsp;<button onclick=\"window.location.href='/update'\">Update</button></body>";
    httpServer.send(200, "text/html", aboutResponse);
    httpServer.sendHeader("Connection", "close");
    bangLED(LOW);
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

void handleRestart()
{
    Serial.println("Restarting in 5 seconds...");
    httpServer.send(200, "text/html", metaRedirect);
    delay(5000);
    ESP.restart();
}

void bangLED(int state)
{
    digitalWrite(activityLED, state);
}
