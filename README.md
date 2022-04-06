# ESP8266TemperatureHumidity
A modification of a single-plot web-based data visualizer.


The .c file should really be a .ino file -- it is designed to be compiled and uploaded using the Arduino environment.  As such, it should be in the same directory
as index.h, which holds the code to show pretty graphs in real time directly from an ESP8266 acting as a web server.

But this code can also send data to a backend that stores data in a MySQL database.  This is what data.php does.  Place it on a PHP/MySQL web server you control,
in a directory of your choosing (I chose /weather) and configure it to connect to a database by changing the credentials in the code.  You can create the table
using weather.sql.  It produces a table for logging weather data.

So far there is no means to display that data, but this will be coming soon.
