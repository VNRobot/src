/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot center motors motion patterns
*/

// pin numbers for servo motors
enum cPinsServo {
  CT1_MOTOR = 4,
  CT2_MOTOR = 5
};

// structure for center motors
struct center {
  char front;
  char rear;
};

// init servo library
Servo servo_ct_1;
Servo servo_ct_2;

// motors attached flag
bool centerAttached = false;
// motors values
char centerSetValueF = 0;
char centerSetValueR = 0;
// calibration data
center centerCalibrationData = {0, 0};
// servo motor value
short centerMotorAngleValue[2] = {0, 0};
// dynamic ballance
center dynamicCenterBallance = {0, 0};
// center motor value for turning/shifting positive - outside
center centerValue = {0, 0};
// ballance correction
center centerCorrect = {0, 0};
// direction
short directionValue = 0;
// side shift value
short sideShiftValue = 0;

/*
uses
m_legsValue
m_getButtonPressed()
*/

// limit angle value
short _limitCenterMotorValue(short mAngle) {
  if (mAngle > 130) {
    mAngle = 130;
  } else if (mAngle < 50) {
    mAngle = 50;
  }
  return mAngle;
}

// reduce shift value
void _reduceSideShiftvalue(void) {
  if (sideShiftValue > 0) {
    sideShiftValue --;
  } else if (sideShiftValue < 0) {
    sideShiftValue ++;
  }
}

// process direction
void _processDiretion(void) {
  short directionMax = directionValue;
  if (directionMax > 20) {
    directionMax = 20;
  } else if (directionMax < -20) {
    directionMax = -20;
  }
  if (directionMax > 0) {
    // turn left
    if (m_legsValue.fl.lifted) {
      centerSetValueF = directionMax;
    } else if (m_legsValue.fr.lifted) {
      centerSetValueF = -directionMax;
    }
    if (m_legsValue.rr.lifted) {
      centerSetValueR = directionMax;
    } else if (m_legsValue.rl.lifted) {
      centerSetValueR = -directionMax;
    }
  } else if (directionMax < 0) {
    // turn right
    if (m_legsValue.fr.lifted) {
      centerSetValueF = -directionMax;
    } else if (m_legsValue.fl.lifted) {
      centerSetValueF = directionMax;
    }
    if (m_legsValue.rr.lifted) {
      centerSetValueR = directionMax;
    } else if (m_legsValue.rl.lifted) {
      centerSetValueR = -directionMax;
    }
  } else {
    centerSetValueF = 0;
    centerSetValueR = 0;
  }
}

// process side shift
void _processSideShift(void) {
  short sideShiftMax = sideShiftValue;
  if (sideShiftMax > 20) {
    sideShiftMax = 20;
  } else if (sideShiftMax < -20) {
    sideShiftMax = -20;
  }
  if (sideShiftMax > 0) {
    // shift right
    if (m_legsValue.fl.lifted) {
      centerSetValueF = -sideShiftMax;
    } else if (m_legsValue.fr.lifted) {
      centerSetValueF = sideShiftMax;
    }
    if (m_legsValue.rr.lifted) {
      centerSetValueR = sideShiftMax;
    } else if (m_legsValue.rl.lifted) {
      centerSetValueR = -sideShiftMax;
    }
  } else if (sideShiftMax < 0) {
    // shift left
    if (m_legsValue.fr.lifted) {
      centerSetValueF = sideShiftMax;
    } else if (m_legsValue.fl.lifted) {
      centerSetValueF = -sideShiftMax;
    }
    if (m_legsValue.rr.lifted) {
      centerSetValueR = sideShiftMax;
    } else if (m_legsValue.rl.lifted) {
      centerSetValueR = -sideShiftMax;
    }
  } else {
    centerSetValueF = 0;
    centerSetValueR = 0;
  }
}

// do servo pwm cycle 500, 2500
void _doPWMCenter(void) {
  servo_ct_1.write(centerMotorAngleValue[0]);
  servo_ct_2.write(centerMotorAngleValue[1]);
}

