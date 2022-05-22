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
int Temperature		= 	0; 					// Initialise Temperature Variable
int Humidity		= 	0; 					// Initialise Humidity Variable

String PulseReading() { // return string of sensor information
	return String(digitalRead(PulseSensor));
}

String AHT-TempReading() { // return string of sensor information
	return String(Temperature);
}

String AHT-HumidityReading() { // return string of sensor information
	return String(Humidity);
}

String GSRReading() { // return string of sensor information
	return String(analogRead(GSR));
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
	server.on("/PulseSensor", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", PulseReading().c_str());
	});
	server.on("/AHT-Temp", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", AHT-TempReading().c_str());
	});
	server.on("/AHT-Humidity", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", AHT-HumidityReading().c_str());
	});
	server.on("/GSR", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", GSRReading().c_str());
	});
	// Start server
	server.begin();
}

void loop(){

}
