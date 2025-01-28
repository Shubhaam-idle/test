#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

// Wi-Fi credentials
const char* ssid = "SK";
const char* password = "";

// URLs for firmware binary and version file
const char* firmware_url = "https://raw.githubusercontent.com/Shubhaam-idle/test/main/wb.bin";
const char* version_url = "https://raw.githubusercontent.com/Shubhaam-idle/test/refs/heads/main/wv.txt";

// Current firmware version
const String current_version = "2.0";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  checkForUpdate();
}

void loop() {
  // Placeholder for main loop tasks
  Serial.println("Running...");
  delay(10000);
}

void checkForUpdate() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Check firmware version
    http.begin(version_url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      String new_version = http.getString();
      new_version.trim(); // Remove any trailing whitespace

      Serial.print("Current Version: ");
      Serial.println(current_version);
      Serial.print("Available Version: ");
      Serial.println(new_version);

      if (new_version != current_version) {
        Serial.println("New firmware version detected. Starting OTA update...");
        performOTAUpdate();
      } else {
        Serial.println("Firmware is up to date.");
      }
    } else {
      Serial.printf("Failed to check version. HTTP Code: %d\n", httpCode);
    }

    http.end();
  } else {
    Serial.println("Not connected to WiFi.");
  }
}

void performOTAUpdate() {
  HTTPClient http;
  http.begin(firmware_url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      Serial.println("Starting OTA update...");
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
        Serial.printf("Update failed. Error: %d\n", Update.getError());
      }
    } else {
      Serial.println("Not enough space for OTA update!");
    }
  } else {
    Serial.printf("HTTP GET failed for firmware with code: %d\n", httpCode);
  }

  http.end();
}
