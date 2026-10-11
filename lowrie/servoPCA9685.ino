/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

// structure for one leg motors
struct motors {
  int motor1;
  int motor2;
  int motor3;
  int motor4;
};
// legs motors structure
struct allCalMotors {
  motors fl;
  motors fr;
  motors rl;
  motors rr;
};

// motors calibration values for all motors
allCalMotors servoCalibrationData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// servo motor value
int servoMotorAngleValue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// servo motor center point
int servoMotorCenterPoint[16] = {60, 120, 60, 120, 120, 60, 120, 60, 60, 120, 60, 120, 120, 60, 120, 60};
// motors attached flag
bool attached = false;
// motors init values
int hightSetValueL = HIGHT_LOW;
int hightSetValueR = HIGHT_LOW;
int forwardSetValueL = 0;
int forwardSetValueR = 0;
// enable motors
bool frontLegsEnable = false;
bool rearLegsEnable = false;
bool extraLegsEnable = false;
// Create object to represent PCA9685 at default I2C address
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);

/*
uses
m_legsValue
m_getButtonPressed()
*/

// calculate motor 1 and motor 2 angles
int _calculateMotorAngle(int Hval, int Sval, int motorNum) {
  /*
  // for first motor mirror shift
  if (motorNum == 1) {
    Sval = -Sval;
  }
  // calculate Lvalue and angles
  Sval -= 13; // nalf distance between motors in mm
  float Lval = sqrt(Hval * Hval + Sval * Sval);
  float AngleC = (acos(Hval / Lval) * 180) / 3.14;
  if (Sval < 0) {
    AngleC = -AngleC;
  }
  float AngleB = (acos((Lval * Lval + 70.0 * 70.0 - 105.0 * 105.0) / (2 * Lval * 70.0)) * 180) / 3.14;
  //AngleB = (acos((Lval * Lval - 6125) / (Lval * 140)) * 180) / 3.14;
  return (int)(90 - AngleB - AngleC);
  */
  return 20;
}

// calculate motor 1 and motor 2 angles
int _calculateMotorAngleExtra(int Hval, int Sval, int motorNum) {
  /*
  // extra hight for rear motors
  Hval += 24;
  // for first motor mirror shift
  if (motorNum == 1) {
    Sval = -Sval;
  }
  // calculate Lvalue and angles
  Sval -= 13; // nalf distance between motors in mm
  float Lval = sqrt(Hval * Hval + Sval * Sval);
  float AngleC = (acos(Hval / Lval) * 180) / 3.14;
  if (Sval < 0) {
    AngleC = -AngleC;
  }
  float AngleB = (acos((Lval * Lval + 70.0 * 70.0 - 105.0 * 105.0) / (2 * Lval * 70.0)) * 180) / 3.14;
  //AngleB = (acos((Lval * Lval - 6125) / (Lval * 140)) * 180) / 3.14;
  return (int)(90 - AngleB - AngleC);
  */
  return 20;
}

// limit angle value
int _limitMotorValue(int mAngle) {
  if (mAngle > 180) {
    mAngle = 180;
  } else if (mAngle < 0) {
    mAngle = 0;
  }
  return mAngle;
}

// set motor angle
void _setMotorServo(int location, int motorAngle) {
  int angleValue = _limitMotorValue(motorAngle);
  long timevalue = map(angleValue, 0, 180, SERVOMIN, SERVOMAX);
  pca9685.setPWM(location, 0, (int)timevalue);
}

