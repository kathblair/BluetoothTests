//For uno with JACKET
#include <Adafruit_NeoPixel.h>

#define SERVOABSMIN  60 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOABSMAX  250 // Not sure about this
#define SERVOMIDDLE 128
#define SERVOPIN  11 // the pin
#define JMIN 60
#define JMAX 170
#define HIGHLOWSERVO 2
#define HIGHLOWLIGHTS 3

#define Pixels 60
#define STARTPIXEL 0

float redStates[Pixels];
float blueStates[Pixels];
float greenStates[Pixels];
float fadeRate = 0.96;

#define PIN 5   //the Xadow Duino used DIN/D5/PC6 to control the WS2812b

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
    // make the pushbutton's pin an input:
    pinMode(HIGHLOWLIGHTS, INPUT);
    pinMode(HIGHLOWSERVO, INPUT);
    
    // put your setup code here, to run once:
    pinMode(SERVOPIN, OUTPUT);
    pinMode(PIN, OUTPUT);
        
    analogWrite(SERVOPIN, JMIN);
    
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    
   for(uint16_t l = 0; l < Pixels; l++) {
    redStates[l] = 0;
    greenStates[l] = 0;
    blueStates[l] = 0;
  }
}

void loop() {
  if(digitalRead(HIGHLOWSERVO)==0){
    Serial.println("Low servo");
    analogWrite(SERVOPIN, JMIN);
    delay(10000);
  }
  if(digitalRead(HIGHLOWSERVO)==1){
    Serial.println("high servo");
    analogWrite(SERVOPIN, JMAX);
    delay(10000);
  }
  if(digitalRead(HIGHLOWLIGHTS)==0){
    Serial.println("Low light");
    pixoff();
  }
  if(digitalRead(HIGHLOWLIGHTS)==1){
    strip.setBrightness(255);
    rainbowCycle(5);
    //twinkle();
  }
  

}


void pixoff(){
   uint16_t n = strip.numPixels();
   for(int i=STARTPIXEL; i<n; i++){
     strip.setPixelColor(i, 0, 0, 0);
     //strip.begin();
     strip.setBrightness(0);
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


