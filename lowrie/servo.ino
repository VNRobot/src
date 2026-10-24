/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

// calibration angle
#define SERVO_CALIBRATION_ANGLE_MIN   -15
#define SERVO_CALIBRATION_ANGLE_MAX   15

// structure for one leg motors
struct motors {
  char motor1;
  char motor2;
};
// legs motors structure
struct allCalMotors {
  motors fl;
  motors fr;
  motors rl;
  motors rr;
  motors el;
  motors er;
};

// motors calibration values for all motors
allCalMotors servoCalibrationData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// servo motor value
short servoMotorAngleValue[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// servo motor center point
short servoMotorCenterPoint[12] = {60, 120, 120, 60, 60, 120, 120, 60, 60, 120, 60, 120};
// motors attached flag
bool servoAttached = false;
// motors init values
short hightSetValueL = 80;
short hightSetValueR = 80;
char forwardSetValueL = 0;
char forwardSetValueR = 0;
// side flipped flags
char flippedL = 1;
char flippedR = 1;
// enable motors
bool frontLegsEnable = false;
bool rearLegsEnable = false;
bool extraLegsEnable = false;
// ride mode
bool rideEnabled = false;

/*
uses
m_legsValue
m_getButtonPressed()
*/

// calculate motor 1 and motor 2 angles
short _calculateMotorAngle(int Hval, int Sval, char motorNum) {
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
  //AngleB = (acos((Lval * Lval + 70 * 70 - 105 * 105) / (2 * Lval * 70)) * 180) / 3.14;    6125
  AngleB = (acos((Lval * Lval - 6125) / (Lval * 140)) * 180) / 3.14;
  return (short)(90 - AngleB - AngleC);
}

// calculate motor 1 and motor 2 angles
short _calculateMotorAngleExtra(int Hval, int Sval, char motorNum) {
  float Lval = (float)Hval;
  float AngleB = 0;
  float AngleC = 0;
  // for first motor mirror shift
  if (motorNum == 1) {
    Sval = -Sval;
  }
  // calculate Lvalue and angles
  Sval += 6; // nalf distance between motors in mm
  Lval = sqrt(Hval * Hval + Sval * Sval);
  AngleC = (acos(Hval / Lval) * 180) / 3.14;
  if (Sval < 0) {
    AngleC = -AngleC;
  }
  //AngleB = (acos((Lval * Lval + 70 * 70 - 105 * 105) / (2 * Lval * 70)) * 180) / 3.14;    6125
  AngleB = (acos((Lval * Lval - 6125) / (Lval * 140)) * 180) / 3.14;
  return (short)(90 - AngleB - AngleC);
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

// do servo pwm cycle 500, 2500
void _doPWMServo(void) {
  if (servoAttached) {
    if (frontLegsEnable) {
      servo_fl_1.write(_limitMotorValue(servoCalibrationData.fl.motor1 + servoMotorAngleValue[0] + servoMotorCenterPoint[0])); // fl 1
      servo_fl_2.write(_limitMotorValue(servoCalibrationData.fl.motor2 + servoMotorAngleValue[1] + servoMotorCenterPoint[1])); // fl 2
      servo_fr_1.write(_limitMotorValue(servoCalibrationData.fr.motor1 + servoMotorAngleValue[2] + servoMotorCenterPoint[2])); // fr 1
      servo_fr_2.write(_limitMotorValue(servoCalibrationData.fr.motor2 + servoMotorAngleValue[3] + servoMotorCenterPoint[3])); // fr 2
    }
    if (rearLegsEnable) {
      servo_rl_1.write(_limitMotorValue(servoCalibrationData.rl.motor1 + servoMotorAngleValue[4] + servoMotorCenterPoint[4])); // rl 1
      servo_rl_2.write(_limitMotorValue(servoCalibrationData.rl.motor2 + servoMotorAngleValue[5] + servoMotorCenterPoint[5])); // rl 2
      servo_rr_1.write(_limitMotorValue(servoCalibrationData.rr.motor1 + servoMotorAngleValue[6] + servoMotorCenterPoint[6])); // rr 1
      servo_rr_2.write(_limitMotorValue(servoCalibrationData.rr.motor2 + servoMotorAngleValue[7] + servoMotorCenterPoint[7])); // rr 2
    }
    if (extraLegsEnable) {
      servo_ex_1.write(_limitMotorValue(servoCalibrationData.el.motor1 + servoMotorAngleValue[8] + servoMotorCenterPoint[8]));
      servo_ex_2.write(_limitMotorValue(servoCalibrationData.el.motor2 + servoMotorAngleValue[9] + servoMotorCenterPoint[9]));
      servo_ex_3.write(_limitMotorValue(servoCalibrationData.er.motor1 + servoMotorAngleValue[10] + servoMotorCenterPoint[10]));
      servo_ex_4.write(_limitMotorValue(servoCalibrationData.er.motor2 + servoMotorAngleValue[11] + servoMotorCenterPoint[11]));
    }
  }
}

// fast motor move
void _moveServosQuick(short hightL, short shiftL, short hightR, short shiftR) {
  short mValue1 = _calculateMotorAngle((int)hightL, (int)(shiftL), 1);
  short mValue2 = _calculateMotorAngle((int)hightL, (int)(shiftL), 2);
  servoMotorAngleValue[0] = flippedL * mValue1;
  servoMotorAngleValue[1] = - flippedL * mValue2;
  servoMotorAngleValue[4] = flippedL * mValue1;
  servoMotorAngleValue[5] = - flippedL * mValue2;
  mValue1 = _calculateMotorAngleExtra((int)hightL, (int)(shiftL), 1);
  mValue2 = _calculateMotorAngleExtra((int)hightL, (int)(shiftL), 2);
  servoMotorAngleValue[8] = flippedL * mValue1;
  servoMotorAngleValue[10] = flippedL * mValue1;
  mValue1 = _calculateMotorAngle((int)hightR, (int)(shiftR), 1);
  mValue2 = _calculateMotorAngle((int)hightR, (int)(shiftR), 2);
  servoMotorAngleValue[2] = - flippedR * mValue1;
  servoMotorAngleValue[3] = flippedR * mValue2;
  servoMotorAngleValue[6] = - flippedR * mValue1;
  servoMotorAngleValue[7] = flippedR * mValue2;
  mValue1 = _calculateMotorAngleExtra((int)hightR, (int)(shiftR), 1);
  mValue2 = _calculateMotorAngleExtra((int)hightR, (int)(shiftR), 2);
  servoMotorAngleValue[9] = - flippedR * mValue1;
  servoMotorAngleValue[11] = - flippedR * mValue1;
  _doPWMServo();
}

// init servo motors
void attachServo(bool frontE, bool rearE, bool extraE) {
  frontLegsEnable = frontE;
  rearLegsEnable = rearE;
  extraLegsEnable = extraE;
  if (!servoAttached) {
    // init motors one by one
    if (frontLegsEnable) {
      servo_fl_1.attach(FL1_MOTOR, 500, 2500);
      servo_fl_1.write(servoMotorCenterPoint[0]);
      delay(100);
      servo_fr_1.attach(FR1_MOTOR, 500, 2500);
      servo_fr_1.write(servoMotorCenterPoint[2]);
      delay(100);
      servo_fl_2.attach(FL2_MOTOR, 500, 2500);
      servo_fl_2.write(servoMotorCenterPoint[1]);
      delay(100);
      servo_fr_2.attach(FR2_MOTOR, 500, 2500);
      servo_fr_2.write(servoMotorCenterPoint[3]);
      delay(100);
    }
    if (rearLegsEnable) {
      servo_rl_1.attach(RL1_MOTOR, 500, 2500);
      servo_rl_1.write(servoMotorCenterPoint[4]);
      delay(100);
      servo_rr_1.attach(RR1_MOTOR, 500, 2500);
      servo_rr_1.write(servoMotorCenterPoint[6]);
      delay(100);
      servo_rl_2.attach(RL2_MOTOR, 500, 2500);
      servo_rl_2.write(servoMotorCenterPoint[5]);
      delay(100);
      servo_rr_2.attach(RR2_MOTOR, 500, 2500);
      servo_rr_2.write(servoMotorCenterPoint[7]);
      delay(100);
    }
    if (extraLegsEnable) {
      servo_ex_1.attach(EX1_MOTOR, 500, 2500);
      servo_ex_1.write(servoMotorCenterPoint[0]);
      delay(100);
      servo_ex_2.attach(EX2_MOTOR, 500, 2500);
      servo_ex_2.write(servoMotorCenterPoint[2]);
      delay(100);
      servo_ex_3.attach(EX3_MOTOR, 500, 2500);
      servo_ex_3.write(servoMotorCenterPoint[1]);
      delay(100);
      servo_ex_4.attach(EX4_MOTOR, 500, 2500);
      servo_ex_4.write(servoMotorCenterPoint[3]);
      delay(100);
    }
    servoAttached = true;
  }
}

// init servo motors
void initServo(bool calibrationMode) {
  if (servoAttached) {
    // check for calibration mode
    if (calibrationMode) {
      // do calibration
      char calibrationStage = 0;
      servoCalibrationData.fl.motor1 = 0;
      servoCalibrationData.fl.motor2 = 0;
      servoCalibrationData.fr.motor1 = 0;
      servoCalibrationData.fr.motor2 = 0;
      servoCalibrationData.rl.motor1 = 0;
      servoCalibrationData.rl.motor2 = 0;
      servoCalibrationData.rr.motor1 = 0;
      servoCalibrationData.rr.motor2 = 0;
      servoCalibrationData.el.motor1 = 0;
      servoCalibrationData.el.motor2 = 0;
      servoCalibrationData.er.motor1 = 0;
      servoCalibrationData.er.motor2 = 0;
      servoMotorAngleValue[0] = 0;
      servoMotorAngleValue[1] = 0;
      servoMotorAngleValue[2] = 0;
      servoMotorAngleValue[3] = 0;
      servoMotorAngleValue[4] = 0;
      servoMotorAngleValue[5] = 0;
      servoMotorAngleValue[6] = 0;
      servoMotorAngleValue[7] = 0;
      servoMotorAngleValue[8] = 0;
      servoMotorAngleValue[9] = 0;
      servoMotorAngleValue[10] = 0;
      servoMotorAngleValue[11] = 0;
      // motors one by one
      while (calibrationMode) {
        _doPWMServo();
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
            if (frontLegsEnable) {
              servoCalibrationData.fl.motor1 ++;
              if (servoCalibrationData.fl.motor1 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.fl.motor1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 2:
          {
            if (frontLegsEnable) {
              servoCalibrationData.fl.motor2 ++;
              if (servoCalibrationData.fl.motor2 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.fl.motor2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 5:
          {
            if (frontLegsEnable) {
              servoCalibrationData.fr.motor1 ++;
              if (servoCalibrationData.fr.motor1 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.fr.motor1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 6:
          {
            if (frontLegsEnable) {
              servoCalibrationData.fr.motor2 ++;
              if (servoCalibrationData.fr.motor2 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.fr.motor2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 3:
          {
            if (rearLegsEnable) {
              servoCalibrationData.rl.motor1 ++;
              if (servoCalibrationData.rl.motor1 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.rl.motor1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 4:
          {
            if (rearLegsEnable) {
              servoCalibrationData.rl.motor2 ++;
              if (servoCalibrationData.rl.motor2 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.rl.motor2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 7:
          {
            if (rearLegsEnable) {
              servoCalibrationData.rr.motor1 ++;
              if (servoCalibrationData.rr.motor1 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.rr.motor1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 8:
          {
            if (rearLegsEnable) {
              servoCalibrationData.rr.motor2 ++;
              if (servoCalibrationData.rr.motor2 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.rr.motor2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 9:
          {
            if (extraLegsEnable) {
              servoCalibrationData.el.motor1 ++;
              if (servoCalibrationData.el.motor1 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.el.motor1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 10:
          {
            if (extraLegsEnable) {
              servoCalibrationData.el.motor2 ++;
              if (servoCalibrationData.el.motor2 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.el.motor2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 11:
          {
            if (extraLegsEnable) {
              servoCalibrationData.er.motor1 ++;
              if (servoCalibrationData.er.motor1 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.er.motor1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 12:
          {
            if (extraLegsEnable) {
              servoCalibrationData.er.motor2 ++;
              if (servoCalibrationData.er.motor2 > SERVO_CALIBRATION_ANGLE_MAX) {
                servoCalibrationData.er.motor2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 13:
          {
            EEPROM.update(1, (unsigned char)(servoCalibrationData.fl.motor1 + 128));
            EEPROM.update(2, (unsigned char)(servoCalibrationData.fl.motor2 + 128));
            EEPROM.update(3, (unsigned char)(servoCalibrationData.fr.motor1 + 128));
            EEPROM.update(4, (unsigned char)(servoCalibrationData.fr.motor2 + 128));
            EEPROM.update(5, (unsigned char)(servoCalibrationData.rl.motor1 + 128));
            EEPROM.update(6, (unsigned char)(servoCalibrationData.rl.motor2 + 128));
            EEPROM.update(7, (unsigned char)(servoCalibrationData.rr.motor1 + 128));
            EEPROM.update(8, (unsigned char)(servoCalibrationData.rr.motor2 + 128));
            EEPROM.update(9, (unsigned char)(servoCalibrationData.el.motor1 + 128));
            EEPROM.update(10, (unsigned char)(servoCalibrationData.el.motor2 + 128));
            EEPROM.update(11, (unsigned char)(servoCalibrationData.er.motor1 + 128));
            EEPROM.update(12, (unsigned char)(servoCalibrationData.er.motor2 + 128));
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
    servoCalibrationData.el.motor1 = (char)(EEPROM.read(9) - 128);
    servoCalibrationData.el.motor2 = (char)(EEPROM.read(10) - 128);
    servoCalibrationData.er.motor1 = (char)(EEPROM.read(11) - 128);
    servoCalibrationData.er.motor2 = (char)(EEPROM.read(12) - 128);
    _doPWMServo();
    delay(100);
  }
}

// detach hardware
void detachServo(void) {
  if (servoAttached) {
    if (frontLegsEnable) {
      servo_fl_1.detach();
      servo_fr_1.detach();
      servo_fl_2.detach();
      servo_fr_2.detach();
    }
    if (rearLegsEnable) {
      servo_rl_1.detach();
      servo_rr_1.detach();
      servo_rl_2.detach();
      servo_rr_2.detach();
    }
    if (extraLegsEnable) {
      servo_ex_1.detach();
      servo_ex_2.detach();
      servo_ex_3.detach();
      servo_ex_4.detach();
    }
  }
  servoAttached = false;
  delay(1000);
}

// linear motor move
void setServo(short hi1, short hi2, short timeDelay) {
  if (servoAttached) {
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
      _moveServosQuick(hightSetValueL, forwardSetValueL, hightSetValueR, forwardSetValueR);
      delay(timeDelay);
    }
  }
}

// set servo motors
void setServoQuick(short hightL, short hightR, short timeDelay) {
  if (servoAttached) {
    // set motors values fast
    forwardSetValueL = 0;
    hightSetValueL = hightL;
    forwardSetValueR = 0;
    hightSetValueR = hightR;
    _moveServosQuick(hightSetValueL, forwardSetValueL, hightSetValueR, forwardSetValueR);
    delay(timeDelay);
  }
}

// move leg motors.
void updateLegsServoCount(void) {
  if (servoAttached) {
    if (!rideEnabled) {
      servoMotorAngleValue[0] = flippedL * (_calculateMotorAngle(m_legsValue.fl.hight, m_legsValue.fl.shift, 1));
      servoMotorAngleValue[1] = - flippedL * (_calculateMotorAngle(m_legsValue.fl.hight, m_legsValue.fl.shift, 2));
      servoMotorAngleValue[2] = - flippedR * (_calculateMotorAngle(m_legsValue.fr.hight, m_legsValue.fr.shift, 1));
      servoMotorAngleValue[3] = flippedR * (_calculateMotorAngle(m_legsValue.fr.hight, m_legsValue.fr.shift, 2));
      servoMotorAngleValue[4] = flippedL * (_calculateMotorAngle(m_legsValue.rl.hight, m_legsValue.rl.shift, 1));
      servoMotorAngleValue[5] = - flippedL * (_calculateMotorAngle(m_legsValue.rl.hight, m_legsValue.rl.shift, 2));
      servoMotorAngleValue[6] = - flippedR * (_calculateMotorAngle(m_legsValue.rr.hight, m_legsValue.rr.shift, 1));
      servoMotorAngleValue[7] = flippedR * (_calculateMotorAngle(m_legsValue.rr.hight, m_legsValue.rr.shift, 2));
      servoMotorAngleValue[8] = flippedL * (_calculateMotorAngleExtra(m_legsValue.fl.hight, m_legsValue.fl.shift, 1));
      servoMotorAngleValue[9] = - flippedR * (_calculateMotorAngleExtra(m_legsValue.fr.hight, m_legsValue.fr.shift, 1));
      servoMotorAngleValue[10] = flippedL * (_calculateMotorAngleExtra(m_legsValue.rl.hight, m_legsValue.rl.shift, 1));
      servoMotorAngleValue[11] = - flippedR * (_calculateMotorAngleExtra(m_legsValue.rr.hight, m_legsValue.rr.shift, 1));
    }
    _doPWMServo();
  }
}

// set flipped state
void setFlippedServo(char flipL, char flipR) {
  if (servoAttached) {
    flippedL = flipL;
    flippedR = flipR;
  }
}

// enable ride
void rideModeServo(bool doRide) {
  if (servoAttached) {
    if (doRide) {
      if (!rideEnabled) {
        rideEnabled = true;
      }
    } else {
      rideEnabled = false;
    }
  }
}
