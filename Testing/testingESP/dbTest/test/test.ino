
#include <WiFi.h>
//#include <ESPAsyncWebServer.h>
//#include <SPIFFS.h>
  #include <HTTPClient.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
const char* ssid = "";
const char* password = "";
//AsyncWebServer server(80);
int i = 0;

const char* serverName = "http://your-mindshift.com/post-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorReading() { // return string of sensor information
  return String(random(100)); // placeholder value
 }

String sensorReading2() { // return string of sensor information
  return String(random(100)); // placeholder value
 }

String sensorReading3() { // return string of sensor information
  return String(random(100)); // placeholder value
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
                           + "&value2=" + String(sensorReading()) + "&value3=" + String(sensorReading()) + "";
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
  //Send an HTTP POST request every 30 seconds
  delay(30000);
}
