#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Buehl-privat"
#define WLAN_PASS       "HidsOdW377"
#define LED             2

/************************* Server config *************************************/

#define AUTH_TOKEN      "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1OTRkOTkyYjFiYWY0ODQxYTc2MjM1NTQxYjZjOGFjNiIsImlhdCI6MTczNDg3ODc0MSwiZXhwIjoyMDUwMjM4NzQxfQ.LVLxtrLHgMr-uP7unnypTlyyFaNTIyvT78yvUqKKWJM"
#define HOST            "192.168.178.104"
#define PORT            8123
#define URL_PATH        "/api/services/light/turn_on"
#define CONTENT_TYPE    "application/json"
#define POST_DATA       "{\"entity_id\": \"light.zirkulation_relais_zirkulationspumpe\"}"

/************************** Global State *************************************/

WiFiClient client;

void sendPostRequest();
void blinkError();
void shutdown();

/*************************** Sketch Code ************************************/

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.println("Initializing WiFi connection...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  int failCounter = 300;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    Serial.println(failCounter);
    if (--failCounter <= 0) {
      Serial.println("\nFailed to connect to WiFi.");
      while (true) {
        digitalWrite(LED, HIGH);
        delay(200);
        digitalWrite(LED, LOW);
        delay(200);
      }
    }
  }

  Serial.println("\nWiFi connected.");
  digitalWrite(LED, HIGH);

  sendPostRequest();

  shutdown();
}

void loop() {
  // Nothing to do here
}

void sendPostRequest() {
  if (!client.connect(HOST, PORT)) {
    Serial.println("Failed to connect to server.");
    blinkError();
    return;
  }

  Serial.println("Connected to server. Sending POST request...");

  // Construct the HTTP POST request
  client.print(String("POST ") + URL_PATH + " HTTP/1.1\r\n" +
               "Host: " + HOST + "\r\n" +
               "Authorization: " + AUTH_TOKEN + "\r\n" +
               "Content-Type: " + CONTENT_TYPE + "\r\n" +
               "Content-Length: " + String(strlen(POST_DATA)) + "\r\n" +
               "Connection: close\r\n\r\n" +
               POST_DATA);

  // Wait for the server's response
  while (client.connected() || client.available()) {
    if (client.available()) {
      String response = client.readStringUntil('\n');
      Serial.println(response);
    }
  }

  Serial.println("POST request sent and response received.");
  client.stop();
}

void blinkError() {
  for (int i = 0; i < 10; ++i) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}


void shutdown() {
  Serial1.println("Shutting down.");
  Serial1.println("Going to sleep.");
  ESP.deepSleep(0);
  Serial1.println("Sleep failed.");
  while(1) {
    blinkError();
  }
}
