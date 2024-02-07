/*
Board Lowrie 1.1 demo code
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Uses ultrasonic sensor HC-SR04 
 - Gets distance in cm
*/

// output pin
const int triggerPin = 7;   // connector S6
// input pin 
const int echoPin = 8;      // connector S7

// init ultrasonic sesnsor
void initUltrasonic(void) {
  pinMode(triggerPin, OUTPUT); 
	pinMode(echoPin, INPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
}

// read distance to sensor in cm
unsigned short readUltrasonic(void) {
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10000);
  digitalWrite(triggerPin, LOW);
  return (unsigned short)(pulseIn(echoPin, HIGH) / 40);
}