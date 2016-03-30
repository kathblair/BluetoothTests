#include <Adafruit_NeoPixel.h>

#define SERVOABSMIN  60 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOABSMAX  250 // Not sure about this
#define SERVOMIDDLE 128
#define SERVOPIN  11 // the pin
#define MUSMIN 60
#define MUSMAX 170

#define PIN 5
#define Pixels 21
#define STARTPIXEL 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(Pixels, PIN, NEO_GRB + NEO_KHZ800);

float redStates[Pixels];
float blueStates[Pixels];
float greenStates[Pixels];
float fadeRate = 0.96;

int pushButton = 2;

boolean state = 0; // 1 is high, 0 is low, start low.

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
  
  // put your setup code here, to run once:
  pinMode(SERVOPIN, OUTPUT);
  
  analogWrite(SERVOPIN, MUSMIN);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  for(uint16_t l = 0; l < Pixels; l++) {
    redStates[l] = 0;
    greenStates[l] = 0;
    blueStates[l] = 0;
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  // let's set the state first and then do the other stuff
  if(state==0){
    lowstate();
  }
  
  if(state==1){
    highstate();
  }
  
  int buttonState = digitalRead(pushButton);
  
  if(buttonState == 1){
    Serial.println("button press");
    switch(state){
      case 1:
        // was already high, set to low.
        state = 0;
        break;
      case 0:
        // was already low, set to high.
        state = 1;
        break;
    }
  }
}

void lowstate() {
  Serial.println("Low State");
  pixoff();  
  analogWrite(SERVOPIN, MUSMIN);
  delay(5000);
 }
 
 void highstate() {
   Serial.println("High State");
   analogWrite(SERVOPIN, MUSMAX);
   rainbowCycle(5);
 }
 
 void pixoff(){
   uint16_t n = strip.numPixels();
   for(int i=STARTPIXEL; i<n; i++){
     strip.setPixelColor(i, 0, 0, 0);
   }
   strip.show();
 }
 
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=STARTPIXEL; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
