#define USE_ARDUINO_INTERRUPTS false
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

const int GRNLED 		= 	25; 				// Green LED for testing
const int REDLED 		= 	26; 				// Red LED for testing
const int FADETIME 		= 	5; 					// LED Fade Time
const int BAUDRATE 		= 	9600; 				// Set Baudrate
const int PulseSensor	= 	35;  				// Pulse Sensor Pin Definition
const int AHT20_SDA  	= 	21; 				// AHT20 SDA Pin Definition
const int AHT20_SCL 	= 	22; 				// AHT20 SCL Pin Definition
const int GSR 			= 	38; 				// GSR Signal Pin Definition
int Threshold			= 	1750; 				// Initialise Heart Rate Threshold
int GSRRead 			= 	0; 					// Initialise GSR Value
int Temperature			= 	0; 					// Initialise Temperature Variable
int Humidity			= 	0; 					// Initialise Humidity Variable
int Signal				= 	0; 					// Initialise Heart Rate Signal Variable
const byte 	SAMPLES_PER_SERIAL_SAMPLE = 10;

// Set up Pulse Sensor
PulseSensorPlayground pulseSensor;

String PulseReading() { 						// return string of pulse sensor information
	Signal = pulseSensor.getBeatsPerMinute();
	Serial.print("Pulse BPM:   ");
	Serial.print(String(Signal));
	Serial.print('\n');
	return String(Signal);
}

String AHTTempReading() { 						// return string of temp sensor information
	sensors_event_t humidity, temp; 			// Create temp and humidity objects
	aht.getEvent(&humidity, &temp); 			// populate temp and humidity objects with fresh data
	Temperature = temp.temperature;
	Serial.print("Temperature: ");
	Serial.print(String(Temperature));
	Serial.print('\n');
	return String(Temperature);
}

String AHTHumidityReading() { 					// return string of humidity sensor information
	sensors_event_t humidity, temp; 			// Create temp and humidity objects
	aht.getEvent(&humidity, &temp); 			// populate temp and humidity objects with fresh data
	Humidity = humidity.relative_humidity;
	Serial.print("Humidity:    ");
	Serial.print(String(Humidity));
	Serial.print('\n');
	return String(Humidity);
}

String GSRReading() { 							// return string of GSR sensor information
	GSRRead = analogRead(GSR);
	Serial.print("GSR Value:   ");
	Serial.print(String(GSRRead));
	Serial.print('\n');
	return String(GSRRead);
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
		Serial.println("Connecting to WiFi..");
		digitalWrite(REDLED, LOW);
		delay(50);
		digitalWrite(REDLED, HIGH);
		delay(50);
	}
	Serial.println(WiFi.localIP());

	// Set up AHT20
	while (! aht.begin(&Wire)) {
		Serial.println("Could not find AHT? Check wiring");
		digitalWrite(REDLED, LOW);
		delay(50);
		digitalWrite(REDLED, HIGH);
		delay(50);
		return;
	}
	Serial.println("AHT20 found");

	// Set up Pulse Sensor
	pulseSensor.analogInput(PulseSensor);
	pulseSensor.blinkOnPulse(GRNLED);
	pulseSensor.fadeOnPulse(FADETIME);
	pulseSensor.setThreshold(Threshold);
	while (!pulseSensor.begin()){
		Serial.println("PulseSensor not found");
		// Flash the led to show things didn't work.
		digitalWrite(REDLED, LOW);
		delay(50);
		digitalWrite(REDLED, HIGH);
		delay(50);
	}
	Serial.println("PulseSensor Found");



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
	delay(20);
	pulseSensor.outputSample();

	if (pulseSensor.sawStartOfBeat()) {
		pulseSensor.outputBeat();
		Serial.println("Beat");
	}
}
