/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

// calibration angle
#define SERVO_CALIBRATION_ANGLE_MIN       -15
#define SERVO_CALIBRATION_ANGLE_MAX       15

// structure for servo
typedef struct servos {
  bool frontLegsEnable;
  bool rearLegsEnable;
  bool extraLegsEnable;
} servos;

// servo flags
servos servoFlag = {
  false,    // bool frontLegsEnable;
  false,    // bool rearLegsEnable;
  false     // bool extraLegsEnable;
};
// motors attached flag
bool servoAttached = false;

/*
uses
m_legsValue
m_getButtonPressed()
*/

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
    if (servoFlag.frontLegsEnable) {
      servo_fl_1.write(_limitMotorValue((short)(m_legsValue.fl.calibration1 + m_legsValue.fl.angle1 + m_legsValue.fl.center1)));
      servo_fl_2.write(_limitMotorValue((short)(m_legsValue.fl.calibration2 + m_legsValue.fl.angle2 + m_legsValue.fl.center2)));
      servo_fr_1.write(_limitMotorValue((short)(m_legsValue.fr.calibration1 + m_legsValue.fr.angle1 + m_legsValue.fr.center1)));
      servo_fr_2.write(_limitMotorValue((short)(m_legsValue.fr.calibration2 + m_legsValue.fr.angle2 + m_legsValue.fr.center2)));
    }
    if (servoFlag.rearLegsEnable) {
      servo_rl_1.write(_limitMotorValue((short)(m_legsValue.rl.calibration1 + m_legsValue.rl.angle1 + m_legsValue.rl.center1)));
      servo_rl_2.write(_limitMotorValue((short)(m_legsValue.rl.calibration2 + m_legsValue.rl.angle2 + m_legsValue.rl.center2)));
      servo_rr_1.write(_limitMotorValue((short)(m_legsValue.rr.calibration1 + m_legsValue.rr.angle1 + m_legsValue.rr.center1)));
      servo_rr_2.write(_limitMotorValue((short)(m_legsValue.rr.calibration2 + m_legsValue.rr.angle2 + m_legsValue.rr.center2)));
    }
    if (servoFlag.extraLegsEnable) {
      servo_el_1.write(_limitMotorValue((short)(m_legsValue.el.calibration1 + m_legsValue.el.angle1 + m_legsValue.el.center1)));
      servo_el_2.write(_limitMotorValue((short)(m_legsValue.el.calibration2 + m_legsValue.el.angle2 + m_legsValue.el.center2)));
      servo_er_1.write(_limitMotorValue((short)(m_legsValue.er.calibration1 + m_legsValue.er.angle1 + m_legsValue.er.center1)));
      servo_er_2.write(_limitMotorValue((short)(m_legsValue.er.calibration2 + m_legsValue.er.angle2 + m_legsValue.er.center2)));
    }
  }
}

// fast motor move
void _moveServosQuick(char mValue1, char mValue2) {
  m_legsValue.fl.angle1 = mValue1;
  m_legsValue.fl.angle2 = - mValue1;
  m_legsValue.fr.angle1 = - mValue2;
  m_legsValue.fr.angle2 = mValue2;
  m_legsValue.rl.angle1 = mValue1;
  m_legsValue.rl.angle2 = - mValue1;
  m_legsValue.rr.angle1 = - mValue2;
  m_legsValue.rr.angle2 = mValue2;
  m_legsValue.el.angle1 = mValue1;
  m_legsValue.el.angle2 = - mValue1;
  m_legsValue.er.angle1 = - mValue2;
  m_legsValue.er.angle2 = mValue2;
  _doPWMServo();
}

