#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
const char* ssid = "DaBoisWireless_2G";
const char* password = "dungyreamyxmPgG";
AsyncWebServer server(80);

int BAUDRATE 		= 	9600; 					// Set Baudrate
int PulseSensor		= 	0;  					// Pulse Sensor Pin Definition
int AHT20_SDA  		= 	34; 					// AHT20 SDA Pin Definition
int AHT20_SCL 		= 	35; 					// AHT20 SCL Pin Definition
int GSR 		= 	38; 					// GSR Signal Pin Definition
int analogInput		= 	38; 					// Initialise Analog Input Variable
bool deviceConnected 	= 	false;					// Initialise Connected Device Bool
uint32_t value 		= 	0;    					// Initialise Counter Value

String sensorReading() { // return string of sensor information
  return String(random(100));
 }

void setup(){
  Serial.begin(BAUDRATE);
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
