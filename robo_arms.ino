/************************************************************************
 * robo_arms
 * 
 * AUTHOR: Brandon Arnold
 * CREATED: 16:15, 14 April 2016
 * DESCRIPTION: This program will be running on an Arduino Uno equipped w/
 *   an Adafruit Servo/PWM shield capable of controlling up to 16 servos or
 *   PWM channels. The program is meant to control 3 robotic arms (3 servos
 *   each) each equipped w/ 1 grabber (1 servo each) to grab or place PVC 
 *   rings with an external diameter of 2.375". 
 *   
 *   This robot will be competing as a part of Cal Poly's Roborodentia XXI
 *   which is a competition put on every year by the Roborodentia club as a
 *   part of Cal Poly's Open House, in particular as a part of the CPE/EE
 *   showcase.
 *   
 *   This is the main driving program for the robotic arms that detects pin
 *   change interrupts and performs a certain routine according the the pin 
 *   that the change is detected on.
 *   
 * CONSIDERATIONS: 
 *   - The servos will potentially draw a significant amount of current 
 *     when moving all at the same time. This current will be investigated
 *     at several times by using a current-monitoring power supply to power
 *     the servo shield
 *   - It may be worth investigating whether to move the servos more slowly
 *     than their maximum speed in order to not jeopardize the physical arm
 *     and it's stability.
 * 
 * NOTES:
 *   - Some of this code is adopted from the example provided as a part of
 *     the Adafruit library associated with the Adafruit Servo/PWM shield.
 *   - The number associated with the servos will increase from the base of
 *     the robotic arm to the tip. The first 'shoulder' servos will thus be
 *     numbered 0,4,8; the second 'elbow' servos will be 1,5,9; the third
 *     'wrist' servos will be numbered 2,6,10; finally, the servos driving
 *     the grabbers will be designated 3,7,11.
 *   - The servos responsible for turning the right and left arms will be
 *     numbered 12 (left) and 13 (right).
 * 
 *************************************************************************/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
#define SERVOMID  375 // approximate middle of the range shown above

#define SHOULDER_HIGH 40
#define SHOULDER_LOW 130
#define WRIST_HIGH 60
#define WRIST_LOW 140
#define GRABBER_OPEN 90
#define GRABBER_CLOSE 50

//MANA'S CHANGES 
#define SPIDERHANDS 2

// Macro for mapping angle values to a pulse length
#define convert(x) map(x,0,180,SERVOMIN,SERVOMAX)

// global servo # counter
uint8_t servonum = 0;

// States
enum State { ringsGrabbed, emptyHands };
State currState = emptyHands;

void setup() {
  // No serial communication in the actual run code
  pwm.begin();
  Serial.begin(9600);
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  pinMode(SPIDERHANDS, INPUT); 
  centerPlace();
 // pinMode(13, OUTPUT); 
//  digitalWrite(SPIDERHANDS, HIGH); 
  //enableInterrupt(SPIDERHANDS, positionArms, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(SPIDERHANDS),centerGrab, RISING);
  //attachInterrupt(digitalPinToInterrupt(SPIDERHANDS),centerPlace, FALLING);
  //yield();
}

void loop() {
  if(digitalRead(SPIDERHANDS) == HIGH) {
    positionArms(); 
    delay(1);
  }

}

/*
 * positionArms() takes an array of integers as an argument which represents
 *   the position in degrees of each servo channel [0-15] on the servo shield.
 *   The degree values can be inclusively within the range of 0-180 and will
 *   be written in as global constants for the four different states that the 
 *   robo arms will potentially be in (grab rings - close, grab rings - far,
 *   place rings - close, place rings - far).
 *   
 *   CONSIDERATIONS: I should consider when I am placing or grabbing rings
 *   because each case may require moving the servos in separate orders.
 */
void positionArms(){
  //noInterrupts(); 
  Serial.println("new");
  delay(70);
  //Argument was int servoarms[]
// ALL OF THE BELOW CODE IS DEPRECATED FOR THE TIME BEING BECAUSE ROBOTS
//  int pulseLen[16]; // array for pulse values for use with setPWM()
//
//  // map all degree values to pulse values
//  for(int i = 0; i < 16; i++){
//    pulseLen[i] = map(servoDegrees[i], 0, 180, SERVOMIN, SERVOMAX);
//  }
//  // first rotate the servos which control the turning of the left/right arms
//  pwm.setPWM(12, 0, pulseLen[12]); // left arm
//  pwm.setPWM(13, 0, pulseLen[13]); // right arm
//
//  // move each arm individually and delay a bit after adjusting the servos
//  //  in an arm
//  for(int j = 0; j < 12; j++){
//    if(j > 0 && (j % 4 == 0)){ // If we're about to start the 2nd/3rd arm
//      delay(500); // wait a 1/2 second
//    }
//    pwm.setPWM(j, 0, pulseLen[j]);
//  }


  if(currState == ringsGrabbed){
    centerPlace();
    currState = emptyHands;
  }
  else if(currState == emptyHands){
    centerGrab();
    currState = ringsGrabbed;
  }
  
  //interrupts(); 
}



/* 
 * The servos that are a part of the center arm reside on shield PWM channels
 * 4 (shoulder), 5 (wrist), and 6 (grabber).
 * 
 * NOTE: delays are probably unnecessary.
 * SHOULDER HIGH POINT: 40
 * SHOULDER LOW POINT: 130
 * WRIST LOW POINT: 90
 * WRIST HIGH : 30
 * GRABBER OPEN: 90
 * GRABBER CLOSE: 60
 */
void centerGrab() {
  Serial.print("grabbing \n"); 
  pwm.setPWM(4, 0, convert(SHOULDER_LOW)); delay(500);
  //pwm.setPWM(5, 0, convert(WRIST_LOW)); delay(150);
  pwm.setPWM(6, 0, convert(GRABBER_CLOSE)); delay(750);

  // Lift after grabbing
  slowLiftOrDrop(1);
}

void centerPlace() {
  Serial.print("placeingn");   
  slowLiftOrDrop(0);
  //pwm.setPWM(5, 0, convert(WRIST_HIGH)); delay(150);
  pwm.setPWM(6, 0, convert(GRABBER_OPEN)); delay(750);

  pwm.setPWM(4, 0, convert(SHOULDER_HIGH)); delay(750);
}

void slowLiftOrDrop(byte lift) { // for handling rings
  unsigned int i, wristAngle;
  
  if(lift){
    wristAngle = WRIST_LOW;
    for(i = SHOULDER_LOW; i >= SHOULDER_HIGH; i-=3){
        pwm.setPWM(4,0,convert(i)); 
        pwm.setPWM(5,0,convert(wristAngle-=2));
        delay(35);
     }
  }
  else{
    wristAngle = WRIST_HIGH;
     for(i = SHOULDER_HIGH; i <= SHOULDER_LOW; i+=3){
        
        pwm.setPWM(4,0,convert(i)); 
        pwm.setPWM(5,0,convert(wristAngle+=2));
        delay(35);
     }
  }
  //pwm.setPWM(5,0,25);
}

