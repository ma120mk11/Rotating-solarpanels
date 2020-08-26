/*  v1.0
 *  This program uploads given the data to Google Docs.
 *  The link for Google Spreadsheet:
 *  https://docs.google.com/spreadsheets/d/1gXEi51ec1EhLaPkwQn9Iorw4NKVxetNzNAsS8LYtLmg/edit#gid=0
 *  
 */

#include <CytronWiFiShield.h>
#include <CytronWiFiClient.h>
#include <SoftwareSerial.h>

//***********************************************************************************************
// SETTINGS:

//   *** NETWORK    SETTINGS ***  //
const char* ssid = "HUAWEI P20 Pro";        // Network name
const char* pass = "3665d14cd73b";          // Password

//   *** GOOGLE DOCS SETTINGS ***  //
const char* host = "script.google.com";
const char *GScriptId = "AKfycbw9ykkNwFh9cAhBRfEpQMsGAA16ITga74wXDZhelszfDOzn6lk";  // 

//***********************************************************************************************

const int httpsPort = 443;

// Write to google Spreadsheets
String url = String("/macros/s/") + GScriptId + "/exec";

int errorCount = 0;


void wifi_INIT(){
	/*
	*  Wifi connection
	*/
	
	if(!wifi.begin(10, 11)){
		Serial.println(F("Error talking to Wifi-shield. Restart required"));
		while(true);
	}
		
	Serial.println(F("Starting wifi connection..."));

	
	if(!wifi.connectAP(ssid, pass) && errorCount >= 4){
		Serial.println(F("Error connecting to WiFi. Restart required"));
		while(true);
	}
	if(!wifi.connectAP(ssid, pass)){
		Serial.println(F("Error connecting to WiFi. Trying to reconnect in 5s..."));
		errorCount ++;
		delay(5000);
		wifi_INIT();
	}
	errorCount = 0;
	Serial.println("*************************************");  
	Serial.print(F("Connected to:   "));Serial.println(wifi.SSID());
	Serial.print(F("IP address:     "));Serial.println(wifi.localIP());
	Serial.println("*************************************");
}


void logToCloud(float value0, float value1, float value2, float value3, float value4, float value5){
	/*
	* If the connection is lost -> retry 4 times
	*/
	String arg = "?val0=" + String(value0) 
				+ "&val1=" + String(value1)
				+ "&val2=" + String(value2)
				+ "&val3=" + String(value3)
				+ "&val4=" + String(value4)
				+ "&val5=" + String(value5);
				// If you need to upload more data rows, add arguments here. Note that you must update the function
				// and the google script also.
				
	bool i = 0;
	ESP8266Client client;
	while(i == 0){
		
		Serial.print("Uploading data to spreadsheet...");
		i = 1;
		if (!client.secure_connect(host, httpsPort)) {
			if (errorCount >= 4){
				Serial.println(F("\nFailed to connect to server. Unable to upload data."));
				client.stop();
				return;
			}
			else{
				Serial.println(F("\nFailed to connect to server. Retrying in 5s..."));
				client.stop();
				delay(5000);
				errorCount++;
				i = 0;
			}  
		}
	}
	
	errorCount = 0;       // Reset error
	
	client.print(String("GET ") + url + arg + " HTTP/1.1\r\n" +
				"Host: " + host + "\r\n" +
				"User-Agent: BuildFailureDetectorESP8266\r\n" +
				"Connection: close\r\n\r\n");
	
	Serial.println(F("done \n"));
}
