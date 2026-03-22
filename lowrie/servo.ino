/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

// center position in the leg forward shift. bigger the number more weight on front
#define FORWARD_BALLANCE_SHIFT  0
// motor center position down from horizontal
#define MOTOR_ANGLE             0
// shift to increase distance between front and rear legs
#define TRAPEZ_SHIFT            0

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
Servo servo_fl_1;
Servo servo_fl_2;
Servo servo_fr_1;
Servo servo_fr_2;
Servo servo_rl_1;
Servo servo_rl_2;
Servo servo_rr_1;
Servo servo_rr_2;

// static ballance
allLegs staticBallance = {0, 0, false, 0, 0, false, 0, 0, false, 0, 0, false};
// motors attached flag
bool attached = false;
// motors init values
short hightSetValueL = HIGHT_LOW;
short hightSetValueR = HIGHT_LOW;
char forwardSetValueL = 0;
char forwardSetValueR = 0;
// motors calibration values for all motors
allMotors servoCalibrationData = {0, 0, 0, 0, 0, 0, 0, 0};
// servo motor value
short servoMotorAngleValue[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/*
uses
m_robotState
m_legsValue
m_legCorrect
m_getButtonPressed()
*/

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
  //AngleB = (acos((Lval * Lval + 70 * 70 - 112 * 112) / (2 * Lval * 70)) * 180) / 3.14;    7644
  AngleB = (acos((Lval * Lval - 7644) / (Lval * 140)) * 180) / 3.14;
  return (char)(90 - AngleB - AngleC);
}

// linear motor move
void _moveLinearServo(short hi1, short hi2, short timeDelay) {
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
    if (forwardSetValueL > 0) {
      forwardSetValueL --;
    } else if (forwardSetValueL < 0) {
      forwardSetValueL ++;
    }
    if (forwardSetValueR > 0) {
      forwardSetValueR --;
    } else if (forwardSetValueR < 0) {
      forwardSetValueR ++;
    }
    if ((forwardSetValueL == 0) && (hightSetValueL == hi1) && (forwardSetValueR == 0) && (hightSetValueR == hi2)) {
      doLoop = false;
    }
    _moveServo();
    delay(timeDelay);
  }
}

// fast motor move
void _moveServo(void) {
  char mValue1 = _calculateMotorAngle((int)hightSetValueL, (int)(forwardSetValueL + FORWARD_BALLANCE_SHIFT - TRAPEZ_SHIFT), 1);
  char mValue2 = _calculateMotorAngle((int)hightSetValueL, (int)(forwardSetValueL + FORWARD_BALLANCE_SHIFT - TRAPEZ_SHIFT), 2);
  servoMotorAngleValue[0] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.fl.motor1 + m_robotState.flipStateLNow * mValue1);
  servoMotorAngleValue[1] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.fl.motor2 - m_robotState.flipStateLNow * mValue2);
  mValue1 = _calculateMotorAngle((int)hightSetValueL, (int)(forwardSetValueL + FORWARD_BALLANCE_SHIFT + TRAPEZ_SHIFT), 1);
  mValue2 = _calculateMotorAngle((int)hightSetValueL, (int)(forwardSetValueL + FORWARD_BALLANCE_SHIFT + TRAPEZ_SHIFT), 2);
  servoMotorAngleValue[4] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.rl.motor1 + m_robotState.flipStateLNow * mValue1);
  servoMotorAngleValue[5] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.rl.motor2 - m_robotState.flipStateLNow * mValue2);
  mValue1 = _calculateMotorAngle((int)hightSetValueR, (int)(forwardSetValueR + FORWARD_BALLANCE_SHIFT - TRAPEZ_SHIFT), 1);
  mValue2 = _calculateMotorAngle((int)hightSetValueR, (int)(forwardSetValueR + FORWARD_BALLANCE_SHIFT - TRAPEZ_SHIFT), 2);
  servoMotorAngleValue[2] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.fr.motor1 - m_robotState.flipStateRNow * mValue1);
  servoMotorAngleValue[3] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.fr.motor2 + m_robotState.flipStateRNow * mValue2);
  mValue1 = _calculateMotorAngle((int)hightSetValueR, (int)(forwardSetValueR + FORWARD_BALLANCE_SHIFT + TRAPEZ_SHIFT), 1);
  mValue2 = _calculateMotorAngle((int)hightSetValueR, (int)(forwardSetValueR + FORWARD_BALLANCE_SHIFT + TRAPEZ_SHIFT), 2);
  servoMotorAngleValue[6] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.rr.motor1 - m_robotState.flipStateRNow * mValue1);
  servoMotorAngleValue[7] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.rr.motor2 + m_robotState.flipStateRNow * mValue2);
  doPWMServo();
}

