/*
 * 
*/
#include <Arduino.h> 
#include "RTClib.h"
#include "rtc.h"
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DS1307 time;

void rtc_INIT(){
    if (! time.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! time.isrunning()) {
    Serial.println("RTC is NOT running!");
    
    // Set the date and time the sketch will be uploaded an run on:
    time.adjust(DateTime(2020, 6, 8, 14 ,30));
    Serial.println("Current time set to:");
    printTime();
  }
}


void printTime(){
  DateTime now = time.now();
  Serial.print(now.day(), DEC);
  Serial.print('.');
  Serial.print(now.month(), DEC);
  Serial.print('.');
  Serial.print(now.year(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}
