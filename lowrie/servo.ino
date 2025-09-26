/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

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
// center motors direction. false for lowrie 1.1
bool centerMotorReverse = false;
// motor enabled flag
bool motorEnabled[12] = {false, false, false, false, false, false, false, false, false, false, false, false};
// timer counter state
short timeCounter = 0;
short timeCounterEnd = 20000; // usec

// calculate motor angles
char _calculateMotorAngle(int Hval, int Sval) {
  // calculate Lvalue and angles
  float Lval = (float)Hval;
  float AngleC = 0;
  float AngleB = 0;
  float AngleA = 0;
  Sval -= 16;
  Lval = sqrt(Hval * Hval + Sval * Sval);
  AngleC = (acos(Hval / Lval) * 180) / 3.14;
  if (Sval < 0) {
    AngleC = -AngleC;
  }
  //AngleB = (acos((Lval * Lval + 70 * 70 - 100 * 100) / (2 * Lval * 70)) * 180) / 3.14;
  AngleB = (acos((Lval * Lval - 5100) / (Lval * 140)) * 180) / 3.14;
  AngleA = 90 - AngleB - AngleC;
  return (char)AngleA;
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
    char mValue1 = _calculateMotorAngle((int)hightSetValue, -(int)forwardSetValue);
    char mValue2 = _calculateMotorAngle((int)hightSetValue, (int)forwardSetValue);
    if (m_init.motorsCount == 12) {
      m_motorAngleValue[0] = _limitMotorValue(90 - 30 + mValue1 + calibration.hl.motor1);
      m_motorAngleValue[1] = _limitMotorValue(90 + 30 - mValue2 - calibration.hl.motor2);
      m_motorAngleValue[2] = _limitMotorValue(90 + 30 - mValue1 - calibration.hr.motor1);
      m_motorAngleValue[3] = _limitMotorValue(90 - 30 + mValue2 + calibration.hr.motor2);
    }
    m_motorAngleValue[4] = _limitMotorValue(90 - 30 + mValue1 + calibration.fl.motor1);
    m_motorAngleValue[5] = _limitMotorValue(90 + 30 - mValue2 - calibration.fl.motor2);
    m_motorAngleValue[6] = _limitMotorValue(90 + 30 - mValue1 - calibration.fr.motor1);
    m_motorAngleValue[7] = _limitMotorValue(90 - 30 + mValue2 + calibration.fr.motor2);
    m_motorAngleValue[8] = _limitMotorValue(90 - 30 + mValue1 + calibration.rl.motor1);
    m_motorAngleValue[9] = _limitMotorValue(90 + 30 - mValue2 - calibration.rl.motor2);
    m_motorAngleValue[10] = _limitMotorValue(90 + 30 - mValue1 - calibration.rr.motor1);
    m_motorAngleValue[11] = _limitMotorValue(90 - 30 + mValue2 + calibration.rr.motor2);
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
  char calM1 = _calculateMotorAngle((int)hight, -(int)forward);
  char calM2 = _calculateMotorAngle((int)hight, (int)forward);
  // set motors value
  if (m_init.motorsCount == 12) {
    m_motorAngleValue[0] = _limitMotorValue(90 - 30 + calM1 + calibration.hl.motor1);
    m_motorAngleValue[1] = _limitMotorValue(90 + 30 - calM2 - calibration.hl.motor2);
    m_motorAngleValue[2] = _limitMotorValue(90 + 30 - calM1 - calibration.hr.motor1);
    m_motorAngleValue[3] = _limitMotorValue(90 - 30 + calM2 + calibration.hr.motor2);
  } else if (m_init.motorsCount == 10) {
    if (centerMotorReverse) {
      m_motorAngleValue[2] = _limitMotorValue(90 + calibration.hr.motor1);
      m_motorAngleValue[3] = _limitMotorValue(90 + calibration.hr.motor2);
    } else {
      m_motorAngleValue[2] = _limitMotorValue(90 - calibration.hr.motor1);
      m_motorAngleValue[3] = _limitMotorValue(90 - calibration.hr.motor2);
    }
  }
  m_motorAngleValue[4] = _limitMotorValue(90 - 30 + calM1 + calibration.fl.motor1);
  m_motorAngleValue[6] = _limitMotorValue(90 + 30 - calM1 - calibration.fr.motor1);
  m_motorAngleValue[8] = _limitMotorValue(90 - 30 + calM1 + calibration.rl.motor1);
  m_motorAngleValue[10] = _limitMotorValue(90 + 30 - calM1 - calibration.rr.motor1);
  m_motorAngleValue[5] = _limitMotorValue(90 + 30 - calM2 - calibration.fl.motor2);
  m_motorAngleValue[7] = _limitMotorValue(90 - 30 + calM2 + calibration.fr.motor2);
  m_motorAngleValue[9] = _limitMotorValue(90 + 30 - calM2 - calibration.rl.motor2);
  m_motorAngleValue[11] = _limitMotorValue(90 - 30 + calM2 + calibration.rr.motor2);
}

