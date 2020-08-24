/*
 * v1
 * Calculates the sun's position, depending on time and your location.
 * 
 * The formula is based on the General Solar Position Calculations pdf taken from
 * https://www.esrl.noaa.gov/gmd/grad/solcalc/solareqns.PDF
 * 
 * Ove Niemistö 2020
 */
 
#include <math.h>


//***********************************************************************************************
// SETTINGS:

/* Go to for example https://www.suncalc.org/ and zoom into your position to find these values. */

/* POSITION */
double latitude = 63.21249;
double longitude = 21.66605;

/* TIMEZONE */
int UTC = 3;

//***********************************************************************************************


// VARIABLES:
int year = 2020;
int month = 6;
int day = 3;
int hour = 12;
int minute = 0;
int second = 0;

bool isLeapYear = 1;

int day_of_year;


// FUNCTIONS:
int dayOfYear(int year, int month, int day){
  int day_of_year = day;
  
  int days_per_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  
  // February has 29 days if its leap year:
  if (isLeapYear == 1){
    days_per_month [2] = 29;
  }
  
  while (month > 1){
    day_of_year += days_per_month[month];
    month--;
  }

  return day_of_year;
}


float getAzimuth(int year1, int month1, int day1, int hour1, int minute1){
  // update variables
  year = year1;
  month = month1;
  day = day1;
  hour = hour1;
  minute = minute1;
  
  double gamma;
  double eqtime;
  double decl;
  double latitudeRad = (latitude * 71) / 4068.0;
  double longitudeRad = (longitude * 71) / 4068.0;
  double time_offset;
  double tst;                                       // True Solar Time
  double solarTimeAngle;
  double solarTimeAngleRad;
  double zenith;
  double zenithDeg;
  double azimuth;
  double azimuthDeg;
  double elevationDeg;
  
  day_of_year = dayOfYear(year, month, day);
  
  // Fractional year is calculated in radians:
    if (isLeapYear==1){
      gamma=((2*3.14159265359)/366.0) * (day_of_year - 1 + ((hour-12)/24.0));
    }
    else{
      gamma=((2*3.14159265359)/365.0) * (day_of_year - 1 + ((hour-12)/24.0));
    }

  // From gamma we can estimate the Equation of Time (in minutes):
    eqtime = 229.18 * (0.000075 + 0.001968 * cos(gamma)-0.032077 * sin(gamma) - 0.014615 * cos(2.0*gamma) - 0.040849 * sin(2.0*gamma));

  // Solar declination angle in radians:
    decl = 0.006918 - 0.399912 * cos(gamma) + 0.070257*sin(gamma) - 0.006758*cos(2*gamma) + 0.000907*sin(2*gamma) - 0.002697*cos(3*gamma) + 0.00148*sin(3*gamma);

  // Offset, in minutes:
    time_offset = eqtime + 4.0 * longitude - 60.0 * UTC;

  // True Solar time, in minutes:
    tst = hour * 60.0 + minute + second/60.0 + time_offset;

  
  // Solar hour angle:
    solarTimeAngle = (tst/4.0) - 180;
    solarTimeAngleRad = (solarTimeAngle * 71) / 4068.0;
  
  // Zenith angle. This is the angle between the sun and vertical (straight up).
    zenith = sin(latitudeRad) * sin(decl) + cos(latitudeRad) * cos(decl) * cos(solarTimeAngleRad);
    zenith = acos(zenith); 
    zenithDeg = (zenith * 4068) / 71.0;
    
  // Elevation angle (altitude):  
    elevationDeg = 90.0 - zenithDeg;

  // Azimuth: 
    azimuth = -((sin(latitudeRad) * cos(zenith) - sin(decl))/(cos(latitudeRad) * sin(zenith)));
    azimuth = acos(azimuth);

  // Convert to Degrees:  
    azimuthDeg =  (azimuth * 4068) / 71.0;
    
    if (azimuthDeg < 180 && solarTimeAngle > 0){
      azimuthDeg = 360 - azimuthDeg;
    }
    /*
    Serial.print("day of the year:  ");
    Serial.println(day_of_year);
    Serial.print("Equation of time: ");
    Serial.println(eqtime);
    Serial.print("Time offset:      ");
    Serial.println(time_offset);
    Serial.print("Declination:      ");
    Serial.println((decl * 4068) / 71.0);
    Serial.print("zenith:           ");
    Serial.println((zenith * 4068) / 71.0);
    Serial.print("True solar time:  ");
    Serial.println(tst);
    Serial.print("Elevation angle   ");
    Serial.println(elevationDeg);
    Serial.print("Azimuth:          ");
    Serial.println(azimuth);
    Serial.println("");
    */
    return azimuthDeg;
}
