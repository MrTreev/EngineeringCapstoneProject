#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>

//Parameters
	#define FILTER_LEN 100
	const char* ssid		= "DaBoisWireless_2G";
	const char* password	= "dungyreamyxmPgG";
	const char* user		= "oliver";
	const int PIN_Heart		= 37;
	const int PIN_GSR		= 34;
	const int PIN_SDA		= 21; // Does not change with assignment
	const int PIN_SCL		= 22; // Does not change with assignment

//	Web
	const char* serverName 	= "http://your-mindshift.com/post-data.php";
	String apiKeyValue 		= "tPmAT5Ab3j7F9";

//	GSR
	float voltage 		= 0;
	float vs 			= 3.3;
	float skinResponse 	= 0;
	int count 			= 0;
	uint32_t filterBuffer[FILTER_LEN] = {0};

//	Heart
	bool Pulse 			= false;
	bool firstBeat 		= true;
	bool secondBeat 	= false;
	bool QS 			= false;
	int IBI 			= 750;
	int P 				= 2048;
	int T 				= 2048;
	int threshSetting 	= 3000;
	int thresh 			= 3000;
	int amp 			= 410;
	int FadeLevel 		= 0;
	int delayMs 		= 20;
	int rate[10];

//	Temperature
	//Adafruit_AHTX0 aht;
	//TwoWire AHTI2C = TwoWire(0);

//Initialisations
int Signal 			= 0;
int i 				= 0;
int sampleCounter 	= 0;
int lastBeatTime 	= 0;
int BPM 			= 0;



int sensorReading() { // return string of sensor information
  int voltageReading = analogRead(PIN_GSR);
  voltage = voltageReading * (vs/4095);
  skinResponse = ((voltage - 1.75) * -1) * 1000;
  return skinResponse;
 }

String sensorReading2() { // return string of sensor information
  return String(BPM); // placeholder value
 }

String sensorReading3() { // return string of sensor information
  //sensors_event_t humidity, temp;
  //aht.getEvent(&humidity, &temp);
  return String(1); // placeholder value
 }
void setup(){
  Serial.begin(9600);
// Power
	pinMode(35, OUTPUT);
	digitalWrite(3:, HIGH);
	pinMode(38, OUTPUT);
	digitalWrite(38, HIGH);


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
 //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(filtered(sensorReading())) + "&value2=" + String(sensorReading2()) + "&value3=" + String(sensorReading3()) + "" + "&user=" + user;
    int httpResponseCode = http.POST(httpRequestData);
    Serial.println("&value1=" + String(filtered(sensorReading())) + "&value2=" + String(sensorReading2()) + "&value3=" + String(sensorReading3()));
    if (httpResponseCode>0) {
    }
    else {
    }
    http.end();
  }
  else {
  }

    sampleCounter = sampleCounter + delayMs; // keeping track of time in ms
  int N = sampleCounter - lastBeatTime;

  Signal = analogRead(PIN_Heart);
  Serial.println(Signal);

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

  delay(500);
}

uint32_t filtered(int ADC_Raw)
{
  int i = 0;
  uint32_t Sum = 0;

  filterBuffer[count++] = ADC_Raw;
  if(count == FILTER_LEN){
    count = 0;
    }
  for (i = 0; i < FILTER_LEN; i++){
    Sum += filterBuffer[i];
    }

  uint32_t result = Sum/FILTER_LEN;
  if(result>5000){
    return 0;
    }
  return (Sum/FILTER_LEN);
}
