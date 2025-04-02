/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
  FRONT_MOTOR = 4,
  REAR_MOTOR = 5,
  FL1_MOTOR = 6,
  FL2_MOTOR = 7,
  FR1_MOTOR = 8,
  FR2_MOTOR = 9,
  RL1_MOTOR = 10,
  RL2_MOTOR = 11,
  RR1_MOTOR = 12,
  RR2_MOTOR = 13
};

// init servo library
Servo servo_frnt;
Servo servo_rear;
Servo servo_fl_1;
Servo servo_fl_2;
Servo servo_fr_1;
Servo servo_fr_2;
Servo servo_rl_1;
Servo servo_rl_2;
Servo servo_rr_1;
Servo servo_rr_2;

// limit angle value
short limitMotorValue(short mAngle) {
  if (mAngle > 180) {
    mAngle = 180;
  } else if (mAngle < 0) {
    mAngle = 0;
  }
  return mAngle;
}

// init servo motors
void initServo(int calM1, int calM2) {
  // init motor pins
  // motor connection pins
  //   f,  r, fl1, fl2, fr1, fr2, rl1, rl2, rr1, rr2
  //   3,  4,   5,   6,   7,   8,   9,  10,  11,  12
  // 
  // init motors one by one
  servo_frnt.attach(FRONT_MOTOR, 500, 2500);
  servo_frnt.write(90);
  delay(100);
  servo_rear.attach(REAR_MOTOR, 500, 2500);
  servo_rear.write(90);
  delay(100);
  servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  servo_fl_1.write(limitMotorValue(90 - 30 + calM1));
  delay(100);
  servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  servo_fl_2.write(limitMotorValue(90 + 30 - calM2));
  delay(100);
  servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  servo_fr_1.write(limitMotorValue(90 + 30 - calM1));
  delay(100);
  servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  servo_fr_2.write(limitMotorValue(90 - 30 + calM2));
  delay(100);
  servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  servo_rl_1.write(limitMotorValue(90 - 30 + calM1));
  delay(100);
  servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  servo_rl_2.write(limitMotorValue(90 + 30 - calM2));
  delay(100);
  servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  servo_rr_1.write(limitMotorValue(90 + 30 - calM1));
  delay(100);
  servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  servo_rr_2.write(limitMotorValue(90 - 30 + calM2));
  delay(100);
}

// set servo motors
void setServo(allMotors calibration, int calM1, int calM2) {
  // set motors values after calibration
  servo_frnt.write(90 - calibration.m.st.motor1);
  servo_rear.write(90 - calibration.m.st.motor2);
  servo_fl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.m.fl.motor1));
  servo_fl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.m.fl.motor2));
  servo_fr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.m.fr.motor1));
  servo_fr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.m.fr.motor2));
  servo_rl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.m.rl.motor1));
  servo_rl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.m.rl.motor2));
  servo_rr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.m.rr.motor1));
  servo_rr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.m.rr.motor2));
}

// move motors.
void updateServo(allMotors calibration, allMotors motorValue) {
  servo_frnt.write(limitMotorValue(90 - (motorValue.m.st.motor1 + calibration.m.st.motor1)));
  servo_rear.write(limitMotorValue(90 - (motorValue.m.st.motor2 + calibration.m.st.motor2)));
  servo_fl_1.write(limitMotorValue(90 - 30 + (motorValue.m.fl.motor1 + calibration.m.fl.motor1)));
  servo_fl_2.write(limitMotorValue(90 + 30 - (motorValue.m.fl.motor2 + calibration.m.fl.motor2)));
  servo_fr_1.write(limitMotorValue(90 + 30 - (motorValue.m.fr.motor1 + calibration.m.fr.motor1)));
  servo_fr_2.write(limitMotorValue(90 - 30 + (motorValue.m.fr.motor2 + calibration.m.fr.motor2)));
  servo_rl_1.write(limitMotorValue(90 - 30 + (motorValue.m.rl.motor1 + calibration.m.rl.motor1)));
  servo_rl_2.write(limitMotorValue(90 + 30 - (motorValue.m.rl.motor2 + calibration.m.rl.motor2)));
  servo_rr_1.write(limitMotorValue(90 + 30 - (motorValue.m.rr.motor1 + calibration.m.rr.motor1)));
  servo_rr_2.write(limitMotorValue(90 - 30 + (motorValue.m.rr.motor2 + calibration.m.rr.motor2)));
}