// do servo pwm cycle 500, 2500
void _doPWMServo(void) {
  if (attached) {
    if (frontLegsEnable) {
      _setMotorServo(0, (servoCalibrationData.fl.motor1 + servoMotorAngleValue[0] + servoMotorCenterPoint[0])); // fl 1
      _setMotorServo(1, (servoCalibrationData.fl.motor2 + servoMotorAngleValue[1] + servoMotorCenterPoint[1])); // fl 2
      if (extraLegsEnable) {
        _setMotorServo(2, (servoCalibrationData.fl.motor3 + servoMotorAngleValue[2] + servoMotorCenterPoint[2])); // fl 3
        _setMotorServo(3, (servoCalibrationData.fl.motor4 + servoMotorAngleValue[3] + servoMotorCenterPoint[3])); // fl 4
      }
      _setMotorServo(4, (servoCalibrationData.fr.motor1 + servoMotorAngleValue[4] + servoMotorCenterPoint[4])); // fr 1
      _setMotorServo(5, (servoCalibrationData.fr.motor2 + servoMotorAngleValue[5] + servoMotorCenterPoint[5])); // fr 2
      if (extraLegsEnable) {
        _setMotorServo(6, (servoCalibrationData.fr.motor3 + servoMotorAngleValue[6] + servoMotorCenterPoint[6])); // fr 3
        _setMotorServo(7, (servoCalibrationData.fr.motor4 + servoMotorAngleValue[7] + servoMotorCenterPoint[7])); // fr 4
      }
    }
    if (rearLegsEnable) {
      _setMotorServo(8, (servoCalibrationData.rl.motor1 + servoMotorAngleValue[8] + servoMotorCenterPoint[8])); // rl 1
      _setMotorServo(9, (servoCalibrationData.rl.motor2 + servoMotorAngleValue[9] + servoMotorCenterPoint[9])); // rl 2
      if (extraLegsEnable) {
        _setMotorServo(10, (servoCalibrationData.rl.motor3 + servoMotorAngleValue[10] + servoMotorCenterPoint[10])); // rl 3
        _setMotorServo(11, (servoCalibrationData.rl.motor4 + servoMotorAngleValue[11] + servoMotorCenterPoint[11])); // rl 4
      }
      _setMotorServo(12, (servoCalibrationData.rr.motor1 + servoMotorAngleValue[12] + servoMotorCenterPoint[12])); // rr 1
      _setMotorServo(13, (servoCalibrationData.rr.motor2 + servoMotorAngleValue[13] + servoMotorCenterPoint[13])); // rr 2
      if (extraLegsEnable) {
        _setMotorServo(14, (servoCalibrationData.rr.motor3 + servoMotorAngleValue[14] + servoMotorCenterPoint[14])); // rr 3
        _setMotorServo(15, (servoCalibrationData.rr.motor4 + servoMotorAngleValue[15] + servoMotorCenterPoint[15])); // rr 4
      }
    }
  }
}

// fast motor move
void _moveServosQuick(int hightL, int shiftL, int hightR, int shiftR) {
  servoMotorAngleValue[0] = _calculateMotorAngle(hightL, shiftL, 1);
  servoMotorAngleValue[1] = - _calculateMotorAngle(hightL, shiftL, 2);
  servoMotorAngleValue[2] = _calculateMotorAngleExtra(hightL, shiftL, 1);
  servoMotorAngleValue[3] = - _calculateMotorAngleExtra(hightL, shiftL, 2);
  servoMotorAngleValue[4] = - _calculateMotorAngle(hightR, shiftR, 1);
  servoMotorAngleValue[5] = _calculateMotorAngle(hightR, shiftR, 2);
  servoMotorAngleValue[6] = - _calculateMotorAngleExtra(hightR, shiftR, 1);
  servoMotorAngleValue[7] = _calculateMotorAngleExtra(hightR, shiftR, 2);
  servoMotorAngleValue[8] = _calculateMotorAngle(hightL, shiftL, 1);
  servoMotorAngleValue[9] = - _calculateMotorAngle(hightL, shiftL, 2);
  servoMotorAngleValue[10] = _calculateMotorAngleExtra(hightL, shiftL, 1);
  servoMotorAngleValue[11] = - _calculateMotorAngleExtra(hightL, shiftL, 2);
  servoMotorAngleValue[12] = - _calculateMotorAngle(hightR, shiftR, 1);
  servoMotorAngleValue[13] = _calculateMotorAngle(hightR, shiftR, 2);
  servoMotorAngleValue[14] = - _calculateMotorAngleExtra(hightR, shiftR, 1);
  servoMotorAngleValue[15] = _calculateMotorAngleExtra(hightR, shiftR, 2);
  _doPWMServo();
}

