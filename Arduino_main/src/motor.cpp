/*
 * Controls the motor via relays
 * 
 * We assume there might be some variation in speed depending on the direction the motor is moving.
 * Therefore we measure the time both for left and right direction.
 * 
 */
 
//***********************************************************************************************
// SETTINGS:
// Pins for direction relays. These SHOULD NOT be used with real motor. If one of them fails it will cause a short circuit.
// USE A RELAY WICH MECANICALLY CONTROLS BOTH OUTPUTS AT THE SAME TIME. Direction2 is not needed in that case.                                     
#define direction1 22
#define direction2 23

#define motor 24            // Pin for motor start/stop relay

#define endstopMax 44       // Pin for endstop
#define endstopMin 43       // Pin for endstop
#define angleSpan 330       // Angles the panel turns from endstopMin to endstopMax
#define homeAngle 25        // Defines where the endstopMin is located.

#define motorAccuracy 2     // How many angles the given value must be over current position for the motor to move. Increasing this value will
                            // get you less accuracy, but it decreases wear on mecanical parts as the motor doesnt have to move as often.     

// Adjust for motor irregularities:
int rightOffset = 0;        // Offset in ms (turning right)
int leftOffset = 0;         // Offset in ms (turning left)

//***********************************************************************************************

#include "solar_time.h"
#include "LDR.h"
#include <Arduino.h>
#include <RTClib.h>
#include "motor.h"

int timeLeft = 11839;       // The measured time it takes the panel to turn from endstopMax to endstopMin.
int timeRight = 12273;      // The measured time it takes the panel to turn from endstopMin to endstopMax.

float msPerDegRight = 0;    // The time it takes to move 1 degree right
float msPerDegLeft = 0;     // The time it takes to move 1 degree right

extern int currentPos;
extern int rawLeft;
extern int rawRight;
extern int ldrDiff;



// Comment
void turnLeft(){
	while ( digitalRead(endstopMin) != 0){
		digitalWrite(direction1, LOW);
		digitalWrite(direction2, LOW);
		digitalWrite(motor, LOW);
	}
	digitalWrite(motor, HIGH);
	currentPos = homeAngle;
}


void turnRight(){
	while ( digitalRead(endstopMax) != 0){
		digitalWrite(direction1, HIGH);
		digitalWrite(direction2, HIGH);
		digitalWrite(motor, LOW);
	}
	digitalWrite(motor, HIGH);
	currentPos = angleSpan + homeAngle;
}

// Calculates angles to milliseconds:
int angleToMs(int angle){
	int timeOn = 0;
	if (angle >= 1){
		timeOn = (msPerDegRight) * abs(angle) + rightOffset;
	}
	else{
		timeOn = msPerDegLeft * abs(angle) + leftOffset;
	}
	return timeOn;
}


float msToAngle(int ms, String direction){
	float angle = 0;
	if (direction == "left"){ angle = ms / msPerDegLeft; }
	else { angle = ms / msPerDegRight; }
	return angle;
}


void goTo(int angle){
	// Compare given angle to current position, move to new position only if result is greater than motorAccuracy
	if(abs(angle - currentPos) >= motorAccuracy){
		int angleToMove = angle - currentPos;           // calculate degrees to move
		int timeOn = angleToMs(angleToMove);            // convert degrees to ms
		
		Serial.println("Reqested angle:     " + String(angle));
		Serial.println("Current position:   " + String(currentPos));
		Serial.println("Moving " + String(angleToMove) + " degrees in " + String(timeOn) + " ms");
		delay(2000);
		Serial.print("Moving....");
		
		unsigned long time0 = millis() + timeOn;
		
		if (angleToMove < 0){
			while ( digitalRead(endstopMin) != 0 && time0 > millis()){
				digitalWrite(direction1, LOW);
				digitalWrite(direction2, LOW);
				digitalWrite(motor, LOW);
			}
			digitalWrite(motor, HIGH);          //Turn off motor
		}
		else {
			while ( digitalRead(endstopMax) != 0 && time0 > millis()){
				digitalWrite(direction1, HIGH);
				digitalWrite(direction2, HIGH);
				digitalWrite(motor, LOW);
			}
			digitalWrite(motor, HIGH);          // Turn off motor
		}
	
		currentPos = angle;   // Update current position
		Serial.println("done");
		
	}
	else{
		Serial.println("Current position:   " + String(currentPos));
	}
}


