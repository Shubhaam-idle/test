#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "C15";           // Replace with your WiFi SSID
const char* password = "12345678";   // Replace with your WiFi Password
const char* firmware_url = "https://raw.githubusercontent.com/Shubhaam-idle/test/main/fw.bin";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
  ot();
}

void loop()
 {
 Serial.println("2");
}


void ot()
{
   if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http;
    http.begin(firmware_url); // Specify the URL of the firmware

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      int contentLength = http.getSize();
      bool canBegin = Update.begin(contentLength);

      if (canBegin) {
        Serial.println("Begin OTA update...");
        size_t written = Update.writeStream(http.getStream());

        if (written == contentLength) {
          Serial.println("OTA update complete!");
        } else {
          Serial.println("OTA update failed!");
        }

        if (Update.end()) {
          Serial.println("Update successfully completed. Restarting...");
          ESP.restart();
        } else {
          Serial.printf("Update failed. Error #%d\n", Update.getError());
        }
      } else {
        Serial.println("Not enough space for OTA update!");
      }
    } else {
      Serial.printf("HTTP GET failed with code %d\n", httpCode);
    }

    http.end();
  }
}