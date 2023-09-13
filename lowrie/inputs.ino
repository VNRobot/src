/*
Wlking Robot Lowrie
Licended GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Process analog inputs
left turned sensor A0
right turned sensor A1
*/
// sensors structure
struct sensors {
  unsigned short left;
  unsigned short right;
};
// analog input values left sensor, right sensor
sensors _analogInputs = {0, 0};
// battery low measurement counter
unsigned char _batteryLow = 0;

// check battery low state
bool checkBatteryLowInputs(void) {
  if (digitalRead(BATTERY_LOW) == HIGH) {
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
    if (_analogInputs.left > 500 && _analogInputs.right > 500) {
        return true;
    }
    return false;
}

// update analog proximity sensors readingd
void updateInputs(unsigned char sequenceCount) {
  if (sequenceCount == 0) {
    // read proximity sensors
    _analogInputs.left = (unsigned short)analogRead(A0);
    _analogInputs.right = (unsigned short)analogRead(A1);
  } else {
    // read proximity sensors to find average value
    _analogInputs.left = (_analogInputs.left * 3 + (unsigned short)analogRead(A0)) / 4;
    _analogInputs.right = (_analogInputs.right * 3 + (unsigned short)analogRead(A1)) / 4;
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
unsigned char getTaskByInputs(bool vertical, bool batteryLow) {
  if (batteryLow) {
    return DOWNTASK;
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
