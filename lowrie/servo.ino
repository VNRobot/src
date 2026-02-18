/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
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
Servo m_servo_fl_1;
Servo m_servo_fl_2;
Servo m_servo_fr_1;
Servo m_servo_fr_2;
Servo m_servo_rl_1;
Servo m_servo_rl_2;
Servo m_servo_rr_1;
Servo m_servo_rr_2;

// static ballance
allLegs staticBallance = {0, 0, 0, 0, 0, 0, 0, 0};
// motors attached flag
bool attached = false;
// motors init values
short hightSetValueL = 0;
short hightSetValueR = 0;
char forwardSetValueL = 0;
char forwardSetValueR = 0;

// calculate motor 1 and motor 2 angles
char _calculateMotorAngle(int Hval, int Sval, char motorNum) {
  // used for run, bend or roll
  //if (m_robotState.surfaceAngleDevider > 0) {
  //  // run or bend
  //  Hval += Sval / m_robotState.surfaceAngleDevider;
  //} else if (m_robotState.surfaceAngleDevider < 0) {
  //  // roll
  //  Hval += (Sval * Sval) / m_robotState.surfaceAngleDevider;
  //}
  //
  float Lval = (float)Hval;
  float AngleB = 0;
  float AngleC = 0;
  // for first motor mirror shift
  if (motorNum == 1) {
    Sval = -Sval;
  }
  // calculate Lvalue and angles
  Sval -= 12; // nalf distance between motors in mm
  Lval = sqrt(Hval * Hval + Sval * Sval);
  AngleC = (acos(Hval / Lval) * 180) / 3.14;
  if (Sval < 0) {
    AngleC = -AngleC;
  }
  //AngleB = (acos((Lval * Lval + 70 * 70 - 100 * 100) / (2 * Lval * 70)) * 180) / 3.14;    5100
  //AngleB = (acos((Lval * Lval + 70 * 70 - 104 * 104) / (2 * Lval * 70)) * 180) / 3.14;    5916
  AngleB = (acos((Lval * Lval - 5916) / (Lval * 140)) * 180) / 3.14;
  return (char)(90 - AngleB - AngleC);
}

// linear motor move
void _moveLinearServo(short hi1, char fw1, short hi2, char fw2, short timeDelay) {
  bool doLoop = true;
  while (doLoop) {
    //
    if (hightSetValueL > hi1) {
      hightSetValueL --;
    } else if (hightSetValueL < hi1) {
      hightSetValueL ++;
    }
    if (hightSetValueR > hi2) {
      hightSetValueR --;
    } else if (hightSetValueR < hi2) {
      hightSetValueR ++;
    }
    //
    if (forwardSetValueL > fw1) {
      forwardSetValueL --;
    } else if (forwardSetValueL < fw1) {
      forwardSetValueL ++;
    }
    if (forwardSetValueR > fw2) {
      forwardSetValueR --;
    } else if (forwardSetValueR < fw2) {
      forwardSetValueR ++;
    }
    if ((forwardSetValueL == fw1) && (hightSetValueL == hi1) && (forwardSetValueR == fw2) && (hightSetValueR == hi2)) {
      doLoop = false;
    }
    char mValue1 = _calculateMotorAngle((int)hightSetValueL, (int)forwardSetValueL, 1);
    char mValue2 = _calculateMotorAngle((int)hightSetValueL, (int)forwardSetValueL, 2);
    m_motorAngleValue[0] = _limitMotorValue(90 + m_calibrationData.fl.motor1 + m_robotState.flipStateNow * mValue1);
    m_motorAngleValue[1] = _limitMotorValue(90 - m_calibrationData.fl.motor2 - m_robotState.flipStateNow * mValue2);
    m_motorAngleValue[4] = _limitMotorValue(90 + m_calibrationData.rl.motor1 + m_robotState.flipStateNow * mValue1);
    m_motorAngleValue[5] = _limitMotorValue(90 - m_calibrationData.rl.motor2 - m_robotState.flipStateNow * mValue2);
    mValue1 = _calculateMotorAngle((int)hightSetValueR, (int)forwardSetValueR, 1);
    mValue2 = _calculateMotorAngle((int)hightSetValueR, (int)forwardSetValueR, 2);
    m_motorAngleValue[2] = _limitMotorValue(90 - m_calibrationData.fr.motor1 - m_robotState.flipStateNow * mValue1);
    m_motorAngleValue[3] = _limitMotorValue(90 + m_calibrationData.fr.motor2 + m_robotState.flipStateNow * mValue2);
    m_motorAngleValue[6] = _limitMotorValue(90 - m_calibrationData.rr.motor1 - m_robotState.flipStateNow * mValue1);
    m_motorAngleValue[7] = _limitMotorValue(90 + m_calibrationData.rr.motor2 + m_robotState.flipStateNow * mValue2);
    doPWMServo(timeDelay);
  }
}

