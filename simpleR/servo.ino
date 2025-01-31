/*
Ladybug Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
  LEFT_MOTOR = 6,
  RIGHT_MOTOR = 10,
  WL_MOTOR = 4,
  WR_MOTOR = 5
};

// init servo library
Servo servo_left;
Servo servo_right;
Servo servo_wl;
Servo servo_wr;

bool wingAttached = false;

// init servo motors
void initServo(int calM1, int calM2) {
  // init motors one by one
  servo_left.attach(LEFT_MOTOR, 500, 2500);
  servo_left.write(90 - calM1);
  delay(100);
  servo_right.attach(RIGHT_MOTOR, 500, 2500);
  servo_right.write(90 + calM2);
  delay(100);
  wingAttached = true;
  servo_wl.attach(WL_MOTOR, 500, 2500);
  servo_wl.write(90);
  delay(100);
  servo_wr.attach(WR_MOTOR, 500, 2500);
  servo_wr.write(90);
  delay(100);
}

// set servo motors
void setServo(allMotors * calibration, int calM1, int calM2) {
  // set motors values after calibration
  if (wingAttached) {
    servo_left.write(90 - calibration->left - calM1);
    servo_right.write(90 + calibration->right + calM2);
  }
  servo_wl.write(90 + calibration->m.wheel.motorL);
  servo_wr.write(90 - calibration->m.wheel.motorR);
}

// move motors.
void updateServo(allMotors motorValue) {
  if (wingAttached) {
    servo_left.write(90 - motorValue.left);
    servo_right.write(90 + motorValue.right);
  }
  servo_wl.write(90 + motorValue.m.wheel.motorL);
  servo_wr.write(90 - motorValue.m.wheel.motorR);
}

// attach wings
void attachWingServo(void) {
  if (! wingAttached) {
    servo_left.attach(LEFT_MOTOR, 500, 2500);
    servo_right.attach(RIGHT_MOTOR, 500, 2500);
  }
  wingAttached = true;
}

// attach wings
void detachWingServo(void) {
  if (wingAttached) {
    servo_left.detach();
    servo_right.detach();
  }
  wingAttached = false;
}
