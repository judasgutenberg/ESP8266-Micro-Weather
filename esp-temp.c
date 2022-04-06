/*
 * ESP8266 NodeMCU Real Time Dual Data Graph 
 * Updates and Gets data from webpage without page refresh
 * based on something from https://circuits4you.com
 * organized and extended by Gus Mueller, April 2022
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#include "index.h" //Our HTML webpage contents with javascript
 
#define DHTPIN 2 // Digital pin 4//2
// #define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22 // DHT 22, AM2302, AM2321
#define DHTTYPE DHT21 // DHT 21, AM2301
DHT dht(DHTPIN, DHTTYPE);

//SSID and Password of your WiFi router
const char* ssid = "your-ssid";
const char* password = "your-password";

//data posted to remote server so we can keep a historical record
//url will be in the form: http://your-server.com/weather/data.php?data=
const char* urlGet = "/weather/data.php";
const char* hostGet = "your-server"; //where you put the urlGet path and the data.php file

const int locationId = 1; //for storing data from different locations in the backend
const int secondsGranularity = 120; //how often to store data in the backend in seconds

bool glblRemote = false;
ESP8266WebServer server(80); //Server on port 80

//ESP8266's home page:----------------------------------------------------
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleWeatherData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  //Serial.println("------------");
  //Serial.println(h);
  String tString = String(t) + "**" + String(h); //using delimited data instead of JSON to keep things simple
  //Serial.println(tString);
  //had to use a global, died a little inside
  if(glblRemote) {
    sendRemoteData(tString);
  } else {
    server.send(200, "text/plane", tString); //Send values only to client ajax request
  }
  //also send it to the website of our choosing:
}

//SETUP----------------------------------------------------
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

//SEND DATA TO A REMOTE SERVER TO STORE IN A DATABASE----------------------------------------------------
void sendRemoteData(String datastring) {
  WiFiClient clientGet;
  const int httpGetPort = 80;
  // We now create and add parameters:
  String src = "ESP";
  String typ = "flt";
  String nam = "temp";
  String vint = "92"; 
  String url;
  url =  (String)urlGet + "?locationId=" + locationId + "&mode=saveData&data=" + datastring;
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
