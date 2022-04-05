


CREATE TABLE weather_data(
weather_data_id INT AUTO_INCREMENT PRIMARY KEY,
recorded DATETIME,
temperature DECIMAL(6,3),
pressure DECIMAL(9,4),
humidity DECIMAL(6,3),
wind_direction INT,
precipitation INT,
wind_speed DECIMAL(8,3),
wind_increment INT
)
