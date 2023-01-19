
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define analogPin A3
int reading = 0;

#define redPin 3
#define greenPin 4
#define bluePin 5

void setup() {
  // put your setup code here, to run once:
  pinMode(A0,INPUT);
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
}

void loop() {
  // put your main code here, to run repeatedly:
  //reading = analogRead(analogPin);
  smoothRead = smoothing(analogRead(analogPin));
  Serial.println(smoothRead);
  delay(10);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("mv ");
  display.println(reading*4.88);
  display.print("Gs ");
  display.print(convertNumber(smoothRead));
  display.display();
}

double convertNumber(int data){
  return data*2;
}

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

void colorChoice(char color){
  if(color == "r"){
    redLed();
  }else if(color == "g"){
    greenLed();
  }else if(color == "b"){
    blueLed();
  }
}

int smoothing(int rawData){
  int numberOfData = 10;
  int avgVals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int sum = 0;
  int avg = 0;
  for(int i=numberOfData; i > 1; i --){
    avgVals[i]=avgVals[i-1];
  }
  avgVals[0]=rawData;
  
  for(int i = 0; i < numberOfData; i ++){
    sum = sum + avgVals[i];
  }
  avg = sum/numberOfData;
  delay(1);
  return avg;
}
