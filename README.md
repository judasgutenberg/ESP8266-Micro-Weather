# ESP8266TemperatureHumidity
A modification of a single-plot web-based data visualizer.


The .c file should really be a .ino file -- it is designed to be compiled and uploaded using the Arduino environment.  As such, it should be in the same directory
as index.h, which holds the code to show pretty graphs in real time directly from an ESP8266 acting as a web server.

But this code can also send data to a backend that stores data in a MySQL database.  This is what data.php does.  Place it on a PHP/MySQL web server you control, configure the code in esp-temp.c to point to it in a directory of your choosing (I chose /weather) and configure it to connect to a database by changing the credentials in data.php.  You can create the table using weather.sql.  It produces a table for logging weather data.

Stored MySQL probe data is viewed via index.php on a ChartsJS graph.  The weather_data table has a location_id column to allow for multiple sensors logging from multiple locations.  Be sure to set the locationId appropriately in the Arduino sketch before uploading to your probe.
