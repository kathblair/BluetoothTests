// THIS IS THE JACKET
#include <Wire.h>

/*TIMING MARKS:
0 - 30 seconds: VIDEO
30 seconds: Both on runway, Rachel will stay there
0:32 Chelsea starts walking (bass)
0:50 Chelsea halfway, pauses, rachel starts moving
1:10 Cheslea end and poses, turns to face Rachel
1:15 MOVEMENT UP - A CUE FOR CODE 
  At 75 seconds.
1:30 - move together slowly 
1:35 - Cross over, start to switch places, LIGHTSSSS CUE FOR CODE
  95 seconds 
1:40 at opposite places, move together
1:55 Start moving back to runway, move to side, pose, MOVEMENT DOWN SERVO CUE
  115 seconds
2:10 Chelsea at end of runway, move to side, pose - DO NOT STOP AT MIDDLE ON WAY BACK)
2:15 Rachel move to side, pose - DO NOT STOP AT MIDDLE ON WAY BACK)
2:20 lights OFF / fade
  140 seconds lights off.
*/

#define RGB_WS2812_Address	35

float startmillis;
float servoupinterval = 75000;
float lightsoninterval = 95000;
float servodowninterval = 115000;
float lightsoffinterval = 140000;
float endofshow = 150000;


//for servo
#define SERVOCOM 1
#define LIGHTSCOM 3

//int servoPin = 1;
// TO DO
// place servos and figure out the 

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);

    // put your setup code here, to run once:
    //pinMode(servoPin, OUTPUT);
    pinMode(SERVOCOM, OUTPUT);
    digitalWrite(SERVOCOM, LOW);
    //analogWrite(servoPin, JACKMIN);
    

}

void loop() {

}

void demotiming(){
  float startmillis = millis();
  while ((millis() >= startmillis+servoupinterval) && (millis()<(startmillis+lightsoninterval))) {
    servoup();
  } 
  while ((millis() >= startmillis+lightsoninterval) && (millis()<(startmillis+servodowninterval))) {
    lightson();
  } 
  while ((millis() >= startmillis+servodowninterval) && (millis()<(startmillis+lightsoffinterval))) {
    servodown();
  } 
  while ((millis() >= startmillis+lightsoffinterval) && (millis()<(startmillis+endofshow))) {
    lightsoff();
  }
  //delay(10000); // till the end of the runway show
  
}

void lightson(){
  digitalWrite(LIGHTSCOM, HIGH);
  /*
  Wire.begin();
  Wire.beginTransmission(RGB_WS2812_Address);
  Wire.write("1");        // sends one byte
  Wire.endTransmission();
  */
}

void lightsoff(){
  digitalWrite(LIGHTSCOM, LOW);
  /*
  Wire.begin();
  Wire.beginTransmission(RGB_WS2812_Address);
  Wire.write("0");        // sends one byte
  Wire.endTransmission();
  */
}

void servoup(){
  digitalWrite(SERVOCOM, HIGH);
}

void servodown(){
  digitalWrite(SERVOCOM, LOW);
}
