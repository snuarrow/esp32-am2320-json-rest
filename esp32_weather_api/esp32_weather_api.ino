#include "WiFi.h"
#include "ArduinoJson.h"
#include "WebServer.h"
#include <AM2320.h>

const char* ssid = "---";
const char* password = "---";
const char* sensorId = "---";
char i = 0;
const int ledPin = 2;
long resetti = 0;

WebServer server(80);

AM2320 th;

StaticJsonDocument<250> jsonDocument;
char buffer[250];

void blink_led(int times)
{
  int j = 0;
  for (j; j < times; j++)
  {
    digitalWrite(ledPin, HIGH);
    delay(2);
    digitalWrite(ledPin, LOW);
    delay(150);
  }
}

bool connect_to_wifi()
{
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ledPin, LOW);
    return true;
  }

  digitalWrite(ledPin, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int j = 0;
  for (j; j < 60; j++) {
    Serial.println("Connecting to WiFi..");
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(ledPin, LOW);
      Serial.println("Connected to the Wifi network");
      return true;
    }
    delay(1000);
  }
  Serial.println("restart");
  ESP.restart();
  return false;
}

void create_json()
{
  jsonDocument.clear();
  jsonDocument["sensorId"] = sensorId;
  jsonDocument["temperature"] = th.getTemperature();
  jsonDocument["humidity"] = th.getHumidity();
  serializeJson(jsonDocument, buffer);
}

void getStatus()
{
  Serial.println("Get Status");
  create_json();
  server.send(200, "application/json", buffer);
  blink_led(3);
}

void setup_routing()
{
  server.on("/", getStatus);
  server.begin();
}

void setup()
{
  pinMode(ledPin, OUTPUT);
  blink_led(12);
  digitalWrite(ledPin, HIGH);
  Serial.begin(9600);
  th.begin();
  connect_to_wifi();
  setup_routing();
}

void loop()
{
  th.measure();
  server.handleClient();
  delay(100);
  if (i++ > 100)
  {
    i = 0;
    Serial.println(th.getTemperature());
    Serial.println(WiFi.localIP());
    connect_to_wifi();
    blink_led(1);
  }

  if (resetti++ > 36000)
  {
    resetti = 0;
    Serial.println("restart");
    ESP.restart();
  }
}
