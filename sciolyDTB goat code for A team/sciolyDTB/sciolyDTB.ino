#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Math.h>

//OLED Setup info
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//ADC setup
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1015 ads;

//Setting up ints to store values
int smoothRead = 0;
int avgVals[] = {0, 0, 0, 0, 0};

#define redPin 4
#define greenPin 3
#define bluePin 2

#define buttonPin 7
double tare = 50;

float e =  2.71828;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin,INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(500);
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();

  ads.setGain(GAIN_TWOTHIRDS);
  
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //read the ADC and then 
  smoothRead = smoothing(ads.readADC_SingleEnded(0));
  //Serial.println(smoothRead);
  //small delay so the mesurments are diffrent
  delay(10);

  //an easy way to zero out the scale
  //if(digitalRead(buttonPin) == HIGH){
  //  tare = convertNumber(smoothRead);
  //}
  
  //mostly boilerplate code to clear the display and then write text to the display
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("mv ");
  display.println(ads.computeVolts(smoothRead)*1000);//display the reading from the adc without any changes
  display.print("gs ");
  display.print(convertNumber(smoothRead)-tare);//find the weight but subtract what has been zeroed out
  display.display();

  rangeLol(0, "b", 105, "r", 445, "g", 1000);
}

//returns the grams using the line of best fit
double convertNumber(int data){
  //amazing conversion being done
  //e^{((x/525)+(691/175))}
  double addedVal = 3.9486;
  float dataDiv = (float)data/525;
  Serial.print(data);
  Serial.print("    ");
  Serial.print(dataDiv);
  Serial.print("    ");
  double eRaised = exp(dataDiv+addedVal);
  Serial.println(eRaised);
  return eRaised;
  //return data;
}

//turns on the led of a certain color and turns the rest off
void redLed(){
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}

void greenLed() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, LOW);
}

void ledOff(){
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}

void blueLed() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);
}

//set the ranges for the leds to turn off and on
void rangeLol(int bound1, String c1, int bound2, String c2, int bound3, String c3, int bound4){
  double weight = convertNumber(smoothRead)-tare;
  if(bound1 < weight && weight < bound2){
    colorChoice(c1);
  }else if(bound2 < weight && weight < bound3){
    colorChoice(c2);
  }else if(bound3 < weight && weight < bound4){
    colorChoice(c3);
  }else{
    ledOff();
  }
}

//parse the color choosen
void colorChoice(String color){
  if(color == "r"){
    redLed();
  }else if(color == "g"){
    Serial.println(color);
    greenLed();
  }else if(color == "b"){
    blueLed();
  }
}

//find the avg of the last 5 data points
//I love this int!
int smoothing(int rawData){
  int numberOfData = 5;
  int sum = 0;
  int avg = 0;
  for(int i=numberOfData-1; i > 0; i --){
    avgVals[i]=avgVals[i-1];
  }
  avgVals[0]=rawData;
  
  for(int i = 0; i < numberOfData; i ++){
    sum = sum + avgVals[i];
  }
  avg = sum/numberOfData;
  return avg;
}
