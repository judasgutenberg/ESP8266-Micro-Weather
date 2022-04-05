/*
 * ESP8266 NodeMCU Real Time Dual Data Graph Demo
 * Updates and Gets data from webpage without page refresh
 * based on something from https://circuits4you.com
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#include "index.h" //Our HTML webpage contents with javascripts

 
#define DHTPIN 2 // Digital pin 4//2
// #define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22 // DHT 22, AM2302, AM2321
#define DHTTYPE DHT21 // DHT 21, AM2301
DHT dht(DHTPIN, DHTTYPE);

//SSID and Password of your WiFi router
const char* ssid = "your-ssid";
const char* password = "your-password";

ESP8266WebServer server(80); //Server on port 80

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleWeatherData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  //Serial.println("------------");
  //Serial.println(h);
  String tString = String(t) + "*" + String(h); //using delimited data instead of JSON to keep things simple
  //Serial.println(tString);
 
  server.send(200, "text/plane", tString); //Send values only to client ajax request
}
//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  dht.begin();
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/weatherdata", handleWeatherData); //This page is called by java Script AJAX

  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  //Serial.println(dht.readTemperature());
  server.handleClient();          //Handle client requests
}
