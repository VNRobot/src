/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nanos
Robot center motors motion patterns
*/

// center motor limit
#define CENTER_ANGLE_MAX              10
// direction sensitivity
#define CENTER_TURNING_SENSITIVITY    2
// turning mutiplier
#define CENTER_TURNING_MULIPLIER      6
// calibration angle
#define CENTER_CALIBRATION_ANGLE_MIN   -15
#define CENTER_CALIBRATION_ANGLE_MAX   15


// structure for center motors
struct center {
  char front;
  char rear;
};

// motors attached flag
bool centerAttached = false;
// motors set value values
centers centerSetValue = {0, 0};
// calibration data
center centerCalibrationData = {0, 0};
// servo motor value
short centerMotorAngleValue[2] = {0, 0};
// center motor value for turning/shifting positive - outside
center centerValue = {0, 0};
// direction to turn front
short directionFront = 0;
// direction to turn rear
short directionRear = 0;
// enable center motors
bool frontCenterEnabled = false;
bool rearCenterEnabled = false;

/*
uses
m_legsValue
m_getButtonPressed()
*/

// limit angle value
short _limitCenterMotorValue(short mAngle) {
  if (mAngle > 150) {
    mAngle = 150;
  } else if (mAngle < 30) {
    mAngle = 30;
  }
  return mAngle;
}

// do servo pwm cycle 500, 2500
void _doPWMCenter(void) {
  if (frontCenterEnabled) {
    servo_ex_3.write(centerMotorAngleValue[0]);
  }
  if (rearCenterEnabled) {
    servo_ex_4.write(centerMotorAngleValue[1]);
  }
}

// attach center motors
void attachCenter(bool frontE, bool rearE) {
  frontCenterEnabled = frontE;
  rearCenterEnabled = rearE;
  if (!centerAttached) {
    // set motors value
    centerMotorAngleValue[0] = _limitCenterMotorValue(90); 
    centerMotorAngleValue[1] = _limitCenterMotorValue(90); 
    // init motors one by one
    if (frontCenterEnabled) {
      servo_ex_3.attach(EX3_MOTOR, 500, 2500);
      servo_ex_3.write(centerMotorAngleValue[0]);
      delay(100);
    }
    if (rearCenterEnabled) {
      servo_ex_4.attach(EX4_MOTOR, 500, 2500);
      servo_ex_4.write(centerMotorAngleValue[1]);
      delay(100);
    }
    centerAttached = true;
  }
}