// limit angle value
short _limitMotorValue(short mAngle) {
  if (mAngle > 180) {
    mAngle = 180;
    // todo: reduce robot hight
  } else if (mAngle < 0) {
    mAngle = 0;
    // todo: reduce robot hight
  }
  return mAngle;
}

// init servo motors
void initServo(bool calibrationMode) {
  // set motors value
  servoMotorAngleValue[0] = _limitMotorValue(90 - MOTOR_ANGLE);
  servoMotorAngleValue[1] = _limitMotorValue(90 + MOTOR_ANGLE);
  servoMotorAngleValue[2] = _limitMotorValue(90 + MOTOR_ANGLE);
  servoMotorAngleValue[3] = _limitMotorValue(90 - MOTOR_ANGLE);
  servoMotorAngleValue[4] = _limitMotorValue(90 - MOTOR_ANGLE);
  servoMotorAngleValue[5] = _limitMotorValue(90 + MOTOR_ANGLE);
  servoMotorAngleValue[6] = _limitMotorValue(90 + MOTOR_ANGLE);
  servoMotorAngleValue[7] = _limitMotorValue(90 - MOTOR_ANGLE);
  // init motors one by one
  servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  servo_fl_1.write(servoMotorAngleValue[0]);
  delay(100);
  servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  servo_fr_1.write(servoMotorAngleValue[1]);
  delay(100);
  servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  servo_rl_1.write(servoMotorAngleValue[2]);
  delay(100);
  servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  servo_rr_1.write(servoMotorAngleValue[3]);
  delay(100);
  servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  servo_fl_2.write(servoMotorAngleValue[4]);
  delay(100);
  servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  servo_fr_2.write(servoMotorAngleValue[5]);
  delay(100);
  servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  servo_rl_2.write(servoMotorAngleValue[6]);
  delay(100);
  servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  servo_rr_2.write(servoMotorAngleValue[7]);
  delay(100);
  attached = true;
  // check for calibration mode
  if (calibrationMode) {
    // do calibration
    unsigned char calibrationStage = 0;
    servoCalibrationData.fl.motor1 = 0;
    servoCalibrationData.fl.motor2 = 0;
    servoCalibrationData.fr.motor1 = 0;
    servoCalibrationData.fr.motor2 = 0;
    servoCalibrationData.rl.motor1 = 0;
    servoCalibrationData.rl.motor2 = 0;
    servoCalibrationData.rr.motor1 = 0;
    servoCalibrationData.rr.motor2 = 0;
    // motors one by one
    while (calibrationMode) {
      servoMotorAngleValue[0] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.fl.motor1);
      servoMotorAngleValue[1] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.fl.motor2);
      servoMotorAngleValue[2] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.fr.motor1);
      servoMotorAngleValue[3] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.fr.motor2);
      servoMotorAngleValue[4] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.rl.motor1);
      servoMotorAngleValue[5] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.rl.motor2);
      servoMotorAngleValue[6] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.rr.motor1);
      servoMotorAngleValue[7] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.rr.motor2);
      doPWMServo();
      delay(200);
      // check button press
      if (m_getButtonPressed()) {
        calibrationStage ++;
      }
      switch (calibrationStage) {
        case 0:
          // assembly stage. do nothing
        break;
        case 1:
        {
          servoCalibrationData.fl.motor1 ++;
          if (servoCalibrationData.fl.motor1 > CALIBRATION_ANGLE_MAX) {
            servoCalibrationData.fl.motor1 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 2:
        {
          servoCalibrationData.fl.motor2 ++;
          if (servoCalibrationData.fl.motor2 > CALIBRATION_ANGLE_MAX) {
            servoCalibrationData.fl.motor2 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 3:
        {
          servoCalibrationData.fr.motor1 ++;
          if (servoCalibrationData.fr.motor1 > CALIBRATION_ANGLE_MAX) {
            servoCalibrationData.fr.motor1 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 4:
        {
          servoCalibrationData.fr.motor2 ++;
          if (servoCalibrationData.fr.motor2 > CALIBRATION_ANGLE_MAX) {
            servoCalibrationData.fr.motor2 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 5:
        {
          servoCalibrationData.rl.motor1 ++;
          if (servoCalibrationData.rl.motor1 > CALIBRATION_ANGLE_MAX) {
            servoCalibrationData.rl.motor1 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 6:
        {
          servoCalibrationData.rl.motor2 ++;
          if (servoCalibrationData.rl.motor2 > CALIBRATION_ANGLE_MAX) {
            servoCalibrationData.rl.motor2 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 7:
        {
          servoCalibrationData.rr.motor1 ++;
          if (servoCalibrationData.rr.motor1 > CALIBRATION_ANGLE_MAX) {
            servoCalibrationData.rr.motor1 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 8:
        {
          servoCalibrationData.rr.motor2 ++;
          if (servoCalibrationData.rr.motor2 > CALIBRATION_ANGLE_MAX) {
            servoCalibrationData.rr.motor2 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 9:
        {
          EEPROM.update(1, (unsigned char)(servoCalibrationData.fl.motor1 + 128));
          EEPROM.update(2, (unsigned char)(servoCalibrationData.fl.motor2 + 128));
          EEPROM.update(3, (unsigned char)(servoCalibrationData.fr.motor1 + 128));
          EEPROM.update(4, (unsigned char)(servoCalibrationData.fr.motor2 + 128));
          EEPROM.update(5, (unsigned char)(servoCalibrationData.rl.motor1 + 128));
          EEPROM.update(6, (unsigned char)(servoCalibrationData.rl.motor2 + 128));
          EEPROM.update(7, (unsigned char)(servoCalibrationData.rr.motor1 + 128));
          EEPROM.update(8, (unsigned char)(servoCalibrationData.rr.motor2 + 128));
          calibrationStage ++;
          calibrationMode = false;
        }
        break;
        default:
        break;
      }
    }
  }
  // read calibration
  servoCalibrationData.fl.motor1 = (char)(EEPROM.read(1) - 128);
  servoCalibrationData.fl.motor2 = (char)(EEPROM.read(2) - 128);
  servoCalibrationData.fr.motor1 = (char)(EEPROM.read(3) - 128);
  servoCalibrationData.fr.motor2 = (char)(EEPROM.read(4) - 128);
  servoCalibrationData.rl.motor1 = (char)(EEPROM.read(5) - 128);
  servoCalibrationData.rl.motor2 = (char)(EEPROM.read(6) - 128);
  servoCalibrationData.rr.motor1 = (char)(EEPROM.read(7) - 128);
  servoCalibrationData.rr.motor2 = (char)(EEPROM.read(8) - 128);
  servoMotorAngleValue[0] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.fl.motor1);
  servoMotorAngleValue[1] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.fl.motor2);
  servoMotorAngleValue[2] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.fr.motor1);
  servoMotorAngleValue[3] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.fr.motor2);
  servoMotorAngleValue[4] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.rl.motor1);
  servoMotorAngleValue[5] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.rl.motor2);
  servoMotorAngleValue[6] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.rr.motor1);
  servoMotorAngleValue[7] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.rr.motor2);
  doPWMServo();
  delay(100);
}

// detach hardware
void detachServoZero(void) {
  if (attached) {
    servo_fl_1.detach();
    servo_fr_1.detach();
    servo_rl_1.detach();
    servo_rr_1.detach();
    servo_fl_2.detach();
    servo_fr_2.detach();
    servo_rl_2.detach();
    servo_rr_2.detach();
  }
  attached = false;
  doPWMServo();
  delay(1000);
}

// set servo motors
void setServoZero(short hightL, short hightR, short timeDelay) {
  if (attached) {
    if (timeDelay == 0) {
      // set motors values fast
      forwardSetValueL = 0;
      hightSetValueL = hightL;
      forwardSetValueR = 0;
      hightSetValueR = hightR;
      _moveServo();
      delay(500);
    } else {
      // set motors values slow
      _moveLinearServo(hightL, hightR, timeDelay);
    }
  }
}

// move leg motors.
void updateLegsServo(void) {
  servoMotorAngleValue[0] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.fl.motor1 + m_robotState.flipStateLNow * (_calculateMotorAngle(m_legsValue.fl.hight + staticBallance.fl.hight, m_legsValue.fl.shift + staticBallance.fl.shift, 1)));
  servoMotorAngleValue[1] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.fl.motor2 - m_robotState.flipStateLNow * (_calculateMotorAngle(m_legsValue.fl.hight + staticBallance.fl.hight, m_legsValue.fl.shift + staticBallance.fl.shift, 2)));
  servoMotorAngleValue[2] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.fr.motor1 - m_robotState.flipStateRNow * (_calculateMotorAngle(m_legsValue.fr.hight + staticBallance.fr.hight, m_legsValue.fr.shift + staticBallance.fr.shift, 1)));
  servoMotorAngleValue[3] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.fr.motor2 + m_robotState.flipStateRNow * (_calculateMotorAngle(m_legsValue.fr.hight + staticBallance.fr.hight, m_legsValue.fr.shift + staticBallance.fr.shift, 2)));
  servoMotorAngleValue[4] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.rl.motor1 + m_robotState.flipStateLNow * (_calculateMotorAngle(m_legsValue.rl.hight + staticBallance.rl.hight, m_legsValue.rl.shift + staticBallance.rl.shift, 1)));
  servoMotorAngleValue[5] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.rl.motor2 - m_robotState.flipStateLNow * (_calculateMotorAngle(m_legsValue.rl.hight + staticBallance.rl.hight, m_legsValue.rl.shift + staticBallance.rl.shift, 2)));
  servoMotorAngleValue[6] = _limitMotorValue(90 + MOTOR_ANGLE - servoCalibrationData.rr.motor1 - m_robotState.flipStateRNow * (_calculateMotorAngle(m_legsValue.rr.hight + staticBallance.rr.hight, m_legsValue.rr.shift + staticBallance.rr.shift, 1)));
  servoMotorAngleValue[7] = _limitMotorValue(90 - MOTOR_ANGLE + servoCalibrationData.rr.motor2 + m_robotState.flipStateRNow * (_calculateMotorAngle(m_legsValue.rr.hight + staticBallance.rr.hight, m_legsValue.rr.shift + staticBallance.rr.shift, 2)));
  doPWMServo();
}

