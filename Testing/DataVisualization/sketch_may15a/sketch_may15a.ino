#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <stdlib.h>  
#include <string> 
const char* ssid = "Telstra1A50C5";
const char* password = "abqnwhgznt";
AsyncWebServer server(80);

String sensorReading() { // return string of sensor information
  return String(random(100));
 }

void setup(){
  Serial.begin(9600);
  if(!SPIFFS.begin()){
    Serial.println("Make sure to sketch upload!");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", sensorReading().c_str());
  });
  // Start server
  server.begin();
}
 
void loop(){
  
}
