<?php 
//temperaturebot backend. 
//i've tried to keep all the code vanilla and old school
//of course in php it's all kind of bleh
//gus mueller, April 7 2022
//////////////////////////////////////////////////////////////

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);
include("./functions.php");
include("config.php");

$conn = mysqli_connect($servername, $username, $password, $database);

$mode = "";

$date = new DateTime("now", new DateTimeZone('America/New_York'));//obviously, you would use your timezone, not necessarily mine
$formatedDateTime =  $date->format('Y-m-d H:i:s');
//$formatedDateTime =  $date->format('H:i');

$method = "";
$out = "";
if($_REQUEST) {
	$mode = gvfw("mode");
	$locationId = gvfw("locationId");
	if($locationId == "") {
		$locationId = 1;
	}
	if($mode=="kill") {
    $method  = "kill";
	
	} else if ($mode=="getData") {

 
		if(!$conn) {
			$out = ["error"=>"bad database connection"];
		} else {
			$scale = gvfw("scale");
			if($scale == ""  || $scale == "fine") {
				$sql = "SELECT * FROM " . $database . ".weather_data  
				WHERE recorded > DATE_ADD(NOW(), INTERVAL -1 DAY) AND location_id=" . $locationId . " 
				ORDER BY weather_data_id ASC";
			} else {
				if($scale == "hour") {
					$sql = "SELECT
					 *,
					 YEAR(recorded), DAYOFYEAR(recorded), HOUR(recorded) FROM " . $database . ".weather_data  
					 WHERE recorded > DATE_ADD(NOW(), INTERVAL -7 DAY) AND location_id=" . $locationId . " 
						GROUP BY YEAR(recorded), DAYOFYEAR(recorded), HOUR(recorded)
					 	ORDER BY weather_data_id ASC";
				}
				if($scale == "day") {
					$sql = "SELECT 	 
					*,
			 		YEAR(recorded), DAYOFYEAR(recorded) FROM " . $database . ".weather_data  
					WHERE location_id=" . $locationId . " 
						GROUP BY YEAR(recorded), DAYOFYEAR(recorded)
					 	ORDER BY weather_data_id ASC";
				}
			}
			/*
			//using averages didn't work for some reason:
			weather_data_id, 
			 recorded, 
			  AVG(temperature) AS temperature, 
			  AVG(pressure) AS pressure, 
			  AVG(humidity) AS humidity, 
			 wind_direction, 
			 AVG(precipitation) AS precipitation, 
			 wind_increment, 
			 */
			//echo $sql;
			$result = mysqli_query($conn, $sql);
			$out = [];
			while($row = mysqli_fetch_array($result)) {
				array_push($out, $row);
			}
		}
		$method  = "read";	
	} else if ($mode == "saveData") { //save data
      if(!$conn) {
        $out = ["error"=>"bad database connection"];
      } else {
        $data = gvfw("data");
        $arrData = explode("*", $data);
        $temperature = $arrData[0];
        $pressure = intval($arrData[1]);
        $humidity = $arrData[2];
        $gasMetric = "NULL";
        if(count($arrData)>3) {
          $gasMetric = $arrData[3];
        }
        $sql = "INSERT INTO weather_data(location_id, recorded, temperature, pressure, humidity, gas_metric, wind_direction, precipitation, wind_speed, wind_increment) VALUES (" . 
          mysqli_real_escape_string($conn, $locationId) . ",'" .  
          mysqli_real_escape_string($conn, $formatedDateTime)  . "'," . 
          mysqli_real_escape_string($conn, $temperature) . "," . 
          mysqli_real_escape_string($conn, $pressure) . "," . 
          mysqli_real_escape_string($conn, $humidity) . "," . 
          mysqli_real_escape_string($conn, $gasMetric) .
          ",NULL,NULL,NULL,NULL)";
        //echo $sql;
        if($storagePassword == gvfw("storagePassword")) { //prevents malicious data corruption
          $result = mysqli_query($conn, $sql);
        }
        $method  = "insert";
        $out = Array("message" => "done", "method"=>$method);
  	}

    }
	echo json_encode($out);
	
	
} else {
	echo '{"message":"done", "method":"' . $method . '"}';
}


//CREATE USER 'weathertron'@'localhost' IDENTIFIED  BY 'your_password';
//GRANT CREATE, ALTER, DROP, INSERT, UPDATE, DELETE, SELECT, REFERENCES, RELOAD on *.* TO 'weathertron'@'localhost' WITH GRANT OPTION;
//GRANT ALL PRIVILEGES ON *.* TO 'weathertron'@'localhost' WITH GRANT OPTION;
 
