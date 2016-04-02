#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define COLOR_PIXEL_ADDR  35 // so just use the dec value of 0x23.
#define BUFFER_SIZE  20
#define DEBUG  1

uint8_t I2C_buffer[BUFFER_SIZE];
uint8_t buffer[BUFFER_SIZE];
uint8_t Char_nu=0;//how many byte received

#define Pixels 60
#define STARTPIXEL 0

uint8_t scan_data[4] = {0x00,0x00,0x00,COLOR_PIXEL_ADDR};
uint8_t scan_count = 0;
boolean flag_data_receive = 0;

String message;

float redStates[Pixels];
float blueStates[Pixels];
float greenStates[Pixels];
float fadeRate = 0.96;

#define PIN 5   //the Xadow Duino used DIN/D5/PC6 to control the WS2812b
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

uint32_t black = strip.Color(0, 0, 0);

void setup()
{
    pinMode(13,OUTPUT);     
    digitalWrite(13,HIGH);
  
    Wire.begin(COLOR_PIXEL_ADDR);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
    
    Serial.begin(9600);           // start serial for output
    Serial.println("Led matrix test.");
    
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    
   for(uint16_t l = 0; l < Pixels; l++) {
    redStates[l] = 0;
    greenStates[l] = 0;
    blueStates[l] = 0;
  }
}

void loop(){
  //Serial.println("Led matrix test.");
  //twinkle();
  //rainbowCycle(5);
}

void receiveEvent(int howMany)
{
  clearBuffer(I2C_buffer,BUFFER_SIZE);
  message = "";
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    //Serial.write(c);         // print the character
    message += c;
  }
  Serial.println(message);
  flag_data_receive = 1;
  if(message == "1"){
    Serial.println("Lights On");
    strip.setBrightness(255);
    rainbowCycle(5);
    //twinkle();
  }
  if(message=="0"){
   Serial.println("Lights off"); 
   pixoff();
  }
}

void requestEvent()
{
    Wire.write(scan_data[scan_count++]);
    if(scan_count >= 4)scan_count = 0;
}

void clearBuffer(uint8_t buff[],uint8_t size)
{
     for (uint8_t i = 0;i < size;i++)
     {
          buff[i]=0x00;
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


