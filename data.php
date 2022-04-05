<?php 
//speakerbot backend. this plays audio via a python script on the server (usually something like a raspberry pi)
//i've tried to keep all the code vanilla and old school
//gus mueller, july 12 2019
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
$username = "weathertron";
$database = "weathertron";
$password = "your_password";
$conn = mysqli_connect($servername, $username, $password, $database);
 

$date = new DateTime("now", new DateTimeZone('America/New_York') );
$formatedDateTime =  $date->format('Y-m-d H:i:s');
//$formatedDateTime =  $date->format('H:i');

if($_REQUEST) {
	$mode = $_REQUEST["mode"];
	if($mode=="kill") {
    $method  = "kill";
	
	} else if ($_REQUEST["mode"] && $mode=="getData") {

 
		if(!$conn) {
			$out = ["error"=>"bad database connection"];
		} else {
			$sql = "SELECT * FROM " . $database . ".weather_data  WHERE recorded > DATE_ADD(NOW(), INTERVAL -1 DAY)  ORDER BY weather_data_id   ASC LIMIT 0, 200";
			$result = mysqli_query($conn, $sql);
			$out = [];
			while($row = mysqli_fetch_array($result)) {
				array_push($out, $row);
			
			}
		}
		$method  = "read";
		
	} else { //save data
 
      if(!$conn) {
        $out = ["error"=>"bad database connection"];
      } else {
        $data = $_REQUEST["data"];
        $arrData = explode("*", $data);
        $temperature = $arrData[0];
        $humidity = $arrData[1];
        $sql = "INSERT INTO weather_data(recorded, temperature, pressure, humidity, wind_direction, precipitation, wind_speed, wind_increment) VALUES ('" . $formatedDateTime  . "'," . $temperature . ",0," . $humidity . ",0,0,0,0)";
        //echo $sql;
        $result = mysqli_query($conn, $sql);
        $method  = "insert";
        $out = Array("message" => "done", "method"=>$method);
  
    }
	}
	echo json_encode($out);
	
	
} else {
	echo '{"message":"done", "method":"' . $method . '"}';
}


//CREATE USER 'weathertron'@'localhost' IDENTIFIED  BY 'tron';
//GRANT CREATE, ALTER, DROP, INSERT, UPDATE, DELETE, SELECT, REFERENCES, RELOAD on *.* TO 'weathertron'@'localhost' WITH GRANT OPTION;
//GRANT ALL PRIVILEGES ON *.* TO 'weathertron'@'localhost' WITH GRANT OPTION;
 
