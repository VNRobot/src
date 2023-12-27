/*
Wlking Robot Lowrie
Licended GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Process analog inputs
left turned sensor A0
right turned sensor A1
battery sensor A6
current sensors:
center A7
front A3
rear A2
*/
// sensors structure
struct sensors {
  unsigned short left;
  unsigned short right;
  unsigned short center;
  unsigned short front;
  unsigned short rear;
  unsigned short battery;
};
// analog input values left sensor, right sensor
sensors _analogInputs = {0, 0, 0, 0, 0, 0};
// battery low measurement counter
unsigned char _batteryLow = 0;

// check battery low state
bool checkBatteryLowInputs(void) {
  // battery low level is 6.2v - 750
  if ((_analogInputs.battery > 500) && (_analogInputs.battery < 750)) {
    _batteryLow ++;
  } else {
    _batteryLow = 0;
  }
  if (_batteryLow > 2) {
    return true;
  }
  return false;
}

// check for calibration mode
bool checkForDemoModeInputs(void) {
    // sensors are blocked 500 ~ 5cm
    if (_analogInputs.left > 400 || _analogInputs.right > 400) {
        return true;
    }
    return false;
}

// update analog proximity sensors readingd
void updateInputs(unsigned char sequenceCount) {
  if (sequenceCount == 0) {
    // read proximity sensors
    _analogInputs.left = (unsigned short)analogRead(A1);
    _analogInputs.right = (unsigned short)analogRead(A0);
    _analogInputs.center = (unsigned short)analogRead(A7);
    _analogInputs.front = (unsigned short)analogRead(A3);
    _analogInputs.rear = (unsigned short)analogRead(A2);
    _analogInputs.battery = (unsigned short)analogRead(A6);
  } else {
    // read proximity sensors to find average value
    _analogInputs.left = (_analogInputs.left * 3 + (unsigned short)analogRead(A1)) / 4;
    _analogInputs.right = (_analogInputs.right * 3 + (unsigned short)analogRead(A0)) / 4;
    _analogInputs.center += (unsigned short)analogRead(A7);
    _analogInputs.front += (unsigned short)analogRead(A3);
    _analogInputs.rear += (unsigned short)analogRead(A2);
    _analogInputs.battery += (unsigned short)analogRead(A6);
  }
  if (sequenceCount == 19) {
    // find average value
    _analogInputs.center /= 20;
    _analogInputs.front /= 20;
    _analogInputs.rear /= 20;
    _analogInputs.battery /= 20;
  }
}

// process distances
unsigned char getDistanceLevel(unsigned short input)
{
  if (input > 80) {
    if (input > 140) {
      if (input > 210) {
        if (input > 500) {
          // blocked
          return 3;
        } else {
          // 10 - 30 cm
          return 2;
        }
      } else {
        // 30 - 50 cm
        return 1;
      }
    } else {
      // far - 50 cm
      return 0;
    }
  } else {
    // out of range
    return 0;
  }
}

// process sensors return next task name
// could be more complex if remembers previos states
unsigned char getTaskByInputs(bool vertical, bool batteryLow, bool highCurrent) {
  if (batteryLow) {
    return DOWNTASK;
  }
  if (highCurrent) {
    return STANDTASK;
  }
  if (!vertical) {
    // stop moving
    return STANDTASK;
  }
  // check sensors enabled
  if (! m_sensorsEnabled) {
    return m_defaultTask;
  }
  // calculate obstacle code
  switch ((unsigned char)(getDistanceLevel(_analogInputs.right) * 4 + getDistanceLevel(_analogInputs.left))) {
    //         left rignt
    case 0: //    0    0
      return m_defaultTask;
    break;
    case 1: //    1    0
      return WALKSHIFTRIGHTTASK;
    break;
    case 2: //    2    0
      return WALKTURNRIGHTTASK;
    break;
    case 3: //    3    0
      return STANDTURNRIGHTTASK;
    break;
    case 4: //    0    1
      return WALKSHIFTLEFTTASK;
    break;
    case 5: //    1    1
    {
      if (_analogInputs.right > _analogInputs.left) {
        return WALKTURNLEFTTASK;
      } else {
        return WALKTURNRIGHTTASK;
      }
    }
    break;
    case 6: //    2    1
      return STANDTURNRIGHTTASK;
    break;
    case 7: //    3    1
      return STANDTURNRIGHTTASK;
    break;
    case 8: //    0    2
      return WALKTURNLEFTTASK;
    break;
    case 9: //    1    2
      return STANDTURNLEFTTASK;
    break;
    case 10: //   2     2
      return TURNAROUNDTASK;
    break;
    case 11: //    3    2
      return STANDTURNRIGHT2TASK;
    break;
    case 12: //    0    3
      return STANDTURNLEFTTASK;
    break;
    case 13: //    1    3
      return STANDTURNLEFTTASK;
    break;
    case 14: //    2    3
      return STANDTURNLEFT2TASK;
    break;
    case 15: //    3    3
      return WALKBACKTASK;
    break;
    default:
      return m_defaultTask;
    break;
  }
  return m_defaultTask;
}

// get current of center motors
unsigned short getCenterCurrentInputs(void) {
  if ((_analogInputs.battery < _analogInputs.center) || (! m_currentEnabled)) {
    return 0;
  }
  return _analogInputs.battery - _analogInputs.center;
}

// get current of front motors
unsigned short getFrontCurrentInputs(void) {
  if ((_analogInputs.battery < _analogInputs.front) || (! m_currentEnabled)) {
    return 0;
  }
  return _analogInputs.battery - _analogInputs.front;
}

// get current of rear motors
unsigned short getRearCurrentInputs(void) {
  if ((_analogInputs.battery < _analogInputs.rear) || (! m_currentEnabled)) {
    return 0;
  }
  return _analogInputs.battery - _analogInputs.rear;
}

// fix balance using current sensors
bool checkHighCurrentInputs(void) {
  // max current is 150
  if (getCenterCurrentInputs() > 150) {
    return true;
  }
  else if (getFrontCurrentInputs() > 150) {
    return true;
  }
  else if (getRearCurrentInputs() > 150) {
    return true;
  }
  return false;
}
