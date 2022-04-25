/*
 * ESP8266 NodeMCU Real Time Dual Data Graph 
 * Updates and Gets data from webpage without page refresh
 * based on something from https://circuits4you.com
 * reorganized and extended by Gus Mueller, April 2022
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <DHT.h>

#include <Wire.h>
#include <SFE_BMP180.h>

#include "index.h" //Our HTML webpage contents with javascript
 
#define DHTPIN 2 // Digital pin 4//2
// #define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22 // DHT 22, AM2302, AM2321
#define DHTTYPE DHT21 // DHT 21, AM2301
DHT dht(DHTPIN, DHTTYPE);

//SSID and Password of your WiFi router, among other things, are in here:
#include "config.h"

bool glblRemote = false;
ESP8266WebServer server(80); //Server on port 80
SFE_BMP180 pressure;

//ESP8266's home page:----------------------------------------------------
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleWeatherData() {
  double humidityValue;
  double temperatureValue;
  double pressureValue;
  String transmitString = "";
  if(true) {
    //BMP180 code:
    char status;
    double p0,a;
    status = pressure.startTemperature();
    if (status != 0)
    {
      // Wait for the measurement to complete:
      delay(status);   
      // Retrieve the completed temperature measurement:
      // Note that the measurement is stored in the variable T.
      // Function returns 1 if successful, 0 if failure.
      status = pressure.getTemperature(temperatureValue);
      if (status != 0)
      {
        status = pressure.startPressure(3);
        if (status != 0)
        {
          // Wait for the measurement to complete:
          delay(status);
          // Retrieve the completed pressure measurement:
          // Note that the measurement is stored in the variable P.
          // Note also that the function requires the previous temperature measurement (temperatureValue).
          // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
          // Function returns 1 if successful, 0 if failure.
          status = pressure.getPressure(pressureValue,temperatureValue);
          if (status != 0)
          {
            a = pressure.altitude(pressureValue,p0);
          }
          else Serial.println("error retrieving pressure measurement\n");
        }
        else Serial.println("error starting pressure measurement\n");
      }
      else Serial.println("error retrieving temperature measurement\n");
    }
    else Serial.println("error starting temperature measurement\n");
    humidityValue = 0; //really should set unknown values as null
  } else {
    humidityValue = (double)dht.readHumidity();
    temperatureValue = (double)dht.readTemperature();
    pressureValue = 0; //really should set unknown values as null
  }
  transmitString = NullifyOrNumber(temperatureValue) + "*" + NullifyOrNumber(pressureValue) + "*" + NullifyOrNumber(humidityValue); //using delimited data instead of JSON to keep things simple
  Serial.println(transmitString);
  //had to use a global, died a little inside
  if(glblRemote) {
    sendRemoteData(transmitString);
  } else {
    server.send(200, "text/plain", transmitString); //Send values only to client ajax request
  }
}

String NullifyOrNumber(double inVal) {
  if(inVal == NULL) {
    return "NULL";
  } else {

    return String(inVal);
  }
}

//SETUP----------------------------------------------------
void setup(void){
  dht.begin();
  pressure.begin();
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

//SEND DATA TO A REMOTE SERVER TO STORE IN A DATABASE----------------------------------------------------
void sendRemoteData(String datastring) {
  WiFiClient clientGet;
  const int httpGetPort = 80;
  String url;
  url =  (String)urlGet + "?storagePassword=" + storagePassword + "&locationId=" + locationId + "&mode=saveData&data=" + datastring;
  Serial.print(">>> Connecting to host: ");
  Serial.println(hostGet);
  if (!clientGet.connect(hostGet, httpGetPort)) {
    Serial.print("Connection failed: ");
    Serial.print(hostGet);
  } else {
      clientGet.println("GET " + url + " HTTP/1.1");
      clientGet.print("Host: ");
      clientGet.println(hostGet);
      clientGet.println("User-Agent: ESP8266/1.0");
      clientGet.println("Connection: close\r\n\r\n");
      unsigned long timeoutP = millis();
      while (clientGet.available() == 0) {
        if (millis() - timeoutP > 10000) {
          Serial.print(">>> Client Timeout: ");
          Serial.println(hostGet);
          clientGet.stop();
          return;
        }
      }
      //just checks the 1st line of the server response. Could be expanded if needed.
      while(clientGet.available()){
        String retLine = clientGet.readStringUntil('\r');
        Serial.println(retLine);
        break; 
      }
  } //end client connection if else             
  Serial.print(">>> Closing host: ");
  Serial.println(hostGet);
  clientGet.stop();
}

//LOOP----------------------------------------------------
void loop(void){
  long nowTime = millis();
  if(nowTime - ((nowTime/(1000 * secondsGranularity) )*(1000 * secondsGranularity)) == 0 ) {  //send data to backend server every <secondsGranularity> seconds or so
    glblRemote = true;
    handleWeatherData();
    glblRemote = false;
  }
  //Serial.println(dht.readTemperature());
  server.handleClient();          //Handle client requests
}