// init servo motors
void attachServo(bool frontE, bool rearE, bool extraE) {
  Serial.println(F(" attachServo "));
  frontLegsEnable = frontE;
  rearLegsEnable = rearE;
  extraLegsEnable = extraE;
  if (!attached) {
    // Initialize PCA9685
    pca9685.begin();
    // Set PWM Frequency to 50Hz
    pca9685.setPWMFreq(50);
    // init motors one by one
    if (frontLegsEnable) {
      _setMotorServo(0, servoMotorCenterPoint[0]); // fl 1
      delay(100);
      _setMotorServo(1, servoMotorCenterPoint[1]); // fl 2
      delay(100);
      if (extraLegsEnable) {
        _setMotorServo(2, servoMotorCenterPoint[2]); // fl 3
        delay(100);
        _setMotorServo(3, servoMotorCenterPoint[3]); // fl 4
        delay(100);
      }
      _setMotorServo(4, servoMotorCenterPoint[4]); // fr 1
      delay(100);
      _setMotorServo(5, servoMotorCenterPoint[5]); // fr 2
      delay(100);
      if (extraLegsEnable) {
        _setMotorServo(6, servoMotorCenterPoint[6]); // fr 3
        delay(100);
        _setMotorServo(7, servoMotorCenterPoint[7]); // fr 4
        delay(100);
      }
    }
    if (rearLegsEnable) {
      _setMotorServo(8, servoMotorCenterPoint[8]); // rl 1
      delay(100);
      _setMotorServo(9, servoMotorCenterPoint[9]); // rl 2
      delay(100);
      if (extraLegsEnable) {
        _setMotorServo(10, servoMotorCenterPoint[10]); // rl 3
        delay(100);
        _setMotorServo(11, servoMotorCenterPoint[11]); // rl 4
        delay(100);
      }
      _setMotorServo(12, servoMotorCenterPoint[12]); // rr 1
      delay(100);
      _setMotorServo(13, servoMotorCenterPoint[13]); // rr 2
      delay(100);
      if (extraLegsEnable) {
        _setMotorServo(14, servoMotorCenterPoint[14]); // rr 3
        delay(100);
        _setMotorServo(15, servoMotorCenterPoint[15]); // rr 4
        delay(100);
      }
    }
    attached = true;
  }
}

