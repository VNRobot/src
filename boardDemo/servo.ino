/*
Board Lowrie 1.1 demo code
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
 - Connect servo motors to S2, S5 and S9 connectors
 - Servo connected to S5 and S9 will move repetative
 - Servo connected to S2 will move when electronic board is rotated horizontally
*/

#include <Servo.h>

// pin numbers for servo motors
enum m_dPins {
  S1_PIN = 2,   // D2
  S2_PIN = 3,   // D3
  S3_PIN = 4,   // D4
  S4_PIN = 5,   // D5
  S5_PIN = 6,   // D6
  S6_PIN = 7,   // D7
  S7_PIN = 8,   // D8
  S8_PIN = 9,   // D9
  S9_PIN = 10,  // D10
  S10_PIN = 11, // D11
  S11_PIN = 12, // D12
  S12_PIN = 13  // D13
};

// for demo using only 3 servo motors out of 12
//Servo S1; // IR receiver can use only D2 input. Comment if using IR receiver
Servo S2;
//Servo S3;
//Servo S4;
Servo S5;
//Servo S6;
//Servo S7;
//Servo S8;
Servo S9;
//Servo S10;
//Servo S11;
//Servo S12;

// variable for motor angle
char motorAngle = 0;

// init servo motors
void initServo(void) {
  // init servo motors one by one
  // S2
  S2.attach(S2_PIN, 500, 2500);
  S2.write(90);
  delay(100);
  // S5
  S5.attach(S5_PIN, 500, 2500);
  S5.write(90);
  delay(100);
  // S9
  S9.attach(S9_PIN, 500, 2500);
  S9.write(90);
  delay(100);
}

// update servo motors values. move motors.
void updateServo(int direction) {
  // set motors angle values
  S2.write(90 + direction * 2);
  S5.write(90 + motorAngle * 2);
  S9.write(90 + motorAngle * 3);
  motorAngle += 2;
  if (motorAngle > 10) {
    motorAngle = -10;
  }
}