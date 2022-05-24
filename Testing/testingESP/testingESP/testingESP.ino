#include <PulseSensorPlayground.h>
#include <Adafruit_AHTX0.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
const char* ssid = "DaBoisWireless_2G";
const char* password = "dungyreamyxmPgG";
Adafruit_AHTX0 aht;
AsyncWebServer server(80);

int BAUDRATE 		= 	115200; 				// Set Baudrate
int PulseSensor		= 	4;  					// Pulse Sensor Pin Definition
int AHT20_SDA  		= 	34; 					// AHT20 SDA Pin Definition
int AHT20_SCL 		= 	35; 					// AHT20 SCL Pin Definition
int GSR 		= 	38; 					// GSR Signal Pin Definition
int Temperature		= 	0; 					// Initialise Temperature Variable
int Humidity		= 	0; 					// Initialise Humidity Variable
int Signal		= 	0; 					// Initialise Heart Rate Signal Variable
int Threshold		= 	1880; 					// Initialise Heart Rate Threshold
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;
PulseSensorPlayground pulseSensor;


String PulseReading() { 						// return string of pulse sensor information
	pulseSensor.analogInput(PulseSensor);
	Signal = pulseSensor.getBeatsPerMinute();
	return String(Signal);
}

String AHTTempReading() { 						// return string of temp sensor information
	sensors_event_t humidity, temp; 				// Create temp and humidity objects
	aht.getEvent(&humidity, &temp); 				// populate temp and humidity objects with fresh data
	Temperature = temp.temperature;
	return String(Temperature);
}

String AHTHumidityReading() { 						// return string of humidity sensor information
	sensors_event_t humidity, temp; 				// Create temp and humidity objects
	aht.getEvent(&humidity, &temp); 				// populate temp and humidity objects with fresh data
	Humidity = humidity.relative_humidity;
	return String(Humidity);
}

String GSRReading() { 							// return string of GSR sensor information
	return String(analogRead(GSR));
}


void setup(){
	Serial.begin(BAUDRATE);

	Wire.begin(AHT20_SDA, AHT20_SCL);

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


	while (! aht.begin(&Wire)) {
		Serial.println("Could not find AHT? Check wiring");
		delay(1000);
		return;
	}
	Serial.println("AHT10 or AHT20 found");


	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/index.html");
	});
	server.on("/PulseSensor", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", PulseReading().c_str());
	});
	server.on("/AHTTemp", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", AHTTempReading().c_str());
	});
	server.on("/AHTHumidity", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", AHTHumidityReading().c_str());
	});
	server.on("/GSR", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", GSRReading().c_str());
	});
	// Start server
	server.begin();
}

void loop(){

}
