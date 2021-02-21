#include "WiFi.h"
#include "ArduinoJson.h"
#include "WebServer.h"
#include <AM2320.h>

const char* ssid = "---";
const char* password = "---";
const char* sensorId = "0";

WebServer server(80);

AM2320 th(&Wire);

StaticJsonDocument<250> jsonDocument;
char buffer[250];

void connect_to_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while ( WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
}

void create_json() {
  read_sensor();  
  jsonDocument.clear();  
  jsonDocument["sensorId"] = sensorId;
  jsonDocument["temperature"] = th.cTemp;
  jsonDocument["humidity"] = th.Humidity;
  serializeJson(jsonDocument, buffer);
}

void getStatus() {
    Serial.println("Get Status");
    create_json();
    server.send(200, "application/json", buffer);
}

void setup_routing() {
    server.on("/", getStatus);
    server.begin();
}

void read_sensor() {
   while (th.Read() == 0) {
   }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  connect_to_wifi();
  setup_routing();

}

void loop() {
  server.handleClient();
  delay(500);
  read_sensor();
  Serial.println(th.cTemp);
  Serial.println(WiFi.localIP());
}