// init servo motors
void initServo(void) {
  // init motors one by one
  if (m_init.motorsCount == 12) {
    //pinMode(HL1_MOTOR, OUTPUT);
    m_servo_hl_1.attach(HL1_MOTOR, 500, 2500);
    motorEnabled[0] = true;
    doPWMServo(100);
    //pinMode(HR1_MOTOR, OUTPUT);
    m_servo_hr_1.attach(HR1_MOTOR, 500, 2500);
    motorEnabled[2] = true;
    doPWMServo(100);
    //pinMode(HL2_MOTOR, OUTPUT);
    m_servo_hl_2.attach(HL2_MOTOR, 500, 2500);
    motorEnabled[1] = true;
    doPWMServo(100);
    //pinMode(HR2_MOTOR, OUTPUT);
    m_servo_hr_2.attach(HR2_MOTOR, 500, 2500);
    motorEnabled[3] = true;
    doPWMServo(100);
  } else if (m_init.motorsCount == 10) {
    //pinMode(HR1_MOTOR, OUTPUT);
    m_servo_hr_1.attach(HR1_MOTOR, 500, 2500);
    motorEnabled[2] = true;
    doPWMServo(100);
    //pinMode(HR2_MOTOR, OUTPUT);
    m_servo_hr_2.attach(HR2_MOTOR, 500, 2500);
    motorEnabled[3] = true;
    doPWMServo(100);
  }
  //pinMode(FL1_MOTOR, OUTPUT);
  m_servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  motorEnabled[4] = true;
  doPWMServo(100);
  //pinMode(FR1_MOTOR, OUTPUT);
  m_servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  motorEnabled[6] = true;
  doPWMServo(100);
  //pinMode(RL1_MOTOR, OUTPUT);
  m_servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  motorEnabled[8] = true;
  doPWMServo(100);
  //pinMode(RR1_MOTOR, OUTPUT);
  m_servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  motorEnabled[10] = true;
  doPWMServo(100);
  //pinMode(FL2_MOTOR, OUTPUT);
  m_servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  motorEnabled[5] = true;
  doPWMServo(100);
  //pinMode(FR2_MOTOR, OUTPUT);
  m_servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  motorEnabled[7] = true;
  doPWMServo(100);
  //pinMode(RL2_MOTOR, OUTPUT);
  m_servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  motorEnabled[9] = true;
  doPWMServo(100);
  //pinMode(RR2_MOTOR, OUTPUT);
  m_servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  motorEnabled[11] = true;
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
      motorEnabled[0] = false;
      motorEnabled[1] = false;
      m_servo_hl_1.detach();
      m_servo_hl_2.detach();
    } else if (m_init.motorsCount >= 10) {
      motorEnabled[2] = false;
      motorEnabled[3] = false;
      m_servo_hr_1.detach();
      m_servo_hr_2.detach();
    }
    motorEnabled[4] = false;
    motorEnabled[6] = false;
    motorEnabled[8] = false;
    motorEnabled[10] = false;
    motorEnabled[5] = false;
    motorEnabled[7] = false;
    motorEnabled[9] = false;
    motorEnabled[11] = false;
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
void setServo(allMotors calibration, short hight, char forward) {
  // set motors values after calibration
  if (m_init.motorsCount == 10) {
    if (centerMotorReverse) {
      m_motorAngleValue[2] = _limitMotorValue(90 + calibration.hr.motor1);
      m_motorAngleValue[3] = _limitMotorValue(90 + calibration.hr.motor2);
    } else {
      m_motorAngleValue[2] = _limitMotorValue(90 - calibration.hr.motor1);
      m_motorAngleValue[3] = _limitMotorValue(90 - calibration.hr.motor2);
    }
  }
  _moveLinearServo(calibration, hight, forward);
}

