#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID "Buehl-privat"
#define WLAN_PASS "HidsOdW377"
#define LED 2

/************************* Server config *************************************/

#define AUTH_TOKEN "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1OTRkOTkyYjFiYWY0ODQxYTc2MjM1NTQxYjZjOGFjNiIsImlhdCI6MTczNDg3ODc0MSwiZXhwIjoyMDUwMjM4NzQxfQ.LVLxtrLHgMr-uP7unnypTlyyFaNTIyvT78yvUqKKWJM"
#define HOST "192.168.178.104"
#define PORT 8123
#define URL_PATH "/api/services/light/turn_on"
#define CONTENT_TYPE "application/json"
#define POST_DATA "{\"entity_id\": \"light.zirkulation_relais_zirkulationspumpe\"}"

/************************** Global State *************************************/

WiFiClient client;

void sendPostRequest();
void blinkError();
void blinkSuccess();
void shutdown();

void connectionFailed()
{
  Serial.println("\nFailed to connect to WiFi.");
  blinkError();
  Serial.println("Aborting.");
  shutdown();
}

void connectToWiFi()
{
  Serial.println("Initializing WiFi connection...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  int failCounter = 60;
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED, failCounter % 2 == 0 ? HIGH : LOW);
    delay(500);
    Serial.print(".");
    Serial.println(failCounter);
    if (--failCounter <= 0)
    {
      connectionFailed();
    }
  }

  Serial.println("\nWiFi connected.");
}

/*************************** Sketch Code ************************************/

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  connectToWiFi();
  digitalWrite(LED, HIGH);

  sendPostRequest();
  shutdown();
}

void loop()
{
  // Nothing to do here
}

void sendPostRequest()
{
  if (!client.connect(HOST, PORT))
  {
    Serial.println("Failed to connect to server.");
    blinkError();
    shutdown();
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
  unsigned long timeout = millis();
  while (!client.available())
  {
    if (millis() - timeout > 5000)
    { // Wait up to 5 seconds
      Serial.println("Server response timeout.");
      blinkError();
      client.stop();
      return;
    }
  }

  Serial.println("Server response received:");

  // Read and print the entire response
  String response = "";
  while (client.available())
  {
    char c = client.read();
    response += c;
  }
  Serial.println(response);

  // Extract the HTTP status code
  int statusCodeStart = response.indexOf("HTTP/1.1") + 9;
  if (statusCodeStart > 8)
  { // Ensure "HTTP/1.1" was found
    int statusCode = response.substring(statusCodeStart, statusCodeStart + 3).toInt();
    Serial.println("HTTP Status Code: " + String(statusCode));
    if (statusCode >= 200 && statusCode < 300)
    {
      Serial.println("HTTP request successful.");
      blinkSuccess();
    }
    else
    {
      Serial.println("HTTP request failed with status: " + String(statusCode));
      blinkError();
    }
  }
  else
  {
    Serial.println("Invalid HTTP response.");
    blinkError();
  }

  client.stop();
}

void blinkSuccess()
{
  digitalWrite(LED, LOW); // turn the LED on
  delay(2000);            // wait
}

void blinkError()
{
  for (int i = 0; i < 28; i++)
  {
    digitalWrite(LED, HIGH); // turn the LED off
    delay(125);              // wait
    digitalWrite(LED, LOW);  // turn the LED on
    delay(125);              // wait
  }
}

void shutdown()
{
  Serial1.println("Shutting down.");
  Serial1.println("Going to sleep.");
  ESP.deepSleep(0);
  Serial1.println("Sleep failed.");
  while (1)
  {
    blinkError();
  }
}
