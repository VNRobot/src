/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot center motors motion patterns
*/

// center motor limit
#define CENTER_ANGLE_MAX     10
// direction sensitivity
#define TURNING_SENSITIVITY 2
// turning mutiplier
#define TURNING_MULIPLIER    4
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
// motors set value values
centers centerSetValue = {0, 0};
// calibration data
center centerCalibrationData = {0, 0};
// servo motor value
short centerMotorAngleValue[2] = {0, 0};
// dynamic ballance
//center dynamicCenterBallance = {0, 0};
// center motor value for turning/shifting positive - outside
center centerValue = {0, 0};
// ballance correction
//center centerCorrect = {0, 0};
// direction to turn front
short directionFront = 0;
// direction to turn rear
short directionRear = 0;
// real leg side angle
centers realAngle = {0, 0};

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
  centerSetValue.front = angle;
  centerSetValue.rear = angle;
  // set motor angle
  centerMotorAngleValue[0] = _limitCenterMotorValue(90 - centerCalibrationData.front - ((centerSetValue.front * 10) / 24));
  centerMotorAngleValue[1] = _limitCenterMotorValue(90 - centerCalibrationData.rear - ((centerSetValue.rear * 10) / 24));
  // move motors
  _doPWMCenter();
}

// set direction. 10 deg max
void setDirectionCenter(short direction) {
  if (direction > CENTER_ANGLE_MAX) {
    direction = CENTER_ANGLE_MAX;
  }
  if (direction < -CENTER_ANGLE_MAX) {
    direction = -CENTER_ANGLE_MAX;
  }
  directionFront = -direction;
  directionRear = direction;
}

// set side shift // 0.42 servo reduction // ((HIGHT_DEFAULT / 2) + 18) * 2 * 3.14    l=502
// approx 1 mm shift for 1 deg
// set side shift. in mm
void setSideShiftCenter(short direction) {
  if (direction > CENTER_ANGLE_MAX) {
    direction = CENTER_ANGLE_MAX;
  }
  if (direction < -CENTER_ANGLE_MAX) {
    direction = -CENTER_ANGLE_MAX;
  }
  directionFront = -direction;
  directionRear = -direction;
}
      
// move center motors.
void updateCenterCount(void) {
  if (directionFront > TURNING_SENSITIVITY) {
    // turn front right
    if (m_legsValue.fr.state == LEG_LIFTED) {
      // move leg to the side
      if (centerValue.front < directionFront) {
        centerValue.front ++;
      }
    } else if (m_legsValue.fl.state == LEG_LIFTED) {
      // move leg back to normal
      if (centerValue.front > 0) {
        centerValue.front --;
      }
    }
  } else if (directionFront < -TURNING_SENSITIVITY) {
    // turn front left
    if (m_legsValue.fl.state == LEG_LIFTED) {
      // move leg to the side
      if (centerValue.front < -directionFront) {
        centerValue.front ++;
      }
    } else if (m_legsValue.fr.state == LEG_LIFTED) {
      // move leg back to normal
      if (centerValue.front > 0) {
        centerValue.front --;
      }
    }
  } else if (directionFront > 0) {
    if (m_legsValue.fl.state == LEG_LIFTED) {
      // move leg back to normal
      if (centerValue.front > 0) {
        centerValue.front --;
      }
    }
  } else {
    if (m_legsValue.fr.state == LEG_LIFTED) {
      // move leg back to normal
      if (centerValue.front > 0) {
        centerValue.front --;
      }
    }
  }
  if (directionRear > TURNING_SENSITIVITY) {
    // turn rear right
    if (m_legsValue.rr.state == LEG_LIFTED) {
      // move leg to the side
      if (centerValue.rear < directionRear) {
        centerValue.rear ++;
      }
    } else if (m_legsValue.rl.state == LEG_LIFTED) {
      // move leg back to normal
      if (centerValue.rear > 0) {
        centerValue.rear --;
      }
    }
  } else if (directionRear < -TURNING_SENSITIVITY) {
    // turn rear left
    if (m_legsValue.rl.state == LEG_LIFTED) {
      // move leg to the side
      if (centerValue.rear < -directionRear) {
        centerValue.rear ++;
      }
    } else if (m_legsValue.rr.state == LEG_LIFTED) {
      // move leg back to normal
      if (centerValue.rear > 0) {
        centerValue.rear --;
      }
    }
  } else if (directionRear > 0) {
    if (m_legsValue.rl.state == LEG_LIFTED) {
      // move leg back to normal
      if (centerValue.rear > 0) {
        centerValue.rear --;
      }
    }
  } else {
    if (m_legsValue.rr.state == LEG_LIFTED) {
      // move leg back to normal
      if (centerValue.rear > 0) {
        centerValue.rear --;
      }
    }
  }
  // real angle
  realAngle.front = ((centerSetValue.front + centerValue.front * TURNING_MULIPLIER) * 10) / 24;
  realAngle.rear = ((centerSetValue.rear + centerValue.rear * TURNING_MULIPLIER) * 10) / 24;
  // set motor angle
  centerMotorAngleValue[0] = _limitCenterMotorValue(90 - centerCalibrationData.front - realAngle.front);
  centerMotorAngleValue[1] = _limitCenterMotorValue(90 - centerCalibrationData.rear - realAngle.rear);
  // move motors
  _doPWMCenter();
}

// get leg angle compensation in mm
short getCenterCompensationFront(void) {
  float hight = HIGHT_DEFAULT + LEG_EXTRA_HIGHT;
  float defaultAngle = (asin(LEG_EXTRA_SIDE / hight) * 180.0) / 3.14;
  return (short)(HIGHT_DEFAULT - (cos(((realAngle.front + defaultAngle) * 3.14) / 180.0)) * HIGHT_DEFAULT);
}

// get leg angle compensation in mm
short getCenterCompensationRear(void) {
  float hight = HIGHT_DEFAULT + LEG_EXTRA_HIGHT;
  float defaultAngle = (asin(LEG_EXTRA_SIDE / hight) * 180.0) / 3.14;
  return (short)(HIGHT_DEFAULT - (cos(((realAngle.rear + defaultAngle) * 3.14) / 180.0)) * HIGHT_DEFAULT);
}

// get leg angle compensation in mm
centers getCenterCompensation(void) {
  centers compensation = {0, 0};
  float hight = HIGHT_DEFAULT + LEG_EXTRA_HIGHT;
  float defaultAngle = (asin(LEG_EXTRA_SIDE / hight) * 180.0) / 3.14;
  compensation.front = (short)(HIGHT_DEFAULT - (cos(((realAngle.front + defaultAngle) * 3.14) / 180.0)) * HIGHT_DEFAULT);
  compensation.rear = (short)(HIGHT_DEFAULT - (cos(((realAngle.rear + defaultAngle) * 3.14) / 180.0)) * HIGHT_DEFAULT);
  return compensation;
}

/*
// update robot ballance
void updateBallanceCenter(void) {
  centerSetValue.front += ;
  centerSetValue.rear -= ;
}
*/