// move center motors.
void updateCenterServo(allMotors calibration, motors centerMotors) {
  if (m_init.motorsCount == 10) {
    if (centerMotorReverse) {
      m_motorAngleValue[2] = _limitMotorValue(90 + centerMotors.motor1 + calibration.hr.motor1);
      m_motorAngleValue[3] = _limitMotorValue(90 + centerMotors.motor2 + calibration.hr.motor2);
    } else {
      m_motorAngleValue[2] = _limitMotorValue(90 - (centerMotors.motor1 + calibration.hr.motor1));
      m_motorAngleValue[3] = _limitMotorValue(90 - (centerMotors.motor2 + calibration.hr.motor2));
    }
  }
}

// move leg motors.
void updateLegsServo(allMotors calibration, allLegs legValue) {
  if (m_init.motorsCount == 12) {
    m_motorAngleValue[0] = _limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.hl.hight + staticBallance.hl.hight, -legValue.hl.shift - staticBallance.hl.shift) + calibration.hl.motor1));
    m_motorAngleValue[1] = _limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.hl.hight + staticBallance.hl.hight, legValue.hl.shift + staticBallance.hl.shift) + calibration.hl.motor2));
    m_motorAngleValue[2] = _limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.hr.hight + staticBallance.hr.hight, -legValue.hr.shift - staticBallance.hr.shift) + calibration.hr.motor1));
    m_motorAngleValue[3] = _limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.hr.hight + staticBallance.hr.hight, legValue.hr.shift + staticBallance.hr.shift)  + calibration.hr.motor2));
  }
  m_motorAngleValue[4] = _limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.fl.hight + staticBallance.fl.hight, -legValue.fl.shift - staticBallance.fl.shift) + calibration.fl.motor1));
  m_motorAngleValue[5] = _limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.fl.hight + staticBallance.fl.hight, legValue.fl.shift + staticBallance.fl.shift)  + calibration.fl.motor2));
  m_motorAngleValue[6] = _limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.fr.hight + staticBallance.fr.hight, -legValue.fr.shift - staticBallance.fr.shift) + calibration.fr.motor1));
  m_motorAngleValue[7] = _limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.fr.hight + staticBallance.fr.hight, legValue.fr.shift + staticBallance.fr.shift)  + calibration.fr.motor2));
  m_motorAngleValue[8] = _limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.rl.hight + staticBallance.rl.hight, -legValue.rl.shift - staticBallance.rl.shift) + calibration.rl.motor1));
  m_motorAngleValue[9] = _limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.rl.hight + staticBallance.rl.hight, legValue.rl.shift + staticBallance.rl.shift)  + calibration.rl.motor2));
  m_motorAngleValue[10] = _limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.rr.hight + staticBallance.rr.hight, -legValue.rr.shift - staticBallance.rr.shift) + calibration.rr.motor1));
  m_motorAngleValue[11] = _limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.rr.hight + staticBallance.rr.hight, legValue.rr.shift + staticBallance.rr.shift)  + calibration.rr.motor2));
}