// init servo motors
void initServo(bool calibrationMode) {
  // check for calibration mode
  if (calibrationMode) {
    // do calibration
    int calibrationStage = 0;
    servoCalibrationData.fl.motor1 = 0;
    servoCalibrationData.fl.motor2 = 0;
    servoCalibrationData.fl.motor3 = 0;
    servoCalibrationData.fl.motor4 = 0;
    servoCalibrationData.fr.motor1 = 0;
    servoCalibrationData.fr.motor2 = 0;
    servoCalibrationData.fr.motor3 = 0;
    servoCalibrationData.fr.motor4 = 0;
    servoCalibrationData.rl.motor1 = 0;
    servoCalibrationData.rl.motor2 = 0;
    servoCalibrationData.rl.motor3 = 0;
    servoCalibrationData.rl.motor4 = 0;
    servoCalibrationData.rr.motor1 = 0;
    servoCalibrationData.rr.motor2 = 0;
    servoCalibrationData.rr.motor3 = 0;
    servoCalibrationData.rr.motor4 = 0;
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
    servoMotorAngleValue[12] = 0;
    servoMotorAngleValue[13] = 0;
    servoMotorAngleValue[14] = 0;
    servoMotorAngleValue[15] = 0;
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
            if (servoCalibrationData.fl.motor1 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.fl.motor1 = CALIBRATION_ANGLE_MIN;
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
            if (servoCalibrationData.fl.motor2 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.fl.motor2 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 3:
        {
          if (frontLegsEnable && extraLegsEnable) {
            servoCalibrationData.fl.motor3 ++;
            if (servoCalibrationData.fl.motor3 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.fl.motor3 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 4:
        {
          if (frontLegsEnable && extraLegsEnable) {
            servoCalibrationData.fl.motor4 ++;
            if (servoCalibrationData.fl.motor4 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.fl.motor4 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 5:
        {
          if (rearLegsEnable) {
            servoCalibrationData.rl.motor1 ++;
            if (servoCalibrationData.rl.motor1 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.rl.motor1 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 6:
        {
          if (rearLegsEnable) {
            servoCalibrationData.rl.motor2 ++;
            if (servoCalibrationData.rl.motor2 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.rl.motor2 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 7:
        {
          if (rearLegsEnable && extraLegsEnable) {
            servoCalibrationData.rl.motor3 ++;
            if (servoCalibrationData.rl.motor3 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.rl.motor3 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 8:
        {
          if (rearLegsEnable && extraLegsEnable) {
            servoCalibrationData.rl.motor4 ++;
            if (servoCalibrationData.rl.motor4 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.rl.motor4 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 9:
        {
          if (frontLegsEnable) {
            servoCalibrationData.fr.motor1 ++;
            if (servoCalibrationData.fr.motor1 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.fr.motor1 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 10:
        {
          if (frontLegsEnable) {
            servoCalibrationData.fr.motor2 ++;
            if (servoCalibrationData.fr.motor2 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.fr.motor2 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 11:
        {
          if (frontLegsEnable && extraLegsEnable) {
            servoCalibrationData.fr.motor3 ++;
            if (servoCalibrationData.fr.motor3 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.fr.motor3 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 12:
        {
          if (frontLegsEnable && extraLegsEnable) {
            servoCalibrationData.fr.motor4 ++;
            if (servoCalibrationData.fr.motor4 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.fr.motor4 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 13:
        {
          if (rearLegsEnable) {
            servoCalibrationData.rr.motor1 ++;
            if (servoCalibrationData.rr.motor1 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.rr.motor1 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 14:
        {
          if (rearLegsEnable) {
            servoCalibrationData.rr.motor2 ++;
            if (servoCalibrationData.rr.motor2 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.rr.motor2 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 15:
        {
          if (rearLegsEnable && extraLegsEnable) {
            servoCalibrationData.rr.motor3 ++;
            if (servoCalibrationData.rr.motor3 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.rr.motor3 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 16:
        {
          if (rearLegsEnable && extraLegsEnable) {
            servoCalibrationData.rr.motor4 ++;
            if (servoCalibrationData.rr.motor4 > CALIBRATION_ANGLE_MAX) {
              servoCalibrationData.rr.motor4 = CALIBRATION_ANGLE_MIN;
            }
          } else {
            calibrationStage ++;
          }
        }
        break;
        case 17:
        {
          EEPROM.write(1, (unsigned char)(servoCalibrationData.fl.motor1 + 128));
          EEPROM.write(2, (unsigned char)(servoCalibrationData.fl.motor2 + 128));
          EEPROM.write(3, (unsigned char)(servoCalibrationData.fl.motor3 + 128));
          EEPROM.write(4, (unsigned char)(servoCalibrationData.fl.motor4 + 128));
          EEPROM.write(5, (unsigned char)(servoCalibrationData.fr.motor1 + 128));
          EEPROM.write(6, (unsigned char)(servoCalibrationData.fr.motor2 + 128));
          EEPROM.write(7, (unsigned char)(servoCalibrationData.fr.motor3 + 128));
          EEPROM.write(8, (unsigned char)(servoCalibrationData.fr.motor4 + 128));
          EEPROM.write(9, (unsigned char)(servoCalibrationData.rl.motor1 + 128));
          EEPROM.write(10, (unsigned char)(servoCalibrationData.rl.motor2 + 128));
          EEPROM.write(11, (unsigned char)(servoCalibrationData.rl.motor3 + 128));
          EEPROM.write(12, (unsigned char)(servoCalibrationData.rl.motor4 + 128));
          EEPROM.write(13, (unsigned char)(servoCalibrationData.rr.motor1 + 128));
          EEPROM.write(14, (unsigned char)(servoCalibrationData.rr.motor2 + 128));
          EEPROM.write(15, (unsigned char)(servoCalibrationData.rr.motor3 + 128));
          EEPROM.write(16, (unsigned char)(servoCalibrationData.rr.motor4 + 128));
          EEPROM.commit();
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
  servoCalibrationData.fl.motor3 = (char)(EEPROM.read(3) - 128);
  servoCalibrationData.fl.motor4 = (char)(EEPROM.read(4) - 128);
  servoCalibrationData.fr.motor1 = (char)(EEPROM.read(5) - 128);
  servoCalibrationData.fr.motor2 = (char)(EEPROM.read(6) - 128);
  servoCalibrationData.fr.motor3 = (char)(EEPROM.read(7) - 128);
  servoCalibrationData.fr.motor4 = (char)(EEPROM.read(8) - 128);
  servoCalibrationData.rl.motor1 = (char)(EEPROM.read(9) - 128);
  servoCalibrationData.rl.motor2 = (char)(EEPROM.read(10) - 128);
  servoCalibrationData.rl.motor3 = (char)(EEPROM.read(11) - 128);
  servoCalibrationData.rl.motor4 = (char)(EEPROM.read(12) - 128);
  servoCalibrationData.rr.motor1 = (char)(EEPROM.read(13) - 128);
  servoCalibrationData.rr.motor2 = (char)(EEPROM.read(14) - 128);
  servoCalibrationData.rr.motor3 = (char)(EEPROM.read(15) - 128);
  servoCalibrationData.rr.motor4 = (char)(EEPROM.read(16) - 128);
  _doPWMServo();
  delay(100);
}

// detach hardware
void detachServo(void) {
  attached = false;
  _doPWMServo();
  delay(1000);
}

// linear motor move
void setServo(int hi1, int hi2, int timeDelay) {
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

// set servo motors
void setServoQuick(int hightL, int hightR, int timeDelay) {
  if (attached) {
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
  servoMotorAngleValue[0] = _calculateMotorAngle(m_legsValue.fl.hight, m_legsValue.fl.shift, 1);
  servoMotorAngleValue[1] = - _calculateMotorAngle(m_legsValue.fl.hight, m_legsValue.fl.shift, 2);
  servoMotorAngleValue[2] = _calculateMotorAngleExtra(m_legsValue.fl.hight, m_legsValue.fl.shift, 1);
  servoMotorAngleValue[3] = - _calculateMotorAngleExtra(m_legsValue.fl.hight, m_legsValue.fl.shift, 2);
  servoMotorAngleValue[4] = - _calculateMotorAngle(m_legsValue.fr.hight, m_legsValue.fr.shift, 1);
  servoMotorAngleValue[5] = _calculateMotorAngle(m_legsValue.fr.hight, m_legsValue.fr.shift, 2);
  servoMotorAngleValue[6] = - _calculateMotorAngleExtra(m_legsValue.fr.hight, m_legsValue.fr.shift, 1);
  servoMotorAngleValue[7] = _calculateMotorAngleExtra(m_legsValue.fr.hight, m_legsValue.fr.shift, 2);
  servoMotorAngleValue[8] = _calculateMotorAngle(m_legsValue.rl.hight, m_legsValue.rl.shift, 1);
  servoMotorAngleValue[9] = - _calculateMotorAngle(m_legsValue.rl.hight, m_legsValue.rl.shift, 2);
  servoMotorAngleValue[10] = _calculateMotorAngleExtra(m_legsValue.rl.hight, m_legsValue.rl.shift, 1);
  servoMotorAngleValue[11] = - _calculateMotorAngleExtra(m_legsValue.rl.hight, m_legsValue.rl.shift, 2);
  servoMotorAngleValue[12] = - _calculateMotorAngle(m_legsValue.rr.hight, m_legsValue.rr.shift, 1);
  servoMotorAngleValue[13] = _calculateMotorAngle(m_legsValue.rr.hight, m_legsValue.rr.shift, 2);
  servoMotorAngleValue[14] = - _calculateMotorAngleExtra(m_legsValue.rr.hight, m_legsValue.rr.shift, 1);
  servoMotorAngleValue[15] = _calculateMotorAngleExtra(m_legsValue.rr.hight, m_legsValue.rr.shift, 2);
  _doPWMServo();
}
