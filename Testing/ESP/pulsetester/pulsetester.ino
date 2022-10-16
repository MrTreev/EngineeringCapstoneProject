#include <stdlib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <string>

int BAUDRATE = 9600;
int Signal = 0;
bool QS = false;
int BPM = 0;
int IBI = 750;
bool Pulse = false;
int sampleCounter = 0;
int lastBeatTime = 0;
int P = 2048;
int T = 2048;
int threshSetting = 2300;
int thresh = 2300;
int amp = 410;
bool firstBeat = true;
bool secondBeat = false;
int FadeLevel = 0;
int delayMs = 20;
int rate[10];

float voltage = 0;
float vs = 3.3;
#define FILTER_LEN 100
float skinResponse = 0;
float filteredSkinResponse = 0;
int count = 0;
uint32_t filterBuffer[FILTER_LEN] = {0};

const char* ssid		= "DaBoisWireless_2G";
const char* password	= "dungyreamyxmPgG";
const char* serverName = "http://your-mindshift.com/post-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";

TaskHandle_t Task1;
TaskHandle_t Task2;

void setup(){
  Serial.begin(BAUDRATE);
	pinMode(35, OUTPUT);
	digitalWrite(35, HIGH);
	pinMode(38, OUTPUT);
	digitalWrite(38, HIGH);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    //Serial.println("Connecting to WiFi...");
  }

  xTaskCreatePinnedToCore(
    getData,
    "Task1",
    10000,
    NULL,
    0,
    &Task1,
    0);

  xTaskCreatePinnedToCore(
    checkWiFi,
    "Task2",
    10000,
    NULL,
    0,
    &Task1,
    1);
}

void loop(){}

void getData(void * pvParamets){
  for(;;){
    Serial.print(getGSR());
    Serial.print(" ");
    Serial.print(getBPM());
    Serial.print(" ");
    Serial.println(Signal);

    setBPM();
    setGSR();
    delay(10);
  }
}

String getGSR(){
  return String(filteredSkinResponse);
}

String getBPM(){
  return String(BPM);
}

void checkWiFi(void * pvParameters) {

  for(;;){
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Prepare your HTTP POST request data
      String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(getGSR())
                             + "&value2=" + String(getBPM()) + "&value3=" + String(String(0)) + "" + "&user=" + "oliver";
      //Serial.print("httpRequestData: ");
      //Serial.println(httpRequestData);

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
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
      }
      else {
        //Serial.print("Error code: ");
        //Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      //Serial.println("WiFi Disconnected");
    }
    delay(1000);
  }
}

void setGSR(){
  int voltageReading = analogRead(34);
  voltage = voltageReading * (vs/4095);
  skinResponse = ((voltage - 1.75) * -1) * 1000;

  int i = 0;
  uint32_t Sum = 0;

  filterBuffer[count++] = skinResponse;
  if(count == FILTER_LEN){
    count = 0;
    }

  for (i = 0; i < FILTER_LEN; i++){
    Sum += filterBuffer[i];
    }

  filteredSkinResponse = Sum/FILTER_LEN;

  if(filteredSkinResponse>4000){
    filteredSkinResponse = 0;
  }
}

void setBPM() {
  Signal = analogRead(37);

  sampleCounter = sampleCounter + delayMs; // keeping track of time in ms
  int N = sampleCounter - lastBeatTime;

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
}