// do servo pwm cycle 500, 2500
void doPWMServo(short pulseCount) {
  // time keeping
  //if (timeCounter >= timeCounterEnd) {
  //  timeCounter = pulseCount * 1000;
  //}

  // start pulse
  //delayMicroseconds(500);

  // center pulse
  //delayMicroseconds(1000);

  if (motorEnabled[0]) {
    m_servo_hl_1.write(m_motorAngleValue[0]);
  }
  if (motorEnabled[1]) {
    m_servo_hl_2.write(m_motorAngleValue[1]);
  }
  if (motorEnabled[2]) {
    m_servo_hr_1.write(m_motorAngleValue[2]);
  }
  if (motorEnabled[3]) {
    m_servo_hr_2.write(m_motorAngleValue[3]);
  }
  if (motorEnabled[4]) {
    m_servo_fl_1.write(m_motorAngleValue[4]);
  }
  if (motorEnabled[5]) {
    m_servo_fl_2.write(m_motorAngleValue[5]);
  }
  if (motorEnabled[6]) {
    m_servo_fr_1.write(m_motorAngleValue[6]);
  }
  if (motorEnabled[7]) {
    m_servo_fr_2.write(m_motorAngleValue[7]);
  }
  if (motorEnabled[8]) {
    m_servo_rl_1.write(m_motorAngleValue[8]);
  }
  if (motorEnabled[9]) {
    m_servo_rl_2.write(m_motorAngleValue[9]);
  }
  if (motorEnabled[10]) {
    m_servo_rr_1.write(m_motorAngleValue[10]);
  }
  if (motorEnabled[11]) {
    m_servo_rr_2.write(m_motorAngleValue[11]);
  }

  // center pulse
  //delayMicroseconds(18500);
  //delay(20);

  if (pulseCount > 0) {
    delay(pulseCount);
  }

  /*
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
  //for (pulse = 0; pulse++; pulse < pulseCount) {
    // set all to high
    for (motorNumber = 0; motorNumber++; motorNumber < 12) {
      if (motorEnabled[motorNumber]) {
        //digitalWrite(motorNumber + 2, HIGH);
      }
    }
    delay(2);
    for (motorNumber = 0; motorNumber++; motorNumber < 12) {
      if (motorEnabled[motorNumber]) {
        //digitalWrite(motorNumber + 2, LOW);
      }
    }
    delay(1);
    
    // start pulse
    delayMicroseconds(500);
    // check end of pulse
    for (angle = 0; angle++; angle < 181) {
      for (motorNumber = 0; motorNumber++; motorNumber < 12) {
        if (motorEnabled[motorNumber]) {
          if (m_motorAngleValue[motorNumber] == angle) {
            digitalWrite(motorNumber + 2, LOW);
          }
        }
      }
      delayMicroseconds(11);
    }
      
    // end pulse
    //delayMicroseconds(18000);
    delay(18);
  //}
  */
}

void updateBallanceServo(allLegs cBallance, char forwardCenter) {
  if (m_init.motorsCount == 12) {
    staticBallance.hl.hight = cBallance.hl.hight;
    staticBallance.hl.shift = forwardCenter + cBallance.hl.shift;
    staticBallance.hr.hight = cBallance.hr.hight;
    staticBallance.hr.shift = forwardCenter + cBallance.hr.shift;
  }
  staticBallance.fl.hight = cBallance.fl.hight;
  staticBallance.fl.shift = forwardCenter + cBallance.fl.shift;
  staticBallance.fr.hight = cBallance.fr.hight;
  staticBallance.fr.shift = forwardCenter + cBallance.fr.shift;
  staticBallance.rl.hight = cBallance.rl.hight;
  staticBallance.rl.shift = forwardCenter + cBallance.rl.shift;
  staticBallance.rr.hight = cBallance.rr.hight;
  staticBallance.rr.shift = forwardCenter + cBallance.rr.shift;
}
