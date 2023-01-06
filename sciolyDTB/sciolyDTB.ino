// need cmath for if the sensor changes logarithmicaly
#include <cmath>

// CALIBRATION
// set up to work like this, where y = grams and x = sensor input
// y = a(b(x+c))+d
// y = a*log_b(x+c)+d  (forgot if this is how log parent function is)
const static float CAL_A = 1;
const static float CAL_B = 1;
const static float CAL_C = 1;
const static float CAL_D = 1;

// PINS
// sensor pad in
unsigned short PIN_PADIN = A0;

void setup () {
	Serial.begin(9600);
}

void loop () {
	
}