// init servo motors
void initCenter(bool calibrationMode) {
  if (centerAttached) {
    // check for calibration mode
    if (calibrationMode) {
      // do calibration
      char calibrationStage = 0;
      centerCalibrationData.front = 0;
      centerCalibrationData.rear = 0;
      // motors one by one
      while (calibrationMode) {
        centerMotorAngleValue[0] = _limitMotorValue(90 - centerCalibrationData.front);
        centerMotorAngleValue[1] = _limitMotorValue(90 - centerCalibrationData.rear);
        _doPWMCenter();
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
            if (frontCenterEnabled) {
              centerCalibrationData.front ++;
              if (centerCalibrationData.front > CENTER_CALIBRATION_ANGLE_MAX) {
                centerCalibrationData.front = CENTER_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 2:
          {
            if (rearCenterEnabled) {
              centerCalibrationData.rear ++;
              if (centerCalibrationData.rear > CENTER_CALIBRATION_ANGLE_MAX) {
                centerCalibrationData.rear = CENTER_CALIBRATION_ANGLE_MIN;
              }
            } else {
              calibrationStage ++;
            }
          }
          break;
          case 3:
          {
            EEPROM.update(9, (unsigned char)(centerCalibrationData.front + 128));
            EEPROM.update(10, (unsigned char)(centerCalibrationData.rear + 128));
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
    centerCalibrationData.front = (char)(EEPROM.read(9) - 128);
    centerCalibrationData.rear = (char)(EEPROM.read(10) - 128);
    centerMotorAngleValue[0] = _limitMotorValue(90 - centerCalibrationData.front);
    centerMotorAngleValue[1] = _limitMotorValue(90 - centerCalibrationData.rear);
    _doPWMCenter();
    delay(100);
  }
}

// detach hardware
void detachCenter(void) {
  if (centerAttached) {
    if (frontCenterEnabled) {
      servo_ex_3.detach();
    }
    if (rearCenterEnabled) {
      servo_ex_4.detach();
    }
    centerAttached = false;
    _doPWMCenter();
    delay(100);
  }
}

// set servo motors
void setCenter(char angle) {
  if (centerAttached) {
    centerSetValue.front = angle;
    centerSetValue.rear = angle;
    // real angle
    m_centerAngle.front = (centerSetValue.front * 10) / 24;
    m_centerAngle.rear = (centerSetValue.rear * 10) / 24;
    // set motor angle
    centerMotorAngleValue[0] = _limitCenterMotorValue(90 - centerCalibrationData.front - m_centerAngle.front);
    centerMotorAngleValue[1] = _limitCenterMotorValue(90 - centerCalibrationData.rear - m_centerAngle.rear);
    // move motors
    _doPWMCenter();
  }
}

// set direction. 20 deg max
void setDirectionCenter(char direction) {
  if (centerAttached) {
    if (direction > CENTER_ANGLE_MAX) {
      direction = CENTER_ANGLE_MAX;
    }
    if (direction < -CENTER_ANGLE_MAX) {
      direction = -CENTER_ANGLE_MAX;
    }
    directionFront = -(short)direction;
    directionRear = (short)direction;
  }
}

// approx 1 mm shift for 1 deg
// set side shift. in mm
void setSideShiftCenter(short direction) {
  if (centerAttached) {
    if (direction > CENTER_ANGLE_MAX) {
      direction = CENTER_ANGLE_MAX;
    }
    if (direction < -CENTER_ANGLE_MAX) {
      direction = -CENTER_ANGLE_MAX;
    }
    directionFront = -direction;
    directionRear = -direction;
  }
}
      
// move center motors.
void updateCenterCount(void) {
  if (centerAttached) {
    if (directionFront > CENTER_TURNING_SENSITIVITY) {
      // turn front right
      if ((m_legsValue.fr.state == LEG_LIFTED_BEFORE) || (m_legsValue.fr.state == LEG_LIFTED_AFTER)) {
        // move leg to the side
        if (centerValue.front < directionFront) {
          centerValue.front ++;
        }
      } else if ((m_legsValue.fl.state == LEG_LIFTED_BEFORE) || (m_legsValue.fl.state == LEG_LIFTED_AFTER)) {
        // move leg back to normal
        if (centerValue.front > 0) {
          centerValue.front --;
        }
      }
    } else if (directionFront < -CENTER_TURNING_SENSITIVITY) {
      // turn front left
      if ((m_legsValue.fl.state == LEG_LIFTED_BEFORE) || (m_legsValue.fl.state == LEG_LIFTED_AFTER)) {
        // move leg to the side
        if (centerValue.front < -directionFront) {
          centerValue.front ++;
        }
      } else if ((m_legsValue.fr.state == LEG_LIFTED_BEFORE) || (m_legsValue.fr.state == LEG_LIFTED_AFTER)) {
        // move leg back to normal
        if (centerValue.front > 0) {
          centerValue.front --;
        }
      }
    } else if (directionFront > 0) {
      if ((m_legsValue.fl.state == LEG_LIFTED_BEFORE) || (m_legsValue.fl.state == LEG_LIFTED_AFTER)) {
        // move leg back to normal
        if (centerValue.front > 0) {
          centerValue.front --;
        }
      }
    } else {
      if ((m_legsValue.fr.state == LEG_LIFTED_BEFORE) || (m_legsValue.fr.state == LEG_LIFTED_AFTER)) {
        // move leg back to normal
        if (centerValue.front > 0) {
          centerValue.front --;
        }
      }
    }
    if (directionRear > CENTER_TURNING_SENSITIVITY) {
      // turn rear right
      if ((m_legsValue.rr.state == LEG_LIFTED_BEFORE) || (m_legsValue.rr.state == LEG_LIFTED_AFTER)) {
        // move leg to the side
        if (centerValue.rear < directionRear) {
          centerValue.rear ++;
        }
      } else if ((m_legsValue.rl.state == LEG_LIFTED_BEFORE) || (m_legsValue.rl.state == LEG_LIFTED_AFTER)) {
        // move leg back to normal
        if (centerValue.rear > 0) {
          centerValue.rear --;
        }
      }
    } else if (directionRear < -CENTER_TURNING_SENSITIVITY) {
      // turn rear left
      if ((m_legsValue.rl.state == LEG_LIFTED_BEFORE) || (m_legsValue.rl.state == LEG_LIFTED_AFTER)) {
        // move leg to the side
        if (centerValue.rear < -directionRear) {
          centerValue.rear ++;
        }
      } else if ((m_legsValue.rr.state == LEG_LIFTED_BEFORE) || (m_legsValue.rr.state == LEG_LIFTED_AFTER)) {
        // move leg back to normal
        if (centerValue.rear > 0) {
          centerValue.rear --;
        }
      }
    } else if (directionRear > 0) {
      if ((m_legsValue.rl.state == LEG_LIFTED_BEFORE) || (m_legsValue.rl.state == LEG_LIFTED_AFTER)) {
        // move leg back to normal
        if (centerValue.rear > 0) {
          centerValue.rear --;
        }
      }
    } else {
      if ((m_legsValue.rr.state == LEG_LIFTED_BEFORE) || (m_legsValue.rr.state == LEG_LIFTED_AFTER)) {
        // move leg back to normal
        if (centerValue.rear > 0) {
          centerValue.rear --;
        }
      }
    }
    // real angle
    m_centerAngle.front = ((centerSetValue.front + centerValue.front * CENTER_TURNING_MULIPLIER) * 10) / 24;
    m_centerAngle.rear = ((centerSetValue.rear + centerValue.rear * CENTER_TURNING_MULIPLIER) * 10) / 24;
    // set motor angle
    centerMotorAngleValue[0] = _limitCenterMotorValue(90 - centerCalibrationData.front - m_centerAngle.front);
    centerMotorAngleValue[1] = _limitCenterMotorValue(90 - centerCalibrationData.rear - m_centerAngle.rear);
    // move motors
    _doPWMCenter();
  }
}
