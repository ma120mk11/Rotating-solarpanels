/*
 * Digital temperature sensor
 * 
 * Use a 10k resistor parallel between signal and Vcc
 * 
 *  BLACK   -   GND
 *  RED     -   +5V
 *  YELLOW  -   signal
 * 
 */

// Function to call in setup:
void temperature_INIT();

// Returns temperature in celsius. Input the number of the sensor (default = 0)
float temperature_read(int);
