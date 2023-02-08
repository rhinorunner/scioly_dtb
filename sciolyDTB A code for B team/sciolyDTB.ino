#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Math.h>

/***********************************************/

// measurement delay, in ms
static constexpr uint16_t DB_MEASUREDELAY = 20;
// debug mode
static constexpr bool DB_DEBUG = true;

// the bounds
static constexpr uint16_t[4] DB_BOUNDS {0, 105, 445, 1000};

// pins
static constexpr uint8_t redPin 4;
static constexpr uint8_t greenPin 3;
static constexpr uint8_t bluePin 2;
static constexpr uint8_t buttonPin 7;

/***********************************************/

// screen things
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ADC setup (jacob help)
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1015 ads;

// e (!!!)
static constexpr float e = 2.71828;

// input from the ADC
static uint16_t ADC_in = 0;
// stores the last 5 data points
static uint16_t dataset[5] = {0,0,0,0,0};

// zero value
// calibration!!!
static float zeroVal = 50;

// set the LEDs
void setLed(const char& color) 
{
	if (color == 'R') {
		digitalWrite(redPin  , HIGH);
		digitalWrite(greenPin, LOW );
		digitalWrite(bluePin , LOW );
	}
	if (color == 'G') {
		digitalWrite(redPin  , LOW );
		digitalWrite(greenPin, HIGH);
		digitalWrite(bluePin , LOW );
	}
	if (color == 'B') {
		digitalWrite(redPin  , LOW );
		digitalWrite(greenPin, LOW );
		digitalWrite(bluePin , HIGH);
	}
	if (color == 'X') {
		digitalWrite(redPin  , LOW);
		digitalWrite(greenPin, LOW);
		digitalWrite(bluePin , LOW);
	}
	if (color == '!') {
		digitalWrite(redPin  , HIGH);
		digitalWrite(greenPin, HIGH);
		digitalWrite(bluePin , HIGH);
	}
}

// converts data to grams
// jacob is a nerd
double convertNumber(uint16_t data)
{
	float data2 = (float)data / 525;
	double dataFinal = exp(data2 + 3.9486);
	
	if (DB_DEBUG) {
		Serial.print("convertNumber data -> grams: [");
		Serial.print(data);
		Serial.print("] -> [");
		Serial.print(dataFinal);
		Serial.print("]\n");
	}
	
	return dataFinal;
}

// average the data set (last 5 data points)
// "I love this int!" YOU ARE WASTING MEMORY SPACE JACOB
// THE DATA COULD EASLIY BE STORED INSIDE UINT16_T
// JESUS CHRIST STOP USING INT
uint16_t smoothing(uint16_t rawData)
{
	uint16_t dataAmount = 5;
	uint16_t sum = 0;
	float avg = 0;

	// push back all the data, and forget about the oldest one
	for(uint16_t i = dataAmount-1; i > 0; --i) {
		datasets[i] = datasets[i-1];
	}
	datasets[0] = rawData;

	for(uint16_t i = 0; i < dataAmount; ++i) {
		sum += datasets[i];
	}
	avg = sum/dataAmount;
	return avg;
}

void setup() 
{
	// set the LED pins
	pinMode(buttonPin,INPUT);
	pinMode(redPin, OUTPUT);
	pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);
	
	// consider changing the baud rate?
	Serial.begin(9600);

	// oops! you suck and broke something
	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		Serial.println(F("SSD1306 allocation failed"));
		while (1);
	}

	
	display.display(); // NO WAY DISPLAY DISPLAY
	// wait! decrease this if you want
	delay(500);
	display.clearDisplay();
	// testing thing? can i just remove this
	display.drawPixel(10, 10, SSD1306_WHITE);
	display.display();

	ads.setGain(GAIN_TWOTHIRDS);

	// oops! you suck and broke something [part 2]
	if (!ads.begin()) {
		Serial.println("Failed to initialize ADS.");
		while (1);
	}
}

void loop() 
{
	// read ADC
	ADC_in = smoothing(ads.readADC_SingleEnded(0));
	// measurement delay
	delay(DB_MEASUREDELAY);
	
	// reset display
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(0,0);

	// print the millivolts
	display.print("mv ");
	display.println(ads.computeVolts(ADC_in)*1000);
	// print the weight
	display.print("gs ");
	display.print(convertNumber(ADC_in)-zeroVal);
	display.display();
	
	// change LED colors based on the set bounds
	double weight = convertNumber(ADC_in)-zeroVal;
	if      ((DB_BOUNDS[0] < weight) && (weight < DB_BOUNDS[1]))
		setLed('G');
	else if ((DB_BOUNDS[1] < weight) && (weight < DB_BOUNDS[2]))
		setLed('B');
	else if ((DB_BOUNDS[2] < weight) && (weight < DB_BOUNDS[3]))
		setLed('R');
	else ledOff();

	// zero button
	if(digitalRead(buttonPin) == HIGH)  
		zeroVal = convertNumber(ADC_in);
}
