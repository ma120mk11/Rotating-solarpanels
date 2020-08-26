/* This program depends on a couple of external libraries that you have to download and install for the program to work.
 *  
 * - RTClib library:  https://github.com/NeiroNx/RTCLib
 * - DallasTemperature library: https://github.com/milesburton/Arduino-Temperature-Control-Library
 * - OneWire: 
 * - SoftwareSerial: Can be found in the library manager. (may be installed by default).
 *   These two can be downloaded from https://github.com/CytronTechnologies/CytronWiFiShield
 * - CytronWiFiShield.h
 * - CytronWiFiClient.h
 * - Ewma filters : Install from library manager.
 * 
 * Each file has a SETTINGS section in the beginning.
 * 
 */
#include <Arduino.h>            // Nedded when using VSC
#include <RTClib.h>
#include <Wire.h>
#include "log_to_cloud.h"
#include "temperature.h"
#include "solar_time.h"
#include "motor.h"
#include "LDR.h"
#include "rtc.h"


//***********************************************************************************************
// SETTINGS:

// Set the time interval the panel should be moving in. This could be further developed to change depending on the month.
// The enstop switches should be OUTSIDE of this time interval.

#define startHour 7
#define stopHour 23     

#define updateInterval 30000    // How often (ms) to check if we should move panel and log to cloud 


//***********************************************************************************************

bool count;
// For LDR 
int rawLeft = 0;
int rawRight = 0;
int ldrDiff = 0;
int lightPercent;
int currentPos;             // Variable to store current set position.

DS1307 rtc;

/*  ******  FUNCTIONS  ******  */
/*void  calibrate();              // motor
void  calibrateStepped();
void  goTo(int angle);
void  turnLeft();
void  turnRight();
int   angleToMs(int angle);*/
//void  rtc_INIT();                // RTC
//void  printTime();               // Prints the current date and time to the serial monitor.
//void  calibrateWithLDR();         // 
//int   calculateDrift();

void setup() {
  Serial.begin(9600);
  Serial.println("*************************************   SETUP");
  temperature_INIT();
  wifi_INIT();
  rtc_INIT();
  motor_INIT();
  
  calibrateWithLDR();       // for testing
  Serial.println("*************************************   SETUP DONE");
}


void loop() {
  DateTime now = rtc.now();
  
  if (now.hour() == stopHour && count == 0){
    calculateDrift();
    turnLeft();       // Reset to home position
    count = 1;         // To only run this once
    Serial.println("*************************************");
  }
  
  // Check if current time is inside time interval:
  else if (now.hour() >= startHour && now.hour() < stopHour){
    count = 0;     // reset count
    
    // get azimuth:
    float azimuth = getAzimuth(now.year(), now.month(), now.day(), now.hour(), now.minute());
    float temp = temperature_read(0);     // Read temperature sensor 0


    printTime();  // Print time and date to serial monitor
    Serial.println("Temperature:  " + String(temp));
    Serial.println("Azimuth:      " + String(azimuth));
     
    goTo(azimuth);    // Update panel position

    readLDR();
    Serial.println("Light:        " + String(lightPercent) + "%");
    Serial.println("L/R diff:     " + String(ldrDiff));
    
    // Upload 6 number values. The order should be the same as the columns on the spreadsheet.
    
    logToCloud(azimuth,0,lightPercent,ldrDiff,temp,0);
    
    Serial.println("*************************************");
  }
  // When time is outside of the update interval
  else if (count == 0){   
    printTime();
    Serial.println("Sleeping... panel will start at " + String(startHour) + ":00");
    count = 1;
    Serial.println("*************************************");
  }
  
  delay(updateInterval);
  
}
