/*
Simple Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
  LEFT_MOTOR = 2,
  RIGHT_MOTOR = 6,
};

// init servo library
Servo servo_left;
Servo servo_right;

// motors calibration value
allMotors motorCalibration = {2, -2};

// init servo motors
void initServo(void) {
  // init motors one by one
  servo_left.attach(LEFT_MOTOR, 500, 2500);
  servo_left.write(90);
  delay(100);
  servo_right.attach(RIGHT_MOTOR, 500, 2500);
  servo_right.write(90);
  delay(100);
}

// set servo motors
void setServo(void) {
  // set motors values after calibration
  servo_left.write(90 + motorCalibration.left);
  delay(100);
  servo_right.write(90 - motorCalibration.right);
  delay(500);
}

// move motors.
void updateServo(allMotors motorValue) {
  servo_left.write(90 + motorValue.left + motorCalibration.left);
  servo_right.write(90 - motorValue.right - motorCalibration.right);
}
