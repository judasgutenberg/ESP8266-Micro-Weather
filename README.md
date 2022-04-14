# ESP8266 Micro Weather
A soup-to-nuts micro-weather datalogging and visualization suite. This descends genetically from a simple single-plot web-based data visualizer for ESP8266 I found at https://circuits4you.com/2019/01/11/esp8266-data-logging-with-real-time-graphs/.

At this point, this version plots three weather parameters from multiple probes physically located at any number of locations (anywhere with access to WiFi) using graphs that can show three different time scales.

The .c file should really be a .ino file -- it is designed to be compiled and uploaded using the Arduino environment.  As such, it should be in the same directory
as index.h, which holds the code to show pretty graphs in real time directly from an ESP8266 acting as a web server.

But this code can also send data to a backend that stores data in a MySQL database.  This is what data.php does.  Place it on a PHP/MySQL web server you control, configure the code in esp-temp.c to point to it in a directory of your choosing (I chose /weather) and configure it to connect to a database by changing the credentials in data.php.  You can create the table using weather.sql.  The table is simple (though fairly comprehensive) for storing weather data, depending on the sensors you add to your micro-weather station.

Stored MySQL probe data is viewed via index.php on a ChartsJS graph.  The weather_data table has a location_id column to allow for multiple sensors logging from multiple locations.  Be sure to set the locationId appropriately in the Arduino sketch before uploading to your probe.

I've made it so the backend can store nulls in cases where there are no sensors for a value, which seemed more honest than storing zeros. For some reason the pressure is still stored as zero even when it should be null.

This system only enforces permissions for the storing of data.  Anyone can see the data logged at my Adirondack cabin using this system by visiting http://randomsprocket.com/weather.
