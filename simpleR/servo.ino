/*
Simple Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
  LEFT_MOTOR = 4,
  RIGHT_MOTOR = 5,
  WL_MOTOR = 6,
  WR_MOTOR = 7
};

// init servo library
Servo servo_left;
Servo servo_right;
Servo servo_wl;
Servo servo_wr;

// init servo motors
void initServo(int calM1, int calM2) {
  // init motors one by one
  servo_left.attach(LEFT_MOTOR, 500, 2500);
  servo_left.write(90);
  delay(100);
  servo_right.attach(RIGHT_MOTOR, 500, 2500);
  servo_right.write(90);
  delay(100);
  servo_wl.attach(WL_MOTOR, 500, 2500);
  servo_wl.write(90 - calM1);
  delay(100);
  servo_wr.attach(WR_MOTOR, 500, 2500);
  servo_wr.write(90 - calM2);
  delay(100);
}

// set servo motors
void setServo(allMotors * calibration, int calM1, int calM2) {
  // set motors values after calibration
  servo_left.write(90 - calibration->left);
  servo_right.write(90 - calibration->right);
  servo_wl.write(90 + calibration->m.wheel.motorL - calM1);
  servo_wr.write(90 - calibration->m.wheel.motorR - calM2);
}

// move motors.
void updateServo(allMotors motorValue) {
  servo_left.write(90 - motorValue.left);
  servo_right.write(90 - motorValue.right);
  servo_wl.write(90 + motorValue.m.wheel.motorL);
  servo_wr.write(90 - motorValue.m.wheel.motorR);
}