// fast motor move
void _moveServo(short hi1, char fw1, short hi2, char fw2) {
  forwardSetValueL = fw1;
  hightSetValueL = hi1;
  forwardSetValueR = fw2;
  hightSetValueR = hi2;
  char mValue1 = _calculateMotorAngle((int)hightSetValueL, (int)forwardSetValueL, 1);
  char mValue2 = _calculateMotorAngle((int)hightSetValueL, (int)forwardSetValueL, 2);
  m_motorAngleValue[0] = _limitMotorValue(90 + m_calibrationData.fl.motor1 + m_robotState.flipStateNow * mValue1);
  m_motorAngleValue[1] = _limitMotorValue(90 - m_calibrationData.fl.motor2 - m_robotState.flipStateNow * mValue2);
  m_motorAngleValue[4] = _limitMotorValue(90 + m_calibrationData.rl.motor1 + m_robotState.flipStateNow * mValue1);
  m_motorAngleValue[5] = _limitMotorValue(90 - m_calibrationData.rl.motor2 - m_robotState.flipStateNow * mValue2);
  mValue1 = _calculateMotorAngle((int)hightSetValueR, (int)forwardSetValueR, 1);
  mValue2 = _calculateMotorAngle((int)hightSetValueR, (int)forwardSetValueR, 2);
  m_motorAngleValue[2] = _limitMotorValue(90 - m_calibrationData.fr.motor1 - m_robotState.flipStateNow * mValue1);
  m_motorAngleValue[3] = _limitMotorValue(90 + m_calibrationData.fr.motor2 + m_robotState.flipStateNow * mValue2);
  m_motorAngleValue[6] = _limitMotorValue(90 - m_calibrationData.rr.motor1 - m_robotState.flipStateNow * mValue1);
  m_motorAngleValue[7] = _limitMotorValue(90 + m_calibrationData.rr.motor2 + m_robotState.flipStateNow * mValue2);
  doPWMServo(500);
}

// limit angle value
short _limitMotorValue(short mAngle) {
  if (mAngle > 180) {
    mAngle = 180;
  } else if (mAngle < 0) {
    mAngle = 0;
  }
  return mAngle;
}

// init servo motors
void initServo(void) {
  // set motors value
  m_motorAngleValue[0] = _limitMotorValue(90 + m_calibrationData.fl.motor1);
  m_motorAngleValue[1] = _limitMotorValue(90 - m_calibrationData.fl.motor2);
  m_motorAngleValue[2] = _limitMotorValue(90 - m_calibrationData.fr.motor1);
  m_motorAngleValue[3] = _limitMotorValue(90 + m_calibrationData.fr.motor2);
  m_motorAngleValue[4] = _limitMotorValue(90 + m_calibrationData.rl.motor1);
  m_motorAngleValue[5] = _limitMotorValue(90 - m_calibrationData.rl.motor2);
  m_motorAngleValue[6] = _limitMotorValue(90 - m_calibrationData.rr.motor1);
  m_motorAngleValue[7] = _limitMotorValue(90 + m_calibrationData.rr.motor2);
  // init motors one by one
  m_servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  doPWMServo(100);
  m_servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  doPWMServo(100);
  m_servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  doPWMServo(100);
  m_servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  doPWMServo(100);
  m_servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  doPWMServo(100);
  m_servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  doPWMServo(100);
  m_servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  doPWMServo(100);
  m_servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  doPWMServo(100);
  attached = true;
}

// detach hardware
void detachServo(void) {
  if (attached) {
    m_servo_fl_1.detach();
    m_servo_fr_1.detach();
    m_servo_rl_1.detach();
    m_servo_rr_1.detach();
    m_servo_fl_2.detach();
    m_servo_fr_2.detach();
    m_servo_rl_2.detach();
    m_servo_rr_2.detach();
  }
  attached = false;
  doPWMServo(1000);
}

