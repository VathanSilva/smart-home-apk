#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "";           // Replace with your WiFi SSID
const char* password = "";   // Replace with your WiFi password

const char* firestoreUrl = "https://firestore.googleapis.com/v1/projects/{projectID}/databases/(default)/documents/users/{userID}/homes/{homeID}/rooms/{roomID}/devices/{deviceID}?key={API Key}";

const int ledPin = 4; // GPIO pin for the LED
String lastState = ""; // To store the last state

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Ensure LED is off initially

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(firestoreUrl);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      // Parse JSON response
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        const char* state = doc["fields"]["state"]["stringValue"];
        Serial.print("Device state: ");
        Serial.println(state);

        // Check if the state has changed
        if (lastState != state) {
          lastState = state;

          if (strcmp(state, "on") == 0) {
            digitalWrite(ledPin, HIGH);
          } else if (strcmp(state, "off") == 0) {
            digitalWrite(ledPin, LOW);
          }
        }
      } else {
        Serial.print("JSON parse error: ");
        Serial.println(error.f_str());
      }
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(1000); // Check the state every 10 seconds
}
