#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
const char* ssid = "SSID";
const char* password = "password";
AsyncWebServer server(80);

String sensorReading() { // return string of sensor information
  return String(random(100));
 }

String sensorReading2() { // return string of sensor information
  return String(random(100));
 }

String sensorReading3() { // return string of sensor information
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
  server.on("/sensor2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", sensorReading2().c_str());
  });
  server.on("/sensor3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", sensorReading3().c_str());
  });
  // Start server
  server.begin();
}

void loop(){

}
