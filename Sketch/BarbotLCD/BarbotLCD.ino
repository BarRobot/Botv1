// Barbot LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Barbot Pins
int pixelpin = 6;

#include <Button.h>
// Buttons
//int sglbtn = A1;
//int dblbtn = A2;
//int gobtn = A3;
Button sglbtn(A1); // Connect your button between pin 2 and GND
Button dblbtn(A2); // Connect your button between pin 3 and GND
Button gobtn(A3); // Connect your button between pin 4 and GND


// Transistor Outputs
int pumpswitch = 8;
int sglswitch = 9;
int dblswitch = 10;
int goswitch = 13;


// Neopixels
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);


byte p20[8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
};
byte p40[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
};
byte p60[8] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
};
byte p80[8] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
};
byte p100[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 100;           // 

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  // Print a message to the LCD.
  lcd.print("Starting...");
  lcd.createChar(0, p20);
  lcd.createChar(1, p40);
  lcd.createChar(2, p60);
  lcd.createChar(3, p80);
  lcd.createChar(4, p100);
  sglbtn.begin();
  dblbtn.begin();
  gobtn.begin();
  pinMode(pumpswitch, OUTPUT);
  pinMode(sglswitch, OUTPUT);
  pinMode(dblswitch, OUTPUT);
  pinMode(goswitch, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  digitalWrite(goswitch, HIGH);
  delay(200);
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  digitalWrite(goswitch, LOW);
  colorWipe(strip.Color(0, 255, 0), 50); // Red

  showIdle();
}

void loop() {
  for(int j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(int i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    if (gobtn.pressed()) {
      pour();
    }
    if (sglbtn.toggled() || dblbtn.toggled()) {
      showIdle();  
    }
    delay(20);
  }
  
}


void pour() {
  int calibrationValue = analogRead(A0);
  int time = map(calibrationValue, 0, 1023, 500, 3000);
  if (sglbtn.read() == Button::PRESSED) {
     lcd.clear();
     lcd.print("Pouring Single");
     colorWipe(strip.Color(0, 0, 255), 50); // Red
     dispense(time);
     showIdle();
  } else if (dblbtn.read() == Button::PRESSED) {
     lcd.clear();
     lcd.print("Double 1/2");
     colorWipe(strip.Color(0, 0, 255), 50); // Red
     dispense(time);
     lcd.clear();
     lcd.print("Double 2/2");
     colorWipe(strip.Color(0, 255, 0), 50); // Red
     delay(100);
     colorWipe(strip.Color(0, 0, 255), 50); // Red
     dispense(time);
     colorWipe(strip.Color(0, 255, 0), 50); // Red
     showIdle();
  } else {
    lcd.clear();
    lcd.print("Select first!");
    delay(1000);
    showIdle();
  }
  
}

void dispense(int time) {
  digitalWrite(goswitch, HIGH);
  digitalWrite(pumpswitch, HIGH);
  for (int i = 0; i<16; i++) {
    for (int j=0; j<5; j++) {
      lcd.setCursor(i, 1);   
      lcd.write(j);
      delay(time/80);
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        digitalWrite(goswitch, ledState);
      }
    }    
  }
  digitalWrite(pumpswitch, LOW);
}

void showIdle() {
  lcd.clear();
  delay(200);
//  lcd.print("Place glass");
  if (sglbtn.read() == Button::PRESSED) {
     lcd.print("Single");
     lcd.setCursor(0, 1);
     lcd.print("Press GO");
  } else if (dblbtn.read() == Button::PRESSED) {
     lcd.print("Double");
     lcd.setCursor(0, 1);
     lcd.print("Press GO");
  } else {
    lcd.print("Select Single");
    lcd.setCursor(0, 1);
    lcd.print("or Double?");
  } 
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