// do servo pwm cycle 500, 2500
void doPWMServo(void) {
  servo_fl_1.write(servoMotorAngleValue[0]);
  servo_fl_2.write(servoMotorAngleValue[1]);
  servo_fr_1.write(servoMotorAngleValue[2]);
  servo_fr_2.write(servoMotorAngleValue[3]);
  servo_rl_1.write(servoMotorAngleValue[4]);
  servo_rl_2.write(servoMotorAngleValue[5]);
  servo_rr_1.write(servoMotorAngleValue[6]);
  servo_rr_2.write(servoMotorAngleValue[7]);
}

// update robot ballance
void updateBallanceServo(void) {
  staticBallance.fl.hight = m_legCorrect.fl.hight;
  staticBallance.fl.shift = FORWARD_BALLANCE_SHIFT - TRAPEZ_SHIFT + m_legCorrect.fl.shift;
  staticBallance.fr.hight = m_legCorrect.fr.hight;
  staticBallance.fr.shift = FORWARD_BALLANCE_SHIFT - TRAPEZ_SHIFT + m_legCorrect.fr.shift;
  staticBallance.rl.hight = m_legCorrect.rl.hight;
  staticBallance.rl.shift = FORWARD_BALLANCE_SHIFT + TRAPEZ_SHIFT + m_legCorrect.rl.shift;
  staticBallance.rr.hight = m_legCorrect.rr.hight;
  staticBallance.rr.shift = FORWARD_BALLANCE_SHIFT + TRAPEZ_SHIFT + m_legCorrect.rr.shift;
}
