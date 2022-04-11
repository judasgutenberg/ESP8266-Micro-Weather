<?php 
//temperaturebot backend. 
//i've tried to keep all the code vanilla and old school
//gus mueller, April 7 2022
//////////////////////////////////////////////////////////////
 

//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);


function cleanBme280Data($in) {
	//echo trim($in) ;
	$arrIn = explode(" ", trim($in));
	$out = $arrIn[0];
	return $out;
}

$file = "";
$blob = "";
$mode = "";

if($_REQUEST && $_REQUEST["file"]) {
	$file = $_REQUEST["file"];
}
  
if($_POST) {
	$blob = base64_decode(str_replace('^', '+', str_replace("~", "/", $_POST['blob']))); //OMG THESE FUCKING REPLACEMENTS!!!
}

$servername = "localhost";
$username = "your_sql_user";
$database = "your_database";
$password = "your_mysql_password";
$storagePassword = "your_storage_password";
$conn = mysqli_connect($servername, $username, $password, $database);
 

$date = new DateTime("now", new DateTimeZone('America/New_York') );
$formatedDateTime =  $date->format('Y-m-d H:i:s');
//$formatedDateTime =  $date->format('H:i');

if($_REQUEST) {
	$mode = $_REQUEST["mode"];
	$locationId = $_REQUEST["locationId"];
	if($mode=="kill") {
    $method  = "kill";
	
	} else if ($_REQUEST["mode"] && $mode=="getData") {

 
		if(!$conn) {
			$out = ["error"=>"bad database connection"];
		} else {
			$sql = "SELECT * FROM " . $database . ".weather_data  WHERE recorded > DATE_ADD(NOW(), INTERVAL -1 DAY) AND location_id=" . $locationId . " ORDER BY weather_data_id ASC";
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
        $data = $_REQUEST["data"];
        $arrData = explode("*", $data);
        $temperature = $arrData[0];
        $pressure = intval($arrData[1]);
        $humidity = $arrData[2];
        $sql = "INSERT INTO weather_data(location_id, recorded, temperature, pressure, humidity, wind_direction, precipitation, wind_speed, wind_increment) VALUES (" . 
          mysqli_real_escape_string($conn, $locationId) . ",'" .  
          mysqli_real_escape_string($conn, $formatedDateTime)  . "'," . 
          mysqli_real_escape_string($conn, $temperature) . "," . 
          mysqli_real_escape_string($conn, $pressure) . "," . 
          mysqli_real_escape_string($conn, $humidity) . 
          ",NULL,NULL,NULL,NULL)";
        //echo $sql;
        if($storagePassword == $_REQUEST["storagePassword"]) { //prevents malicious data corruption
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
 
