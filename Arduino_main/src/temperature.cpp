/*
 * Digital temperature sensor
 */
#include <DallasTemperature.h>
#include <OneWire.h>

//***********************************************************************************************
// SETTINGS:

#define temp_sensor1_pin 52     // Digital pin the DS18B20 bus is connected to

//***********************************************************************************************

float temperature = 0;          // Variable to store temperature value

OneWire oneWirePin(temp_sensor1_pin);
DallasTemperature sensors(&oneWirePin);


void temperature_INIT() {
	
  sensors.begin();
  Serial.println("Temperature initialized");
}


float temperature_read(int index){
  float temperature = 0;
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(index);
  return temperature;
}
