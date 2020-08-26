/*
 * Prints LDR values to the serial monitor
 * 
 * Use Telemetry to monitor data.
*/

#include <Ewma.h>
#include <EwmaT.h>
#include <Arduino.h>

//***********************************************************************************************
// SETTINGS:

#define LDR1 0    // Analog input pin for left LDR
#define LDR2 1    // Analog input pin for right LDR


#define smoothing 0.1     // Smoothing (0<x<1). Lower value results in more smoothing.

    
//***********************************************************************************************

// Filters:
Ewma adcFilter1(smoothing);     
Ewma adcFilter2(smoothing); 


// Variables:
extern int rawLeft;
extern int rawRight;
extern int ldrDiff;
extern int lightPercent;

// Uncomment if a calibration curve is used
// int rawRightComp = 0;
// float x = 0;
// float comp;
// int compensation;


void readLDR() {
  
  // read sensor values:
  rawLeft = analogRead(LDR1);
  rawRight = analogRead(LDR2);

  // Smoothing:
//  rawLeft = adcFilter1.filter(rawLeft);
//  rawRight = adcFilter2.filter(rawRight);

  /* TODO: Try to match sensitivity with a compensation curve. Measure values from both LDR's in different amount of light an generate a curve in excel.
   *  The current curve has too few measure points to be accurate.
      
      x=rawRight;     // Temporary 
      comp = 0.4 *(pow((7/10000000000000)*x,5) - pow((1/1000000000)*x,4) + pow((1/1000000)*x,3) - pow(0.0004*x,2) + 0.1404*x + 0.8875);
      
      compensation = int(comp);
      rawRightComp = rawRight + compensation;
  */
  
  ldrDiff = rawLeft - rawRight;

  if (rawLeft >= rawRight){ lightPercent = float((rawLeft / 1023.0) * 100);}
  else { lightPercent = float((rawRight / 1023.0) * 100);}
}
