<?php
//get the weather data from darksky
//$myKey = "01504b93182f31363104a5a3c201acb9/";
//Toronto 43.6332, -79.4186
//New York ‎40.730610, ‎-73.935242	
//honolulu hawaii. 21.291,-157.684 
$myKey = "21736e4b7fc38b6e81c57f253b6c43b5/";
$latitude = "43.6332";
$longitude = "-79.4186";
$url = "https://api.darksky.net/forecast/";
$exclude = "?exclude=[currently,minutely,daily,flags]";

$api = $url.$myKey.$latitude. "," .$longitude.$exclude;

$content = file_get_contents($api);
$asObjects = json_decode($content);
$data = $asObjects->hourly->data;
$location = $asObjects->timezone;

//define how frequency to grab data
$duration = 5;
$timer = 0;
$weatherStatue = "null";
$curTemp = 0;

foreach ($data as $value){
	if($timer == 0){
		$weatherStatue = $value->icon;
		$curTemp = $value->apparentTemperature;
	}
	if($timer >0 && $timer < $duration){
		if(weather($value->icon) > weather($weatherStatue)){
      	$weatherStatue = $value->icon;
      	$curTemp = $value->apparentTemperature;
      }
	}else if($timer >= $duration){
		$timer = 0;
		// echo "Here is the weather type: ";
		// echo weather($weatherStatue);
		// echo ",";
		// echo $curTemp;
		$weatherStatue = "null";
		$curTemp = 0;		
	}
	$timer ++;
}

// $myObj->tempType = (string)weather($weatherStatue);
// $myObj->curTemp= (string)$curTemp;

// $myJSON = json_encode($myObj);

// error_reporting(E_ERROR | E_PARSE);
// echo $myJSON;

echo $tempType = weather($weatherStatue);
echo ',';
echo $curTemp;

//Giving value to each weather condition
//weather condition would be get from dark sky: clear-day, clear-night, rain, snow, sleet, wind, fog, cloudy, partly-cloudy-day, partly-cloudy-night. *hail, thunderstorm, tornado
//weather condition we will define: clear-day, cloudy/partly-cloudy-day, clear-night/partly-cloudy-night, rain, snow
function weather($i){
	switch ($i) {
		case 'clear-day':
			return 1;
			break;
		// when it is cloudy and partly-cloudy-day, it will be defined as a same weather condition > cloudy
		case 'cloudy';
			return 2;
			break;
		case 'partly-cloudy-day':
			return 2;
			break;
		// when it is clear-night or partly-cloudy-night, it will be defined as a some weather condition > night
		case 'clear-night':
			return 3;
			break;
		case 'partly-cloudy-night':
			return 2;
			break;
		case 'rain':
			return 4;
			break;
		case 'snow':
			return 5;
			break;
		//except for the situation mentioned above, it will be defined as a default type
		default:
			return 1;
			break;
	}
}

?>

