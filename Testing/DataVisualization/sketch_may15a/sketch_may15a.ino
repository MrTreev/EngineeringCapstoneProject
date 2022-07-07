#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
#include <Adafruit_AHTX0.h>
const char* ssid = "DaBoisWireless_2G";
const char* password = "dungyreamyxmPgG";
AsyncWebServer server(80);

Adafruit_AHTX0 aht;
TwoWire AHTI2C = TwoWire(0);

float voltage = 0;
float vs = 3.3;
int Signal = 0;
bool QS = false;
int BPM = 0;
int IBI = 750;
bool Pulse = false;
int sampleCounter = 0;
int lastBeatTime = 0;
int P = 2048;
int T = 2048;
int threshSetting = 2000;
int thresh = 2000;
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
  return String(BPM);
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
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  if (! aht.begin(&AHTI2C)) {
    Serial.println("Could not find AHT? Check wiring");
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

void loop() {
  sampleCounter = sampleCounter + delayMs; // keeping track of time in ms
  int N = sampleCounter - lastBeatTime;

  Signal = analogRead(35);

  if (N > 250 && N < 3000){
    if (Signal > thresh){
      Pulse = true;
      IBI = sampleCounter - lastBeatTime;

      word runningTotal = 0;

      for (int i = 0; i <= 8; i++) {          // shift data in the rate array
        rate[i] = rate[i + 1];                // and drop the oldest IBI value
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values
      BPM = 60000 / runningTotal;             // how many beats can fit into a minute? that's BPM!

      lastBeatTime = sampleCounter;
    }
  }

  if (N > 3000) {                          // if 3 seconds go by without a beat
    thresh = threshSetting;                // set thresh default
    P = 2048;                               // set P default
    T = 2048;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
    Pulse = false;
    BPM = 0;
    IBI = 600;                  // 600ms per beat = 100 Beats Per Minute (BPM)
    Pulse = false;
    amp = 410;                  // beat amplitude 1/10 of input range.
  }
  delay(delayMs);
}