// init servo motors
void attachCenter(void) {
  Serial.println(F("attachCenter"));
  if (!centerAttached) {
    // set motors value
    centerMotorAngleValue[0] = _limitCenterMotorValue(90); 
    centerMotorAngleValue[1] = _limitCenterMotorValue(90); 
    // init motors one by one
    servo_ct_1.attach(CT1_MOTOR, 500, 2500);
    servo_ct_1.write(centerMotorAngleValue[0]);
    delay(100);
    servo_ct_2.attach(CT2_MOTOR, 500, 2500);
    servo_ct_2.write(centerMotorAngleValue[1]);
    delay(100);
    centerAttached = true;
  }
}

// init servo motors
void initCenter(bool calibrationMode) {
  Serial.println(F("initCenter"));
  // check for calibration mode
  if (calibrationMode) {
    // do calibration
    unsigned char calibrationStage = 0;
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
          centerCalibrationData.front ++;
          if (centerCalibrationData.front > CALIBRATION_ANGLE_MAX) {
            centerCalibrationData.front = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 2:
        {
          centerCalibrationData.rear ++;
          if (centerCalibrationData.rear > CALIBRATION_ANGLE_MAX) {
            centerCalibrationData.rear = CALIBRATION_ANGLE_MIN;
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

// detach hardware
void detachCenter(void) {
  if (centerAttached) {
    servo_ct_1.detach();
    servo_ct_2.detach();
  }
  centerAttached = false;
  _doPWMCenter();
  delay(100);
}

// set servo motors
void setCenter(char angle) {
  centerSetValueF = 0;
  centerSetValueR = 0;
  // set motor angle
  centerMotorAngleValue[0] = _limitCenterMotorValue(90 - centerCalibrationData.front - angle);
  centerMotorAngleValue[1] = _limitCenterMotorValue(90 - centerCalibrationData.rear - angle);
  // move motors
  _doPWMCenter();
}

// set direction. 10 deg max
void setDirectionCenter(short direction) {
  directionValue = direction;
}

// set side shift // 0.42 servo reduction // ((HIGHT_DEFAULT / 2) + 18) * 2 * 3.14    l=502
// approx 1 mm shift for 1 deg
// set side shift. in mm
void setSideShiftCenter(short distance) {
  sideShiftValue = distance;
}
      
// move center motors.
void updateCenterCount(char speedLNow, char speedRNow) {
  if (sideShiftValue == 0) {
    // process direction
    _processDiretion();
  } else {
    // process side shift
    _processSideShift();
  }
  // apply shift correction
  if ((m_legsValue.fl.lifted) || (m_legsValue.fr.lifted)) {
    if (centerSetValueF > centerValue.front) {
      centerValue.front ++;
      // reduce shift value
      _reduceSideShiftvalue();
    } else if (centerSetValueF < centerValue.front) {
      centerValue.front --;
      // reduce shift value
      _reduceSideShiftvalue();
    }
  }
  if ((m_legsValue.rl.lifted) || (m_legsValue.rr.lifted)) {
    if (centerSetValueR > centerValue.rear) {
      centerValue.rear ++;
    } else if (centerSetValueR < centerValue.rear) {
      centerValue.rear --;
    }
  }
  // set motor angle
  centerMotorAngleValue[0] = _limitCenterMotorValue(90 - centerCalibrationData.front - centerValue.front * (6 - speedLNow - speedRNow) - dynamicCenterBallance.front);
  centerMotorAngleValue[1] = _limitCenterMotorValue(90 - centerCalibrationData.rear - centerValue.rear * (6 - speedLNow - speedRNow) - dynamicCenterBallance.rear);
  // move motors
  _doPWMCenter();
}

// update robot ballance
void updateBallanceCenter(void) {
  dynamicCenterBallance.front = centerCorrect.front;
  dynamicCenterBallance.rear = centerCorrect.rear;
}