// attach servo motors
void attachServo(bool frontE, bool rearE, bool extraE) {
  servoFlag.frontLegsEnable = frontE;
  servoFlag.rearLegsEnable = rearE;
  servoFlag.extraLegsEnable = extraE;
  if (!servoAttached) {
    // init motors one by one
    if (servoFlag.frontLegsEnable) {
      servo_fl_1.attach(FL1_MOTOR, 500, 2500);
      servo_fl_1.write(m_legsValue.fl.center1);
      delay(100);
      servo_fr_1.attach(FR1_MOTOR, 500, 2500);
      servo_fr_1.write(m_legsValue.fr.center1);
      delay(100);
      servo_fl_2.attach(FL2_MOTOR, 500, 2500);
      servo_fl_2.write(m_legsValue.fl.center2);
      delay(100);
      servo_fr_2.attach(FR2_MOTOR, 500, 2500);
      servo_fr_2.write(m_legsValue.fr.center2);
      delay(100);
    }
    if (servoFlag.rearLegsEnable) {
      servo_rl_1.attach(RL1_MOTOR, 500, 2500);
      servo_rl_1.write(m_legsValue.rl.center1);
      delay(100);
      servo_rr_1.attach(RR1_MOTOR, 500, 2500);
      servo_rr_1.write(m_legsValue.rr.center1);
      delay(100);
      servo_rl_2.attach(RL2_MOTOR, 500, 2500);
      servo_rl_2.write(m_legsValue.rl.center2);
      delay(100);
      servo_rr_2.attach(RR2_MOTOR, 500, 2500);
      servo_rr_2.write(m_legsValue.rr.center2);
      delay(100);
    }
    if (servoFlag.extraLegsEnable) {
      servo_el_1.attach(EL1_MOTOR, 500, 2500);
      servo_el_1.write(m_legsValue.el.center1);
      delay(100);
      servo_el_2.attach(EL2_MOTOR, 500, 2500);
      servo_el_2.write(m_legsValue.el.center2);
      delay(100);
      servo_er_1.attach(ER1_MOTOR, 500, 2500);
      servo_er_1.write(m_legsValue.er.center1);
      delay(100);
      servo_er_2.attach(ER2_MOTOR, 500, 2500);
      servo_er_2.write(m_legsValue.er.center2);
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
      m_legsValue.fl.calibration1 = 0;
      m_legsValue.fl.calibration2 = 0;
      m_legsValue.fr.calibration1 = 0;
      m_legsValue.fr.calibration2 = 0;
      m_legsValue.rl.calibration1 = 0;
      m_legsValue.rl.calibration2 = 0;
      m_legsValue.rr.calibration1 = 0;
      m_legsValue.rr.calibration2 = 0;
      m_legsValue.el.calibration1 = 0;
      m_legsValue.el.calibration2 = 0;
      m_legsValue.er.calibration1 = 0;
      m_legsValue.er.calibration2 = 0;
      m_legsValue.fl.angle1 = 0;
      m_legsValue.fl.angle2 = 0;
      m_legsValue.fr.angle1 = 0;
      m_legsValue.fr.angle2 = 0;
      m_legsValue.rl.angle1 = 0;
      m_legsValue.rl.angle2 = 0;
      m_legsValue.rr.angle1 = 0;
      m_legsValue.rr.angle2 = 0;
      m_legsValue.el.angle1 = 0;
      m_legsValue.el.angle2 = 0;
      m_legsValue.er.angle1 = 0;
      m_legsValue.er.angle2 = 0;
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
            if (servoFlag.frontLegsEnable) {
              m_legsValue.fl.calibration1 ++;
              if (m_legsValue.fl.calibration1 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.fl.calibration1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 2:
          {
            if (servoFlag.frontLegsEnable) {
              m_legsValue.fl.calibration2 ++;
              if (m_legsValue.fl.calibration2 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.fl.calibration2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 5:
          {
            if (servoFlag.frontLegsEnable) {
              m_legsValue.fr.calibration1 ++;
              if (m_legsValue.fr.calibration1 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.fr.calibration1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 6:
          {
            if (servoFlag.frontLegsEnable) {
              m_legsValue.fr.calibration2 ++;
              if (m_legsValue.fr.calibration2 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.fr.calibration2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 3:
          {
            if (servoFlag.rearLegsEnable) {
              m_legsValue.rl.calibration1 ++;
              if (m_legsValue.rl.calibration1 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.rl.calibration1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 4:
          {
            if (servoFlag.rearLegsEnable) {
              m_legsValue.rl.calibration2 ++;
              if (m_legsValue.rl.calibration2 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.rl.calibration2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 7:
          {
            if (servoFlag.rearLegsEnable) {
             m_legsValue.rr.calibration1 ++;
              if (m_legsValue.rr.calibration1 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.rr.calibration1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 8:
          {
            if (servoFlag.rearLegsEnable) {
              m_legsValue.rr.calibration2 ++;
              if (m_legsValue.rr.calibration2 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.rr.calibration2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 9:
          {
            if (servoFlag.extraLegsEnable) {
              m_legsValue.el.calibration1 ++;
              if (m_legsValue.el.calibration1 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.el.calibration1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 10:
          {
            if (servoFlag.extraLegsEnable) {
              m_legsValue.el.calibration2 ++;
              if (m_legsValue.el.calibration2 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.el.calibration2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 11:
          {
            if (servoFlag.extraLegsEnable) {
              m_legsValue.er.calibration1 ++;
              if (m_legsValue.er.calibration1 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.er.calibration1 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 12:
          {
            if (servoFlag.extraLegsEnable) {
              m_legsValue.er.calibration2 ++;
              if (m_legsValue.er.calibration2 > SERVO_CALIBRATION_ANGLE_MAX) {
                m_legsValue.er.calibration2 = SERVO_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 13:
          {
            EEPROM.update(1, (unsigned char)(m_legsValue.fl.calibration1 + 128));
            EEPROM.update(2, (unsigned char)(m_legsValue.fl.calibration2 + 128));
            EEPROM.update(3, (unsigned char)(m_legsValue.fr.calibration1 + 128));
            EEPROM.update(4, (unsigned char)(m_legsValue.fr.calibration2 + 128));
            EEPROM.update(5, (unsigned char)(m_legsValue.rl.calibration1 + 128));
            EEPROM.update(6, (unsigned char)(m_legsValue.rl.calibration2 + 128));
            EEPROM.update(7, (unsigned char)(m_legsValue.rr.calibration1 + 128));
            EEPROM.update(8, (unsigned char)(m_legsValue.rr.calibration2 + 128));
            EEPROM.update(9, (unsigned char)(m_legsValue.el.calibration1 + 128));
            EEPROM.update(10, (unsigned char)(m_legsValue.el.calibration2 + 128));
            EEPROM.update(11, (unsigned char)(m_legsValue.er.calibration1 + 128));
            EEPROM.update(12, (unsigned char)(m_legsValue.er.calibration2 + 128));
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
    m_legsValue.fl.calibration1 = (char)(EEPROM.read(1) - 128);
    m_legsValue.fl.calibration2 = (char)(EEPROM.read(2) - 128);
    m_legsValue.fr.calibration1 = (char)(EEPROM.read(3) - 128);
    m_legsValue.fr.calibration2 = (char)(EEPROM.read(4) - 128);
    m_legsValue.rl.calibration1 = (char)(EEPROM.read(5) - 128);
    m_legsValue.rl.calibration2 = (char)(EEPROM.read(6) - 128);
    m_legsValue.rr.calibration1 = (char)(EEPROM.read(7) - 128);
    m_legsValue.rr.calibration2 = (char)(EEPROM.read(8) - 128);
    m_legsValue.el.calibration1 = (char)(EEPROM.read(9) - 128);
    m_legsValue.el.calibration2 = (char)(EEPROM.read(10) - 128);
    m_legsValue.er.calibration1 = (char)(EEPROM.read(11) - 128);
    m_legsValue.er.calibration2 = (char)(EEPROM.read(12) - 128);
    _doPWMServo();
    delay(100);
  }
}

// detach hardware
void detachServo(void) {
  if (servoAttached) {
    if (servoFlag.frontLegsEnable) {
      servo_fl_1.detach();
      servo_fr_1.detach();
      servo_fl_2.detach();
      servo_fr_2.detach();
    }
    if (servoFlag.rearLegsEnable) {
      servo_rl_1.detach();
      servo_rr_1.detach();
      servo_rl_2.detach();
      servo_rr_2.detach();
    }
    if (servoFlag.extraLegsEnable) {
      servo_el_1.detach();
      servo_el_2.detach();
      servo_er_1.detach();
      servo_er_2.detach();
    }
  }
  servoAttached = false;
  delay(1000);
}

// linear motor move
void setServo(char hi1, char hi2, short timeDelay) {
  if (servoAttached) {
    bool doLoop = true;
    // motors init values
    char hightSetValueL = m_legsValue.fl.angle1;
    char hightSetValueR = - m_legsValue.fr.angle1;
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
      if ((hightSetValueL == hi1) && (hightSetValueR == hi2)) {
        doLoop = false;
      }
      _moveServosQuick(hightSetValueL, hightSetValueR);
      delay(timeDelay);
    }
  }
}

// set servo motors
void setServoQuick(char angleL, char angleR, short timeDelay) {
  if (servoAttached) {
    _moveServosQuick(angleL, angleR);
    delay(timeDelay);
  }
}

// move leg motors.
void updateLegsServoCount(void) {
  if (servoAttached) {
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
