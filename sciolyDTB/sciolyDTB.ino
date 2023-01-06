//oled libs
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//12 bit libs
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1015 ads;

constexpr uint8_t READY_PIN = 3;

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

volatile bool new_data = false;
void IRAM_ATTR NewDataReadyISR() {
  new_data = true;
}

// CALIBRATION
// set up to work like this, where y = grams and x = sensor input
// y = a(b(x+c))+d
// y = a*log_b(x+c)+d  (forgot if this is how log parent function is)
constexpr float CAL_A = 1.5;
constexpr float CAL_B = 1.5;
constexpr float CAL_C = 1.5;
constexpr float CAL_D = 1.5;

// PINS
// sensor pad in
constexpr uint8_t PIN_PADIN = A0;

//led pins
constexpr uint8_t greenPin = 13;
constexpr uint8_t redPin = 12;
constexpr uint8_t bluePin = 11;

void setup () {
  pinMode(greenPin,OUTPUT);
  pinMode(redPin,OUTPUT);
  pinMode(bluePin,OUTPUT);

 
  Serial.begin(115200);
  //display setup
 if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  
  //ads starter
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  pinMode(READY_PIN, INPUT);
  // We get a falling edge every time a new sample is ready.
  attachInterrupt(digitalPinToInterrupt(READY_PIN), NewDataReadyISR, FALLING);

  // Start continuous conversions.
  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_DIFF_0_1, /*continuous=*/true);
 
}

void loop () {
	if (!new_data) {
    return;
  }
  // does this not return float?
  int16_t results = ads.getLastConversionResults();

  // whats this?
	//idk its in the example code tho
  Serial.print("Differential: "); Serial.print(results); Serial.print("("); Serial.print(ads.computeVolts(results)); Serial.println("mV)");
  
  new_data = false;

  // In a real application we probably don't want to do a delay here if we are doing interrupt-based sampling, but we have a delay
  // in this example to avoid writing too much data to the serial port.
  delay(1000);

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  //***change the mv if you change the gain***
  display.println("mv: "+(results * 3));
  display.println("grams: ");
  display.println(grams(results));
  display.display();
}

void redOut(){
  digitalWrite(redPin,HIGH);
  digitalWrite(greenPin,LOW);
  digitalWrite(bluePin,LOW);
}

void greenOut(){
  digitalWrite(redPin,LOW);
  digitalWrite(greenPin,HIGH);
  digitalWrite(bluePin,LOW);
}

void blueOut(){
  digitalWrite(redPin,LOW);
  digitalWrite(greenPin,LOW);
  digitalWrite(bluePin,HIGH);
}

double grams(const int16_t& mv){
  //put the real function here
  return CAL_A * ( CAL_B * ( mv + CAL_C ) ) + CAL_D;
  //return CAL_A * (log(mv) / log(CAL_B) + CAL_C) + CAL_D;
}
