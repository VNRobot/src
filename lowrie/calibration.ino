/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
calibration file
*/

// do legs calibration
unsigned char legsCalibration(unsigned char patternCounter) {
  unsigned short current = 0;
  if (_deviceMode == CALIBRATION_FRONT) {
    if (_calibrationStage == 0) {
      if (_calibrationCounter == 0) {
        // set initial leg calibration
        m_calibration.front = 0;
        m_calibration.rear = 0;
        m_calibration.m.fl.motor1 = -20;
        m_calibration.m.fl.motor2 = -20;
        m_calibration.m.fr.motor1 = -20;
        m_calibration.m.fr.motor2 = -20;
        m_calibration.m.rl.motor1 = -20;
        m_calibration.m.rl.motor2 = -20;
        m_calibration.m.rr.motor1 = -20;
        m_calibration.m.rr.motor2 = -20;
        _calibrationCounter ++;
      } else {
        // read current or button
        current = getCenterCurrentInputs();
        if (_getButtonPressed()  || (current > 39)) {
          if (current > 39) {
            m_calibration.front -= 25;
          }
          _calibrationCounter = 0;
          _calibrationStage = 0;
          _deviceMode = CALIBRATION_REAR;
        } else {
          m_calibration.front ++;
          if (m_calibration.front > 40) {
            m_calibration.front = -20;
          }
        }
      }
    }
  } else if (_deviceMode == CALIBRATION_REAR) {
    if (_calibrationStage == 0) {
      if (_calibrationCounter == 0) {
        // set initial leg calibration
        m_calibration.rear = 0;
        _calibrationCounter ++;
      } else {
        // read current or button
        current = getCenterCurrentInputs();
        if (_getButtonPressed()  || (current > 39)) {
          if (current > 39) {
            m_calibration.rear -= 25;
          }
          _calibrationCounter = 0;
          _calibrationStage = 0;
          _deviceMode = CALIBRATION_AUTO_1;
        } else {
          m_calibration.rear ++;
          if (m_calibration.rear > 40) {
            m_calibration.rear = -20;
          }
        }
      }
    }
  } else if (_deviceMode == CALIBRATION_AUTO_1) {
    // tune motor 1
    switch (_calibrationStage) {
      case 0:
      {
        // leg FL
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.fl.motor1 = -5;
          patternCounter ++;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getFrontCurrentInputs();
          if (_getButtonPressed()  || (current > 39)) {
            if (current > 39) {
              m_calibration.m.fl.motor1 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            m_calibration.m.fl.motor1 ++;
            if (m_calibration.m.fl.motor1 > 30) {
              m_calibration.m.fl.motor1 = -20;
            }
          }
        }
      }
      break;
      case 2:
      {
        // leg FR
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.fr.motor1 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getFrontCurrentInputs();
          if (_getButtonPressed()  || (current > 39)) {
            if (current > 39) {
              m_calibration.m.fr.motor1 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            m_calibration.m.fr.motor1 ++;
            if (m_calibration.m.fr.motor1 > 30) {
              m_calibration.m.fr.motor1 = -20;
            }
          }
        }
      }
      break;
      case 1:
      {
        // leg RL
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.rl.motor1 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getRearCurrentInputs();
          if (_getButtonPressed()  || (current > 39)) {
            if (current > 39) {
              m_calibration.m.rl.motor1 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            m_calibration.m.rl.motor1 ++;
            if (m_calibration.m.rl.motor1 > 30) {
              m_calibration.m.rl.motor1 = -20;
            }
          }
        }
      }
      break;
      case 3:
      {
        // leg RR
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.rr.motor1 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getRearCurrentInputs();
          if (_getButtonPressed()  || (current > 39)) {
            if (current > 39) {
              m_calibration.m.rr.motor1 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage = 0;
            _deviceMode = CALIBRATION_AUTO_2;
          } else {
            m_calibration.m.rr.motor1 ++;
            if (m_calibration.m.rr.motor1 > 30) {
              m_calibration.m.rr.motor1 = -20;
            }
          }
        }
      }
      break;
      default:
      break;
    }
  } else if (_deviceMode == CALIBRATION_AUTO_2) {
    // tune motor 2
    switch (_calibrationStage) {
      case 2:
      {
        // leg FL
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.fl.motor2 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getFrontCurrentInputs();
          if (_getButtonPressed()  || (current > 39)) {
            if (current > 39) {
              m_calibration.m.fl.motor2 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            m_calibration.m.fl.motor2 ++;
            if (m_calibration.m.fl.motor2 > 30) {
              m_calibration.m.fl.motor2 = -20;
            }
          }
        }
      }
      break;
      case 0:
      {
        // leg FR
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.fr.motor2 = -5;
          patternCounter ++;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getFrontCurrentInputs();
          if (_getButtonPressed()  || (current > 39)) {
            if (current > 39) {
              m_calibration.m.fr.motor2 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            m_calibration.m.fr.motor2 ++;
            if (m_calibration.m.fr.motor2 > 30) {
              m_calibration.m.fr.motor2 = -20;
            }
          }
        }
      }
      break;
      case 3:
      {
        // leg RL
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.rl.motor2 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getRearCurrentInputs();
          if (_getButtonPressed()  || (current > 39)) {
            if (current > 39) {
              m_calibration.m.rl.motor2 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage = 0;
            // end of calibration
            patternCounter ++;
            _deviceMode = CALIBRATION_SAVE;
          } else {
            m_calibration.m.rl.motor2 ++;
            if (m_calibration.m.rl.motor2 > 30) {
              m_calibration.m.rl.motor2 = -20;
            }
          }
        }
      }
      break;
      case 1:
      {
        // leg RR
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.rr.motor2 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getRearCurrentInputs();
          if (_getButtonPressed()  || (current > 39)) {
            if (current > 39) {
              m_calibration.m.rr.motor2 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            m_calibration.m.rr.motor2 ++;
            if (m_calibration.m.rr.motor2 > 30) {
              m_calibration.m.rr.motor2 = -20;
            }
          }
        }
      }
      break;
      default:
      break;
    }
  }
  return patternCounter;
}

// detect button pressed
bool _getButtonPressed(void) {
  if (m_modePressed) {
    m_modePressed = false;
    return true;
  }
  return false;
}
