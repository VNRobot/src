/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

// static ballance
allLegs staticBallance = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// motors attached flag
bool attached = false;
// motors init values
short hightInitValue = 0;
short hightSetValue = 0;

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
void _moveLinearServo(allMotors calibration, short hi1, short hi2) {
  bool doLoop = true;
  while (doLoop) {
    if (hi1 > hi2) {
      hi1 --;
    } else if (hi1 < hi2) {
      hi1 ++;
    } else {
      doLoop = false;
    }
    char mValue = _calculateMotorAngle((int)hi1, 0);
    if (m_motorsCount == 12) {
      m_servo_hl_1.write(_limitMotorValue(90 - 30 + mValue + calibration.hl.motor1));
      m_servo_hl_2.write(_limitMotorValue(90 + 30 - mValue - calibration.hl.motor2));
      m_servo_hr_1.write(_limitMotorValue(90 + 30 - mValue - calibration.hr.motor1));
      m_servo_hr_2.write(_limitMotorValue(90 - 30 + mValue + calibration.hr.motor2));
    }
    m_servo_fl_1.write(_limitMotorValue(90 - 30 + mValue + calibration.fl.motor1));
    m_servo_fl_2.write(_limitMotorValue(90 + 30 - mValue - calibration.fl.motor2));
    m_servo_fr_1.write(_limitMotorValue(90 + 30 - mValue - calibration.fr.motor1));
    m_servo_fr_2.write(_limitMotorValue(90 - 30 + mValue + calibration.fr.motor2));
    m_servo_rl_1.write(_limitMotorValue(90 - 30 + mValue + calibration.rl.motor1));
    m_servo_rl_2.write(_limitMotorValue(90 + 30 - mValue - calibration.rl.motor2));
    m_servo_rr_1.write(_limitMotorValue(90 + 30 - mValue - calibration.rr.motor1));
    m_servo_rr_2.write(_limitMotorValue(90 - 30 + mValue + calibration.rr.motor2));
    delay(20);
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
void initServo(allMotors calibration, short hight) {
  hightInitValue = hight;
  char calM = _calculateMotorAngle((int)hightInitValue, 0);
  // init motors one by one
  if (m_motorsCount == 12) {
    m_servo_hl_1.attach(HL1_MOTOR, 500, 2500);
    m_servo_hl_1.write(_limitMotorValue(90 - 30 + calM + calibration.hl.motor1));
    delay(100);
    m_servo_hr_1.attach(HR1_MOTOR, 500, 2500);
    m_servo_hr_1.write(_limitMotorValue(90 + 30 - calM - calibration.hr.motor1));
    delay(100);
    m_servo_hl_2.attach(HL2_MOTOR, 500, 2500);
    m_servo_hl_2.write(_limitMotorValue(90 + 30 - calM - calibration.hl.motor2));
    delay(100);
    m_servo_hr_2.attach(HR2_MOTOR, 500, 2500);
    m_servo_hr_2.write(_limitMotorValue(90 - 30 + calM + calibration.hr.motor2));
    delay(100);
  } else if (m_motorsCount == 10) {
    m_servo_hr_1.attach(HR1_MOTOR, 500, 2500);
    m_servo_hr_1.write(90 - calibration.hr.motor1);
    delay(100);
    m_servo_hr_2.attach(HR2_MOTOR, 500, 2500);
    m_servo_hr_2.write(90 - calibration.hr.motor2);
    delay(100);
  }
  m_servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  m_servo_fl_1.write(_limitMotorValue(90 - 30 + calM + calibration.fl.motor1));
  delay(100);
  m_servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  m_servo_fr_1.write(_limitMotorValue(90 + 30 - calM - calibration.fr.motor1));
  delay(100);
  m_servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  m_servo_rl_1.write(_limitMotorValue(90 - 30 + calM + calibration.rl.motor1));
  delay(100);
  m_servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  m_servo_rr_1.write(_limitMotorValue(90 + 30 - calM - calibration.rr.motor1));
  delay(100);
  m_servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  m_servo_fl_2.write(_limitMotorValue(90 + 30 - calM - calibration.fl.motor2));
  delay(100);
  m_servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  m_servo_fr_2.write(_limitMotorValue(90 - 30 + calM + calibration.fr.motor2));
  delay(100);
  m_servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  m_servo_rl_2.write(_limitMotorValue(90 + 30 - calM - calibration.rl.motor2));
  delay(100);
  m_servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  m_servo_rr_2.write(_limitMotorValue(90 - 30 + calM + calibration.rr.motor2));
  delay(100);
  attached = true;
  }

// attach wings
void detachServo(allMotors calibration) {
  if (attached) {
    _moveLinearServo(calibration, hightSetValue, hightInitValue);
    if (m_motorsCount == 12) {
      m_servo_hl_1.detach();
      m_servo_hl_2.detach();
    } else if (m_motorsCount >= 10) {
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
  delay(1000);
}

// set servo motors
void setServo(allMotors calibration, short hight) {
  hightSetValue = hight;
  // set motors values after calibration
  if (m_motorsCount == 10) {
    m_servo_hr_1.write(90 - calibration.hr.motor1);
    m_servo_hr_2.write(90 - calibration.hr.motor2);
  }
  _moveLinearServo(calibration, hightInitValue, hightSetValue);
}

// move center motors.
void updateCenterServo(allMotors calibration, motors centerMotors) {
  if (m_motorsCount == 10) {
    m_servo_hr_1.write(_limitMotorValue(90 - (centerMotors.motor1 + calibration.hr.motor1)));
    m_servo_hr_2.write(_limitMotorValue(90 - (centerMotors.motor2 + calibration.hr.motor2)));
  }
}

// move leg motors.
void updateLegsServo(allMotors calibration, allLegs legValue) {
  if (m_motorsCount == 12) {
    m_servo_hl_1.write(_limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.hl.hight + staticBallance.hl.hight, -legValue.hl.shift - staticBallance.hl.shift) + calibration.hl.motor1)));
    m_servo_hl_2.write(_limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.hl.hight + staticBallance.hl.hight, legValue.hl.shift + staticBallance.hl.shift)  + calibration.hl.motor2)));
    m_servo_hr_1.write(_limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.hr.hight + staticBallance.hr.hight, -legValue.hr.shift - staticBallance.hr.shift) + calibration.hr.motor1)));
    m_servo_hr_2.write(_limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.hr.hight + staticBallance.hr.hight, legValue.hr.shift + staticBallance.hr.shift)  + calibration.hr.motor2)));
  }
  m_servo_fl_1.write(_limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.fl.hight + staticBallance.fl.hight, -legValue.fl.shift - staticBallance.fl.shift) + calibration.fl.motor1)));
  m_servo_fl_2.write(_limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.fl.hight + staticBallance.fl.hight, legValue.fl.shift + staticBallance.fl.shift)  + calibration.fl.motor2)));
  m_servo_fr_1.write(_limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.fr.hight + staticBallance.fr.hight, -legValue.fr.shift - staticBallance.fr.shift) + calibration.fr.motor1)));
  m_servo_fr_2.write(_limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.fr.hight + staticBallance.fr.hight, legValue.fr.shift + staticBallance.fr.shift)  + calibration.fr.motor2)));
  m_servo_rl_1.write(_limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.rl.hight + staticBallance.rl.hight, -legValue.rl.shift - staticBallance.rl.shift) + calibration.rl.motor1)));
  m_servo_rl_2.write(_limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.rl.hight + staticBallance.rl.hight, legValue.rl.shift + staticBallance.rl.shift)  + calibration.rl.motor2)));
  m_servo_rr_1.write(_limitMotorValue(90 + 30 - (_calculateMotorAngle(legValue.rr.hight + staticBallance.rr.hight, -legValue.rr.shift - staticBallance.rr.shift) + calibration.rr.motor1)));
  m_servo_rr_2.write(_limitMotorValue(90 - 30 + (_calculateMotorAngle(legValue.rr.hight + staticBallance.rr.hight, legValue.rr.shift + staticBallance.rr.shift)  + calibration.rr.motor2)));
}

void updateBallanceServo(allLegs cBallance) {
  if (m_motorsCount == 12) {
    staticBallance.hl.hight = cBallance.hl.hight;
    staticBallance.hl.shift = cBallance.hl.shift;
    staticBallance.hr.hight = cBallance.hr.hight;
    staticBallance.hr.shift = cBallance.hr.shift;
  }
  staticBallance.fl.hight = cBallance.fl.hight;
  staticBallance.fl.shift = cBallance.fl.shift;
  staticBallance.fr.hight = cBallance.fr.hight;
  staticBallance.fr.shift = cBallance.fr.shift;
  staticBallance.rl.hight = cBallance.rl.hight;
  staticBallance.rl.shift = cBallance.rl.shift;
  staticBallance.rr.hight = cBallance.rr.hight;
  staticBallance.rr.shift = cBallance.rr.shift;
}
