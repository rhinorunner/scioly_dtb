#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED Setup info
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//ADC setup
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1015 ads;

//Setting up ints to store values
int smoothRead = 0;
int avgVals[] = {0, 0, 0, 0, 0};

#define redPin 3
#define greenPin 4
#define bluePin 5

void setup() {
  // put your setup code here, to run once:
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

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //read the ADC and then 
  smoothRead = smoothing(ads.readADC_SingleEnded(0));
  Serial.println(smoothRead);
  
  delay(10);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("mv ");
  display.println(ads.computeVolts(smoothRead));
  display.print("Gs ");
  display.print(convertNumber(smoothRead));
  display.display();
}


//gonna add a legit funcion at some point
double convertNumber(int data){
  return data*2;
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

void blueLed() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);
}

//set the ranges for the leds to turn off and on
void rangeLol(int bound1, char c1, int bound2, char c2, int bound3, char c3, int bound4){
  double weight = convertNumber(reading);
  if(bound1 < weight && weight < bound2){
    colorChoice(c1);
  }else if(bound2 < weight && weight < bound3){
    colorChoice(c2);
  }else if(bound3 < weight && weight < bound4){
    colorChoice(c3);
  }
}

//parse the color choosen
void colorChoice(char color){
  if(color == "r"){
    redLed();
  }else if(color == "g"){
    greenLed();
  }else if(color == "b"){
    blueLed();
  }
}

//find the avg of the last 5 data points
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
