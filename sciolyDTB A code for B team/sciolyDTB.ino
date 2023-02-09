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
static constexpr uint16_t DB_BOUNDS[4] {0, 105, 200, 1000};
static constexpr char DB_BOUNDORDER[3] = {'G','B','R'};

// pins
static constexpr uint8_t redPin    = 5;
static constexpr uint8_t greenPin  = 3;
static constexpr uint8_t bluePin   = 4;
static constexpr uint8_t buttonPin = 7;
#define pinthing A0

/***********************************************/

// screen thingsa
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// e (!!!)
static constexpr float e = 2.71828;

// input from the ADC
static uint16_t ADC_in = 0;
// stores the last 5 data points
static uint16_t dataset[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// zero value
// calibration!!!
static float zeroVal = 70;

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
  double dataFinal = data;
  
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
  uint16_t dataAmount = 20;
  uint16_t sum = 0;
  float avg = 0;

  // push back all the data, and forget about the oldest one
  for(uint16_t i = dataAmount-1; i > 0; --i) {
    dataset[i] = dataset[i-1];
  }
  dataset[0] = rawData;

  for(uint16_t i = 0; i < dataAmount; ++i) {
    sum += dataset[i];
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
}

void loop() 
{
  // read ADC
  ADC_in = analogRead(pinthing);
  // measurement delay
  delay(DB_MEASUREDELAY);
  
  // reset display
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  // woohoo! actual thing
  double weight = convertNumber(ADC_in)-zeroVal;
  if (weight < 0) weight = 0;

  // print the millivolts
  display.print("mv ");
  display.println(ADC_in);
  // print the weight
  display.print("gs ");
  display.print(weight);
  display.display();
  
  // change LED colors based on the set bounds
  if      ((DB_BOUNDS[0] < weight) && (weight < DB_BOUNDS[1]))
    setLed(DB_BOUNDORDER[0]);
  else if ((DB_BOUNDS[1] < weight) && (weight < DB_BOUNDS[2]))
    setLed(DB_BOUNDORDER[1]);
  else if ((DB_BOUNDS[2] < weight) && (weight < DB_BOUNDS[3]))
    setLed(DB_BOUNDORDER[2]);
  else setLed('X');

  // zero button
  if(digitalRead(buttonPin) == HIGH)  
    zeroVal = convertNumber(ADC_in);
}
