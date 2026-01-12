/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
  HL1_MOTOR = 2,
  HL2_MOTOR = 3,
  HR1_MOTOR = 4,
  HR2_MOTOR = 5,
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
Servo m_servo_hl_1;
Servo m_servo_hl_2;
Servo m_servo_hr_1;
Servo m_servo_hr_2;
Servo m_servo_fl_1;
Servo m_servo_fl_2;
Servo m_servo_fr_1;
Servo m_servo_fr_2;
Servo m_servo_rl_1;
Servo m_servo_rl_2;
Servo m_servo_rr_1;
Servo m_servo_rr_2;

// static ballance
allLegs staticBallance = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// temp values
short angle = 0;
unsigned char pulse = 0;
unsigned char motorNumber = 0;
// motors attached flag
bool attached = false;
// motors init values
short hightInitValue = 0;
short hightSetValue = 0;
char forwardInitValue = 0;
char forwardSetValue = 0;
// motor enabled flag
bool motorEnabled[12] = {false, false, false, false, false, false, false, false, false, false, false, false};
// robot flip. when flipped -1
char flip = 1;

// calculate motor 1 and motor 2 angles
char _calculateMotorAngle(int Hval, int Sval, char motorNum) {
  float Lval = (float)Hval;
  float AngleB = 0;
  float AngleC = 0;
  // not in use for now
  /*
  if (m_init.motorVertical) {
    Lval = sqrt(Hval * Hval + Sval * Sval);
    AngleC = (acos(Hval / Lval) * 180) / 3.14;
    if (Sval < 0) {
      AngleC = -AngleC;
    }
    //AngleB = (acos(Lval / (2 * 70)) * 180) / 3.14;
    AngleB = (acos(Lval / 140) * 180) / 3.14;
    if (motorNum == 1) {
      // bottom motor
      return (char)(AngleB + AngleC);
    } else {
      // top motor
      return (char)(AngleB - AngleC);
    }
  }
  */
  // for first motor mirror shift
  if (motorNum == 1) {
    Sval = -Sval;
  }
  // calculate Lvalue and angles
  Sval -= BETWEEN_MOTORS;
  Lval = sqrt(Hval * Hval + Sval * Sval);
  AngleC = (acos(Hval / Lval) * 180) / 3.14;
  if (Sval < 0) {
    AngleC = -AngleC;
  }
  //AngleB = (acos((Lval * Lval + 70 * 70 - 100 * 100) / (2 * Lval * 70)) * 180) / 3.14;
  AngleB = (acos((Lval * Lval - 5100) / (Lval * 140)) * 180) / 3.14;
  return (char)(90 - AngleB - AngleC);
}

