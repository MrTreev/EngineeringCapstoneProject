#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
#include <Adafruit_AHTX0.h>
const char* ssid = "TP-LINK_3E17";
const char* password = "61967274";
AsyncWebServer server(80);

Adafruit_AHTX0 aht;
TwoWire AHTI2C = TwoWire(0);

float voltage = 0;
float vs = 3.3;

bool QS = false;
int BPM = 0;
int IBI = 750;
bool Pulse = false;
int sampleCounter = 0;
int lastBeatTime = 0;
int P = 2048;
int T = 2048;
int threshSetting = 2400;
int thresh = 2400;
int amp = 410;
bool firstBeat = true;
bool secondBeat = false;
int FadeLevel = 0;
int delayMs = 20;
int rate[10];

String sensorReading() { // return string of sensor information
  //return String(random(100));

  int voltageReading = analogRead(38);
  voltage = voltageReading * (vs/4095);
  return String(voltage);
 }

String sensorReading2() { // return string of sensor information
  int Signal = analogRead(0);
  return String(Signal);
 }

String sensorReading3() { // return string of sensor information
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  
  return String(temp.temperature);
 }
 
void setup(){
  Serial.begin(9600);
  if(!SPIFFS.begin()){
    //Serial.println("Make sure to sketch upload!");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    //Serial.println("Connecting to WiFi..");
  }

  //Serial.println(WiFi.localIP());

  if (! aht.begin(&AHTI2C)) {
    //Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  //Serial.println("AHT10 or AHT20 found");

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
