/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot center motors motion patterns
*/

// pin numbers for servo motors
enum sPinsServo {
  ST1_MOTOR = 4,
  ST2_MOTOR = 5
};

// init servo library
Servo m_servo_st_1;
Servo m_servo_st_2;

// motors attached flag
bool centerAttached = false;
// motors values
char centerSetValueF = 0;
char centerSetValueR = 0;
// dynamic ballance
motors dynamicCenterBallance = {0, 0};
// direction
short directionValue = 0;
// side shift value
short sideShiftValue = 0;

// limit angle value
short _limitCenterMotorValue(short mAngle) {
  if (mAngle > 130) {
    mAngle = 130;
  } else if (mAngle < 50) {
    mAngle = 50;
  }
  return mAngle;
}

// init servo motors
void initCenter(void) {
  // set motors value
  m_centerMotorAngleValue[0] = _limitCenterMotorValue(90 - m_centerCalibrationData.motor1);
  m_centerMotorAngleValue[1] = _limitCenterMotorValue(90 - m_centerCalibrationData.motor2);
  // init motors one by one
  m_servo_st_1.attach(ST1_MOTOR, 500, 2500);
  doPWMCenter();
  delay(100);
  m_servo_st_2.attach(ST2_MOTOR, 500, 2500);
  doPWMCenter();
  delay(100);
  centerAttached = true;
}

// detach hardware
void detachCenter(void) {
  if (centerAttached) {
    m_servo_st_1.detach();
    m_servo_st_2.detach();
  }
  centerAttached = false;
  doPWMCenter();
  delay(100);
}

// do servo pwm cycle 500, 2500
void doPWMCenter(void) {
  m_servo_st_1.write(m_centerMotorAngleValue[0]);
  m_servo_st_2.write(m_centerMotorAngleValue[1]);
}

// set servo motors
void setCenter(char angle) {
  centerSetValueF = angle;
  centerSetValueR = angle;
  updateCenter();
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

// move leg motors.
void updateCenter(void) {
  if (sideShiftValue == 0) {
    // process direction
    _processDiretion();
  } else {
    // process side shift
    _processSideShift();
  }
  // apply shift correction
  if ((m_legsValue.fl.lifted) || (m_legsValue.fr.lifted)) {
    if (centerSetValueF > m_centerValue.motor1) {
      m_centerValue.motor1 ++;
      // reduce shift value
      _reduceSideShiftvalue();
    } else if (centerSetValueF < m_centerValue.motor1) {
      m_centerValue.motor1 --;
      // reduce shift value
      _reduceSideShiftvalue();
    }
  }
  if ((m_legsValue.rl.lifted) || (m_legsValue.rr.lifted)) {
    if (centerSetValueR > m_centerValue.motor2) {
      m_centerValue.motor2 ++;
    } else if (centerSetValueR < m_centerValue.motor2) {
      m_centerValue.motor2 --;
    }
  }
  // set motor angle
  m_centerMotorAngleValue[0] = _limitCenterMotorValue(90 - m_centerCalibrationData.motor1 - m_centerValue.motor1 * 2 - dynamicCenterBallance.motor1);
  m_centerMotorAngleValue[1] = _limitCenterMotorValue(90 - m_centerCalibrationData.motor2 - m_centerValue.motor2 * 2 - dynamicCenterBallance.motor2);
  // move motors
  doPWMCenter();
}

// update robot ballance
void updateBallanceCenter(void) {
  dynamicCenterBallance.motor1 = m_centerCorrect.motor1;
  dynamicCenterBallance.motor2 = m_centerCorrect.motor2;
}