// linear motor move
void _moveLinearServo(allMotors calibration, short hi2, char fw2) {
  bool doLoop = true;
  while (doLoop) {
    if (hightSetValue > hi2) {
      hightSetValue --;
    } else if (hightSetValue < hi2) {
      hightSetValue ++;
    }
    if (forwardSetValue > fw2) {
      forwardSetValue --;
    } else if (forwardSetValue < fw2) {
      forwardSetValue ++;
    }
    if ((forwardSetValue == fw2) && (hightSetValue == hi2)) {
      doLoop = false;
    }
    char mValue1 = _calculateMotorAngle((int)hightSetValue, (int)forwardSetValue, 1);
    char mValue2 = _calculateMotorAngle((int)hightSetValue, (int)forwardSetValue, 2);
    if (m_init.motorsCount == 12) {
      m_motorAngleValue[0] = _limitMotorValue(90 + calibration.hl.motor1 + flip * (MOTOR_CORRECTION + mValue1));
      m_motorAngleValue[1] = _limitMotorValue(90 - calibration.hl.motor2 - flip * (MOTOR_CORRECTION + mValue2));
      m_motorAngleValue[2] = _limitMotorValue(90 - calibration.hr.motor1 - flip * (MOTOR_CORRECTION + mValue1));
      m_motorAngleValue[3] = _limitMotorValue(90 + calibration.hr.motor2 + flip * (MOTOR_CORRECTION + mValue2));
    }
    m_motorAngleValue[4] = _limitMotorValue(90 + calibration.fl.motor1 + flip * (MOTOR_CORRECTION + mValue1));
    m_motorAngleValue[5] = _limitMotorValue(90 - calibration.fl.motor2 - flip * (MOTOR_CORRECTION + mValue2));
    m_motorAngleValue[6] = _limitMotorValue(90 - calibration.fr.motor1 - flip * (MOTOR_CORRECTION + mValue1));
    m_motorAngleValue[7] = _limitMotorValue(90 + calibration.fr.motor2 + flip * (MOTOR_CORRECTION + mValue2));
    m_motorAngleValue[8] = _limitMotorValue(90 + calibration.rl.motor1 + flip * (MOTOR_CORRECTION + mValue1));
    m_motorAngleValue[9] = _limitMotorValue(90 - calibration.rl.motor2 - flip * (MOTOR_CORRECTION + mValue2));
    m_motorAngleValue[10] = _limitMotorValue(90 - calibration.rr.motor1 - flip * (MOTOR_CORRECTION + mValue1));
    m_motorAngleValue[11] = _limitMotorValue(90 + calibration.rr.motor2 + flip * (MOTOR_CORRECTION + mValue2));
    doPWMServo(20);
  }
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
void initValueServo(allMotors calibration, short hight, char forward) {
  hightInitValue = hight;
  forwardInitValue = forward;
  hightSetValue = hight;
  forwardSetValue = forward;
  char calM1 = _calculateMotorAngle((int)hight, (int)forward, 1);
  char calM2 = _calculateMotorAngle((int)hight, (int)forward, 2);
  // set motors value
  if (m_init.motorsCount == 12) {
    m_motorAngleValue[0] = _limitMotorValue(90 + calibration.hl.motor1 + flip * (MOTOR_CORRECTION + calM1));
    m_motorAngleValue[1] = _limitMotorValue(90 - calibration.hl.motor2 - flip * (MOTOR_CORRECTION + calM2));
    m_motorAngleValue[2] = _limitMotorValue(90 - calibration.hr.motor1 - flip * (MOTOR_CORRECTION + calM1));
    m_motorAngleValue[3] = _limitMotorValue(90 + calibration.hr.motor2 + flip * (MOTOR_CORRECTION + calM2));
  } else if (m_init.motorsCount == 10) {
    m_motorAngleValue[2] = _limitMotorValue(90 + CENTER_MOTOR_DIRECTION * calibration.hr.motor1);
    m_motorAngleValue[3] = _limitMotorValue(90 + CENTER_MOTOR_DIRECTION * calibration.hr.motor2);
  }
  m_motorAngleValue[4] = _limitMotorValue(90 + calibration.fl.motor1 + flip * (MOTOR_CORRECTION + calM1));
  m_motorAngleValue[5] = _limitMotorValue(90 - calibration.fl.motor2 - flip * (MOTOR_CORRECTION + calM2));
  m_motorAngleValue[6] = _limitMotorValue(90 - calibration.fr.motor1 - flip * (MOTOR_CORRECTION + calM1));
  m_motorAngleValue[7] = _limitMotorValue(90 + calibration.fr.motor2 + flip * (MOTOR_CORRECTION + calM2));
  m_motorAngleValue[8] = _limitMotorValue(90 + calibration.rl.motor1 + flip * (MOTOR_CORRECTION + calM1));
  m_motorAngleValue[9] = _limitMotorValue(90 - calibration.rl.motor2 - flip * (MOTOR_CORRECTION + calM2));
  m_motorAngleValue[10] = _limitMotorValue(90 - calibration.rr.motor1 - flip * (MOTOR_CORRECTION + calM1));
  m_motorAngleValue[11] = _limitMotorValue(90 + calibration.rr.motor2 + flip * (MOTOR_CORRECTION + calM2));
}

// init servo motors
void initServo(void) {
  // init motors one by one
  if (m_init.motorsCount == 12) {
    m_servo_hl_1.attach(HL1_MOTOR, 500, 2500);
    doPWMServo(100);
    m_servo_hr_1.attach(HR1_MOTOR, 500, 2500);
    doPWMServo(100);
    m_servo_hl_2.attach(HL2_MOTOR, 500, 2500);
    doPWMServo(100);
    m_servo_hr_2.attach(HR2_MOTOR, 500, 2500);
    doPWMServo(100);
  } else if (m_init.motorsCount == 10) {
    m_servo_hr_1.attach(HR1_MOTOR, 500, 2500);
    doPWMServo(100);
    m_servo_hr_2.attach(HR2_MOTOR, 500, 2500);
    doPWMServo(100);
  }
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

// prepare detach
void detachPrepareServo(allMotors calibration) {
  if (attached) {
    _moveLinearServo(calibration, hightInitValue, forwardInitValue);
  }
}

// detach hardware
void detachServo(void) {
  if (attached) {
    if (m_init.motorsCount == 12) {
      m_servo_hl_1.detach();
      m_servo_hl_2.detach();
    }
    if (m_init.motorsCount >= 10) {
      m_servo_hr_1.detach();
      m_servo_hr_2.detach();
    }
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
void setServo(allMotors calibration, short hight) {
  // set motors values after calibration
  if (m_init.motorsCount == 10) {
    m_motorAngleValue[2] = _limitMotorValue(90 + CENTER_MOTOR_DIRECTION * calibration.hr.motor1);
    m_motorAngleValue[3] = _limitMotorValue(90 + CENTER_MOTOR_DIRECTION * calibration.hr.motor2);
  }
  _moveLinearServo(calibration, hight, FORWARD_BALLANCE_SHIFT);
}

// move center motors.
void updateCenterServo(allMotors calibration, motors centerMotors) {
  if (m_init.motorsCount == 10) {
    m_motorAngleValue[2] = _limitMotorValue(90 + CENTER_MOTOR_DIRECTION * (centerMotors.motor1 + calibration.hr.motor1));
    m_motorAngleValue[3] = _limitMotorValue(90 + CENTER_MOTOR_DIRECTION * (centerMotors.motor2 + calibration.hr.motor2));
  }
}

// do flip motors.
void doFlipServo(short hight) {
  if (flip == 1) {
    flip = -1;
  } else {
    flip = 1;
  }
  hightSetValue = hight;
}

// move leg motors.
void updateLegsServo(allMotors calibration, allLegs legValue) {
  if (m_init.motorsCount == 12) {
    m_motorAngleValue[0] = _limitMotorValue(90 + calibration.hl.motor1 + flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.hl.hight + staticBallance.hl.hight, legValue.hl.shift + staticBallance.hl.shift, 1)));
    m_motorAngleValue[1] = _limitMotorValue(90 - calibration.hl.motor2 - flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.hl.hight + staticBallance.hl.hight, legValue.hl.shift + staticBallance.hl.shift, 2)));
    m_motorAngleValue[2] = _limitMotorValue(90 - calibration.hr.motor1 - flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.hr.hight + staticBallance.hr.hight, legValue.hr.shift + staticBallance.hr.shift, 1)));
    m_motorAngleValue[3] = _limitMotorValue(90 + calibration.hr.motor2 + flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.hr.hight + staticBallance.hr.hight, legValue.hr.shift + staticBallance.hr.shift, 2)));
  }
  m_motorAngleValue[4] = _limitMotorValue(90 + calibration.fl.motor1 + flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.fl.hight + staticBallance.fl.hight, legValue.fl.shift + staticBallance.fl.shift, 1)));
  m_motorAngleValue[5] = _limitMotorValue(90 - calibration.fl.motor2 - flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.fl.hight + staticBallance.fl.hight, legValue.fl.shift + staticBallance.fl.shift, 2)));
  m_motorAngleValue[6] = _limitMotorValue(90 - calibration.fr.motor1 - flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.fr.hight + staticBallance.fr.hight, legValue.fr.shift + staticBallance.fr.shift, 1)));
  m_motorAngleValue[7] = _limitMotorValue(90 + calibration.fr.motor2 + flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.fr.hight + staticBallance.fr.hight, legValue.fr.shift + staticBallance.fr.shift, 2)));
  m_motorAngleValue[8] = _limitMotorValue(90 + calibration.rl.motor1 + flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.rl.hight + staticBallance.rl.hight, legValue.rl.shift + staticBallance.rl.shift, 1)));
  m_motorAngleValue[9] = _limitMotorValue(90 - calibration.rl.motor2 - flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.rl.hight + staticBallance.rl.hight, legValue.rl.shift + staticBallance.rl.shift, 2)));
  m_motorAngleValue[10] = _limitMotorValue(90 - calibration.rr.motor1 - flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.rr.hight + staticBallance.rr.hight, legValue.rr.shift + staticBallance.rr.shift, 1)));
  m_motorAngleValue[11] = _limitMotorValue(90 + calibration.rr.motor2 + flip * (MOTOR_CORRECTION + _calculateMotorAngle(legValue.rr.hight + staticBallance.rr.hight, legValue.rr.shift + staticBallance.rr.shift, 2)));
}

