// WJ SCIENCE OLYMPIAD TEAM A CODE
// Zaine Rehman, Jacob Oshinsky

//oled libs
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PINS
#define sensorPin A5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET		 -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// CALIBRATION
// set up to work like this, where y = grams and x = sensor input
// y = a(b(x+c))+d
// y = a*log_b(x+c)+d	(forgot if this is how log parent function is)
constexpr float	CAL_A		= 1.5 ;
constexpr float	CAL_B		= 1.5 ;
constexpr float	CAL_C		= 1.5 ;
constexpr float	CAL_D		= -263;
static		double CAL_ZERO = 0	 ;

// PINS
// sensor pad in
constexpr uint8_t PAD_PIN = A0;
// adc pin
constexpr uint8_t READY_PIN = 7;
// zero button pin
constexpr uint8_t ZERO_PIN = 6;

//led pins
constexpr uint8_t greenPin = 4;
constexpr uint8_t redPin = 3;
constexpr uint8_t bluePin = 2;

constexpr uint8_t ITERATIONS = 10;
constexpr uint16_t R_0 = 10000; // known resistor value in [Ohms]
constexpr float	 VCC = 5.0; // supply voltage

// light up a specific LED and turn off the others
void setLed(const char& color) 
{
	if (color == 'r') {
		digitalWrite(redPin, HIGH);
		digitalWrite(greenPin, LOW);
		digitalWrite(bluePin, LOW);
		return;
	}
	if (color == 'g') {
		digitalWrite(redPin	, LOW);
		digitalWrite(greenPin, HIGH);
		digitalWrite(bluePin, LOW);
		return;
	}
	if (color == 'b') {
		digitalWrite(redPin, LOW);
		digitalWrite(greenPin,LOW);
		digitalWrite(bluePin, HIGH);
		return;
	}
}

double grams(const double& mv){
    return CAL_A * pow(CAL_B, (mv - CAL_C)) + CAL_D + CAL_ZERO;
	//return CAL_A * ( CAL_B * ( mv - CAL_C ) ) + CAL_D + CAL_ZERO;
	//return CAL_A * (log(mv) / log(CAL_B) - CAL_C) + CAL_D - CAL_ZERO;
}

double conversion(const double& red){
	return ((-50000 / (red * 4.88)) - 27);
	//return (((10000 * -5) / (red * 4.88)) - 27);
}

void setup () 
{
	pinMode(greenPin,OUTPUT);
	pinMode(redPin,OUTPUT);
	pinMode(bluePin,OUTPUT);
	//pinMode(analogPin,INPUT);

 
	Serial.begin(9600);
	//display setup
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		Serial.println(F("SSD1306 allocation failed"));
		while (1); // Don't proceed, loop forever
	}

	// Show initial display buffer contents on the screen --
	// the library initializes this with an Adafruit splash screen.
	display.display();
	delay(1000); // Pause for 2 seconds

	// Clear the buffer
	display.clearDisplay();
	display.drawPixel(10, 10, SSD1306_WHITE);

	// Show the display buffer on the screen. You MUST call display() after
	// drawing commands to make them visible on screen!
	display.display();
	delay(1000);

}

void loop () 
{
	double sum_val = 0.0;
	double R_FSR;
	for(uint16_t i = 0; i < ITERATIONS; i++){
		sum_val += (analogRead(sensorPin) / 1023) * 5;
		delay(10);
	}
	sum_val /= ITERATIONS;

	R_FSR = (R_0 / 1000) * ((VCC / sum_val) - 1);

	
	display.clearDisplay();
	display.setTextSize(1);						 // Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE);				// Draw white text
	display.setCursor(0,0);						 // Start at top-left corner

	display.println("mv: ");
	display.println(mathsfun(R_FSR));
	Serial.println(R_FSR);
	display.println("grams: ");
	display.println(conversion(R_FSR));
	display.display();
}