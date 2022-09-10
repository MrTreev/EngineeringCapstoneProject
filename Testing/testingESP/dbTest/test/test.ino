
#include <WiFi.h>
//#include <ESPAsyncWebServer.h>
//#include <SPIFFS.h>
  #include <HTTPClient.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
const char* ssid = "TP-LINK_3E17";
const char* password = "61967274";
//AsyncWebServer server(80);
int i = 0;

//Adafruit_AHTX0 aht;
//TwoWire AHTI2C = TwoWire(0);

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
int threshSetting = 3000;
int thresh = 3000;
int amp = 410;
bool firstBeat = true;
bool secondBeat = false;
int FadeLevel = 0;
int delayMs = 20;
int rate[10];

const char* serverName = "http://your-mindshift.com/post-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorReading() { // return string of sensor information
  int voltageReading = analogRead(38);
  voltage = voltageReading * (vs/4095);
  return String(voltage);
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

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP()); // This will print out local ip address for the ESP32
//  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(SPIFFS, "/index.html"); // Sends HTML file in SPIFFS when server receives a request on root URL
//  });
//  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/plain", sensorReading().c_str()); //Call sensor reading function when server receives a request on /sensor URL
//  });
//  server.on("/sensor2", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/plain", sensorReading2().c_str()); //Call sensor2 reading function when server receives a request on /sensor2 URL
//  });
//  server.on("/sensor3", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/plain", sensorReading3().c_str()); //Call sensor3 reading function when server receives a request on /sensor3 URL
//  });
  // Start the server

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
    String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(sensorReading())
                           + "&value2=" + String(sensorReading2()) + "&value3=" + String(sensorReading3()) + "" + "&value3=" + "Alec";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");

    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }

    sampleCounter = sampleCounter + delayMs; // keeping track of time in ms
  int N = sampleCounter - lastBeatTime;

  Signal = analogRead(35);
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
  
  //Send an HTTP POST request every 30 seconds
  delay(30000);
}