// do servo pwm cycle 500, 2500
void doPWMServo(short timeDelay) {
  if (m_init.motorsCount == 12) {
    m_servo_hl_1.write(m_motorAngleValue[0]);
    m_servo_hl_2.write(m_motorAngleValue[1]);
  }
  if (m_init.motorsCount >= 10) {
    m_servo_hr_1.write(m_motorAngleValue[2]);
    m_servo_hr_2.write(m_motorAngleValue[3]);
  }
  m_servo_fl_1.write(m_motorAngleValue[4]);
  m_servo_fl_2.write(m_motorAngleValue[5]);
  m_servo_fr_1.write(m_motorAngleValue[6]);
  m_servo_fr_2.write(m_motorAngleValue[7]);
  m_servo_rl_1.write(m_motorAngleValue[8]);
  m_servo_rl_2.write(m_motorAngleValue[9]);
  m_servo_rr_1.write(m_motorAngleValue[10]);
  m_servo_rr_2.write(m_motorAngleValue[11]);
  if (timeDelay > 0) {
    delay(timeDelay);
  }
}

void updateBallanceServo(allLegs cBallance) {
  if (m_init.motorsCount == 12) {
    staticBallance.hl.hight = cBallance.hl.hight;
    staticBallance.hl.shift = FORWARD_BALLANCE_SHIFT + cBallance.hl.shift;
    staticBallance.hr.hight = cBallance.hr.hight;
    staticBallance.hr.shift = FORWARD_BALLANCE_SHIFT + cBallance.hr.shift;
  }
  staticBallance.fl.hight = cBallance.fl.hight;
  staticBallance.fl.shift = FORWARD_BALLANCE_SHIFT + cBallance.fl.shift;
  staticBallance.fr.hight = cBallance.fr.hight;
  staticBallance.fr.shift = FORWARD_BALLANCE_SHIFT + cBallance.fr.shift;
  staticBallance.rl.hight = cBallance.rl.hight;
  staticBallance.rl.shift = FORWARD_BALLANCE_SHIFT + cBallance.rl.shift;
  staticBallance.rr.hight = cBallance.rr.hight;
  staticBallance.rr.shift = FORWARD_BALLANCE_SHIFT + cBallance.rr.shift;
}