// set servo motors
void setServo(short hightL, short hightR, short timeDelay) {
  if (attached) {
    if (timeDelay == 0) {
      // set motors values fast
      _moveServo(hightL, FORWARD_BALLANCE_SHIFT, hightR, FORWARD_BALLANCE_SHIFT);
    } else {
      // set motors values slow
      _moveLinearServo(hightL, FORWARD_BALLANCE_SHIFT, hightR, FORWARD_BALLANCE_SHIFT, timeDelay);
    }
  }
}

// move leg motors.
void updateLegsServo(allLegs legValue) {
  m_motorAngleValue[0] = _limitMotorValue(90 + m_calibrationData.fl.motor1 + m_robotState.flipStateNow * (_calculateMotorAngle(legValue.fl.hight + staticBallance.fl.hight, legValue.fl.shift + staticBallance.fl.shift, 1)));
  m_motorAngleValue[1] = _limitMotorValue(90 - m_calibrationData.fl.motor2 - m_robotState.flipStateNow * (_calculateMotorAngle(legValue.fl.hight + staticBallance.fl.hight, legValue.fl.shift + staticBallance.fl.shift, 2)));
  m_motorAngleValue[2] = _limitMotorValue(90 - m_calibrationData.fr.motor1 - m_robotState.flipStateNow * (_calculateMotorAngle(legValue.fr.hight + staticBallance.fr.hight, legValue.fr.shift + staticBallance.fr.shift, 1)));
  m_motorAngleValue[3] = _limitMotorValue(90 + m_calibrationData.fr.motor2 + m_robotState.flipStateNow * (_calculateMotorAngle(legValue.fr.hight + staticBallance.fr.hight, legValue.fr.shift + staticBallance.fr.shift, 2)));
  m_motorAngleValue[4] = _limitMotorValue(90 + m_calibrationData.rl.motor1 + m_robotState.flipStateNow * (_calculateMotorAngle(legValue.rl.hight + staticBallance.rl.hight, legValue.rl.shift + staticBallance.rl.shift, 1)));
  m_motorAngleValue[5] = _limitMotorValue(90 - m_calibrationData.rl.motor2 - m_robotState.flipStateNow * (_calculateMotorAngle(legValue.rl.hight + staticBallance.rl.hight, legValue.rl.shift + staticBallance.rl.shift, 2)));
  m_motorAngleValue[6] = _limitMotorValue(90 - m_calibrationData.rr.motor1 - m_robotState.flipStateNow * (_calculateMotorAngle(legValue.rr.hight + staticBallance.rr.hight, legValue.rr.shift + staticBallance.rr.shift, 1)));
  m_motorAngleValue[7] = _limitMotorValue(90 + m_calibrationData.rr.motor2 + m_robotState.flipStateNow * (_calculateMotorAngle(legValue.rr.hight + staticBallance.rr.hight, legValue.rr.shift + staticBallance.rr.shift, 2)));
}

// do servo pwm cycle 500, 2500
void doPWMServo(short timeDelay) {
  m_servo_fl_1.write(m_motorAngleValue[0]);
  m_servo_fl_2.write(m_motorAngleValue[1]);
  m_servo_fr_1.write(m_motorAngleValue[2]);
  m_servo_fr_2.write(m_motorAngleValue[3]);
  m_servo_rl_1.write(m_motorAngleValue[4]);
  m_servo_rl_2.write(m_motorAngleValue[5]);
  m_servo_rr_1.write(m_motorAngleValue[6]);
  m_servo_rr_2.write(m_motorAngleValue[7]);
  if (timeDelay > 0) {
    delay(timeDelay);
  }
}

void updateBallanceServo(void) {
  staticBallance.fl.hight = m_legCorrect.fl.hight;
  staticBallance.fl.shift = FORWARD_BALLANCE_SHIFT + m_legCorrect.fl.shift;
  staticBallance.fr.hight = m_legCorrect.fr.hight;
  staticBallance.fr.shift = FORWARD_BALLANCE_SHIFT + m_legCorrect.fr.shift;
  staticBallance.rl.hight = m_legCorrect.rl.hight;
  staticBallance.rl.shift = FORWARD_BALLANCE_SHIFT + m_legCorrect.rl.shift;
  staticBallance.rr.hight = m_legCorrect.rr.hight;
  staticBallance.rr.shift = FORWARD_BALLANCE_SHIFT + m_legCorrect.rr.shift;
}
