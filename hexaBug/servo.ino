/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
  FL1_MOTOR = 2,
  FL2_MOTOR = 3,
  FR1_MOTOR = 4,
  FR2_MOTOR = 5,
  SL1_MOTOR = 6,
  SL2_MOTOR = 7,
  SR1_MOTOR = 8,
  SR2_MOTOR = 9,
  RL1_MOTOR = 10,
  RL2_MOTOR = 11,
  RR1_MOTOR = 12,
  RR2_MOTOR = 13
};

// init servo library
Servo servo_fl_1;
Servo servo_fl_2;
Servo servo_fr_1;
Servo servo_fr_2;
Servo servo_sl_1;
Servo servo_sl_2;
Servo servo_sr_1;
Servo servo_sr_2;
Servo servo_rl_1;
Servo servo_rl_2;
Servo servo_rr_1;
Servo servo_rr_2;

// init servo motors
void initServo(int calM1, int calM2) {
  // init motor pins
  // motor connection pins
  //   f,  r, fl1, fl2, fr1, fr2, rl1, rl2, rr1, rr2
  //   3,  4,   5,   6,   7,   8,   9,  10,  11,  12
  // 
  // init motors one by one
  servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  servo_fl_1.write(90 - calM1);
  delay(100);
  servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  servo_fl_2.write(90 - calM2);
  delay(100);
  servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  servo_fr_1.write(90 + calM1);
  delay(100);
  servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  servo_fr_2.write(90 + calM2);
  delay(100);
  servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  servo_rl_1.write(90 - calM1);
  delay(100);
  servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  servo_rl_2.write(90 - calM2);
  delay(100);
  servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  servo_rr_1.write(90 + calM1);
  delay(100);
  servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  servo_rr_2.write(90 + calM2);
  delay(100);
  servo_sl_1.attach(SL1_MOTOR, 500, 2500);
  servo_sl_1.write(90 - calM1);
  delay(100);
  servo_sl_2.attach(SL2_MOTOR, 500, 2500);
  servo_sl_2.write(90 - calM2);
  delay(100);
  servo_sr_1.attach(SR1_MOTOR, 500, 2500);
  servo_sr_1.write(90 + calM1);
  delay(100);
  servo_sr_2.attach(SR2_MOTOR, 500, 2500);
  servo_sr_2.write(90 + calM2);
  delay(100);
}

// set servo motors
void setServo(allMotors * calibration, int calM1, int calM2) {
  // set motors values after calibration
  servo_fl_1.write(90 + calibration->m.fl.motor1 - calM1);
  servo_fl_2.write(90 - calibration->m.fl.motor2 - calM2);
  servo_fr_1.write(90 - calibration->m.fr.motor1 + calM1);
  servo_fr_2.write(90 + calibration->m.fr.motor2 + calM2);
  servo_sl_1.write(90 + calibration->m.sl.motor1 - calM1);
  servo_sl_2.write(90 - calibration->m.sl.motor2 - calM2);
  servo_sr_1.write(90 - calibration->m.sr.motor1 + calM1);
  servo_sr_2.write(90 + calibration->m.sr.motor2 + calM2);
  servo_rl_1.write(90 + calibration->m.rl.motor1 - calM1);
  servo_rl_2.write(90 - calibration->m.rl.motor2 - calM2);
  servo_rr_1.write(90 - calibration->m.rr.motor1 + calM1);
  servo_rr_2.write(90 + calibration->m.rr.motor2 + calM2);
}

// move motors.
void updateServo(allMotors motorValue) {
  servo_fl_1.write(90 + motorValue.m.fl.motor1);
  servo_fl_2.write(90 - motorValue.m.fl.motor2);
  servo_fr_1.write(90 - motorValue.m.fr.motor1);
  servo_fr_2.write(90 + motorValue.m.fr.motor2);
  servo_sl_1.write(90 + motorValue.m.sl.motor1);
  servo_sl_2.write(90 - motorValue.m.sl.motor2);
  servo_sr_1.write(90 - motorValue.m.sr.motor1);
  servo_sr_2.write(90 + motorValue.m.sr.motor2);
  servo_rl_1.write(90 + motorValue.m.rl.motor1);
  servo_rl_2.write(90 - motorValue.m.rl.motor2);
  servo_rr_1.write(90 - motorValue.m.rr.motor1);
  servo_rr_2.write(90 + motorValue.m.rr.motor2);
}

