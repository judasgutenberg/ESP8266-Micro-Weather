const char* ssid = "your_ssid"; //Moxee Hotspot83_2.4G
const char* password = "your_wifi_password";
const char* storagePassword = "your_storage_password"; //needs to be the same as the one in config.php

//data posted to remote server so we can keep a historical record
//url will be in the form: http://your-server.com:80/weather/data.php?data=
const char* urlGet = "/weather/data.php";
const char* hostGet = "your_server.com";

const int locationId = 1; //for storing data from different locations in the backend
const int secondsGranularity = 300; //how often to store data in the backend in seconds