void calculateDrift(){
	Serial.println("* Calculating drift. Moving to max. *");
	
	// Calculate how long it should take:
	int calculatedTime = angleToMs((homeAngle+angleSpan) - currentPos);
	
	unsigned long time0 = millis();
	turnRight();
	int time1 = millis() - time0;
	int drift = time1 - calculatedTime;
	Serial.println("* Previous position:  " + String(currentPos) +                       "           *");
	Serial.println("* Angle moved:        " + String((homeAngle+angleSpan) - currentPos) +  "            *");
	Serial.println("* Calculated time:    " + String(calculatedTime) + "          *");
	Serial.println("* Actual time:        " + String(time1) +     " ms      *");
	Serial.println("* Drifted:            " + String(drift) +     " ms      *"); 
	Serial.println("* Est drift angle:    " + String(msToAngle(drift, "left")) + "          *");
	// Serial.println("*************************************");

}


// Measure the time it takes to turn from left to right:
void calibrate(){
	
	turnLeft();                     // Make sure panel is at home position.
	unsigned long time1 = 0;        // Temporary variable for calculating time.
	time1 = millis();               // Store current time
	turnRight();                    // Turn panel right
	timeRight = millis() - time1;   // Save the time it took
	msPerDegRight = timeRight / angleSpan;        // How many ms it takes to move panel 1 deg.
	Serial.println("Left time:  " + String(timeLeft) + " ms   " + String(msPerDegRight) + " ms/deg");
	
	delay(500);
	
	// Measure the time it takes to turn from right to left:
	time1 = millis();
	turnLeft();
	timeLeft = millis() - time1;
	msPerDegLeft = timeLeft / angleSpan;
	Serial.println("Right time: " + String(timeRight) + " ms   " + String(msPerDegLeft) + " ms/deg");
	Serial.println("*************************************");
}


void calibrateStepped(){
	/*
	* TODO:
	* Calibrate by moving 10 degrees at a time. This will be needed for example if the motor doesn't start at full speed.
	*/
}






//***********************************************************************************************
// SETTINGS:

#define steps 10
#define accuracy 8

//***********************************************************************************************
  
void calibrateWithLDR(){
  	DS1307 rtc;       // Create RTC instance
  	Serial.println("****** calibrating with LDR **********");
  	Serial.println("Searching for the sun's position...");

	while(abs(ldrDiff) > accuracy) {
		delay(200);
		readLDR();
		if (rawLeft > rawRight){
		// Turn left until endstop or LDR match
		while ( digitalRead(endstopMin) != 0 || abs(ldrDiff) > accuracy){
			digitalWrite(direction1, LOW);
			digitalWrite(direction2, LOW);
			digitalWrite(motor, LOW);
		}
		digitalWrite(motor, HIGH);      // Turn off motor
		}
		if (rawLeft < rawRight){
		goTo(currentPos - steps);
		}
		readLDR();
	}

	DateTime now = rtc.now();
	float azimuth = getAzimuth(now.year(), now.month(), now.day(), now.hour(), now.minute());
	
	currentPos = azimuth;       // TODO: Couses rounding errors, loses accuracy
	
	Serial.println("The current position gives the following LDR values:");
	Serial.println("Left: " + String(rawLeft) + "      Right: " + String(rawRight));
	Serial.println("Homing panel... this will set the calculated home angle");
	turnLeft();                   // Homes the panel. This calculates the home angle also.
	Serial.println("Home angle is: " + String(currentPos));
}


void motor_INIT() {
	// initialize the relaypins as an output:
	pinMode(direction1, OUTPUT);
	pinMode(direction2, OUTPUT);
	pinMode(motor,OUTPUT);

	// The relays use reverse logic
	digitalWrite(motor, HIGH);
	digitalWrite(direction1,HIGH);
	digitalWrite(direction2,HIGH);

	// Set to input
	pinMode(endstopMin, INPUT_PULLUP);
	pinMode(endstopMax, INPUT_PULLUP);

	Serial.print("Homing panel....");
	turnLeft();                         // Move to home position
	currentPos = homeAngle;             // Update current position
	Serial.println("done");
	Serial.println("*************************************");
	calibrate();
}