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
#define CENTER_TURNING_MULIPLIER      2


// structure for center motors
struct center {
  char front;
  char rear;
};

// motors attached flag
bool centerAttached = false;
// motors enabled flag
bool centerEnabled = false;
// center motor value for turning/shifting positive - outside
center centerValue = {0, 0};
// direction to turn front
short directionFront = 0;
// direction to turn rear
short directionRear = 0;

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

// attach center motors
void attachCenter(bool cEnabled) {
  centerEnabled = cEnabled;
  centerAttached = true;
}

// init servo motors
void initCenter(void) {
}


// set servo motors
void setCenter(char angle) {
  if (centerAttached) {
    m_legsValue.er.center1 = 90 + angle;
    m_legsValue.er.center2 = 90 + angle;
  }
}

// set direction. 20 deg max
void setDirectionCenter(char direction) {
  if (centerAttached) {
    if (centerEnabled) {
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
}

// approx 1 mm shift for 1 deg
// set side shift. in mm
void setSideShiftCenter(short direction) {
  if (centerAttached) {
    if (centerEnabled) {
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
}
      
// move center motors.
void updateCenterCount(void) {
  if (centerAttached) {
    bool liftedFL = false;
    bool liftedFR = false;
    bool liftedRL = false;
    bool liftedRR = false;
    bool turnRight = false;
    bool turnLeft = false;
    if (m_legsValue.fl.state == LEG_LIFTED) {
      liftedFL = true;
    }
    if (m_legsValue.fr.state == LEG_LIFTED) {
      liftedFR = true;
    }
    if (m_legsValue.rl.state == LEG_LIFTED) {
      liftedRL = true;
    }
    if (m_legsValue.rr.state == LEG_LIFTED) {
      liftedRR = true;
    }
    if (directionFront > CENTER_TURNING_SENSITIVITY) {
      turnRight = true;
    }
    if (directionRear < -CENTER_TURNING_SENSITIVITY) {
      turnLeft = true;
    }
    if (turnRight) {
      // turn front right
      if (liftedFR) {
        // move leg to the side
        if (centerValue.front < directionFront) {
          centerValue.front ++;
        }
      } else if (liftedFL) {
        // move leg back to normal
        if (centerValue.front > 0) {
          centerValue.front --;
        }
      }
    } else if (turnLeft) {
      // turn front left
      if (liftedFL) {
        // move leg to the side
        if (centerValue.front < -directionFront) {
          centerValue.front ++;
        }
      } else if (liftedFR) {
        // move leg back to normal
        if (centerValue.front > 0) {
          centerValue.front --;
        }
      }
    } else if (directionFront > 0) {
      if (liftedFL) {
        // move leg back to normal
        if (centerValue.front > 0) {
          centerValue.front --;
        }
      }
    } else {
      if (liftedFR) {
        // move leg back to normal
        if (centerValue.front > 0) {
          centerValue.front --;
        }
      }
    }
    if (turnRight) {
      // turn rear right
      if (liftedRR) {
        // move leg to the side
        if (centerValue.rear < directionRear) {
          centerValue.rear ++;
        }
      } else if (liftedRL) {
        // move leg back to normal
        if (centerValue.rear > 0) {
          centerValue.rear --;
        }
      }
    } else if (turnLeft) {
      // turn rear left
      if (liftedRL) {
        // move leg to the side
        if (centerValue.rear < -directionRear) {
          centerValue.rear ++;
        }
      } else if (liftedRR) {
        // move leg back to normal
        if (centerValue.rear > 0) {
          centerValue.rear --;
        }
      }
    } else if (directionRear > 0) {
      if (liftedRL) {
        // move leg back to normal
        if (centerValue.rear > 0) {
          centerValue.rear --;
        }
      }
    } else {
      if (liftedRR) {
        // move leg back to normal
        if (centerValue.rear > 0) {
          centerValue.rear --;
        }
      }
    }
    // real angle
    m_legsValue.er.angle1 = - centerValue.front * CENTER_TURNING_MULIPLIER;
    m_legsValue.er.angle2 = - centerValue.rear * CENTER_TURNING_MULIPLIER;
    // 
    // can set leg hight compensation for all legs
  }
}
