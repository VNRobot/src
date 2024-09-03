/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
motors calibration
*/

// calibration counter
unsigned char _calibrationCounter = 0;
// calibration stage
unsigned char _calibrationStage = 0;
// mode button press flag
bool _modePressed = false;
// device mode
unsigned char _deviceMode = EXPLORE;

// set device mode
void setModeCalibration(unsigned char mode) {
  _deviceMode = mode;
}

// get device mode
unsigned char getModeCalibration(void) {
  return _deviceMode;
}

// do legs calibration
unsigned char legsCalibration(unsigned char patternCounter, allMotors * calibrationData) {
  unsigned short current = 0;
  if (_deviceMode == CALIBRATION_FRONT) {
    if (_calibrationStage == 0) {
      if (_calibrationCounter == 0) {
        // set initial leg calibration
        calibrationData->front = 0;
        calibrationData->rear = 0;
        calibrationData->m.fl.motor1 = -20;
        calibrationData->m.fl.motor2 = -20;
        calibrationData->m.fr.motor1 = -20;
        calibrationData->m.fr.motor2 = -20;
        calibrationData->m.rl.motor1 = -20;
        calibrationData->m.rl.motor2 = -20;
        calibrationData->m.rr.motor1 = -20;
        calibrationData->m.rr.motor2 = -20;
        _calibrationCounter ++;
      } else {
        // read current or button
        current = getCurrent1Inputs();
        if (_getButtonPressed() || (current > 39)) {
          if (current > 39) {
            calibrationData->front -= 25;
          }
          _calibrationCounter = 0;
          _calibrationStage = 0;
          _deviceMode = CALIBRATION_REAR;
        } else {
          calibrationData->front ++;
          if (calibrationData->front > 40) {
            calibrationData->front = -20;
          }
        }
      }
    }
  } else if (_deviceMode == CALIBRATION_REAR) {
    if (_calibrationStage == 0) {
      if (_calibrationCounter == 0) {
        // set initial leg calibration
        calibrationData->rear = 0;
        _calibrationCounter ++;
      } else {
        // read current or button
        current = getCurrent1Inputs();
        if (_getButtonPressed() || (current > 39)) {
          if (current > 39) {
            calibrationData->rear -= 25;
          }
          _calibrationCounter = 0;
          _calibrationStage = 0;
          _deviceMode = CALIBRATION_AUTO_1;
        } else {
          calibrationData->rear ++;
          if (calibrationData->rear > 40) {
            calibrationData->rear = -20;
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
          calibrationData->m.fl.motor1 = -5;
          patternCounter ++;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getCurrent2Inputs();
          if (_getButtonPressed() || (current > 39)) {
            if (current > 39) {
              calibrationData->m.fl.motor1 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            calibrationData->m.fl.motor1 ++;
            if (calibrationData->m.fl.motor1 > 30) {
              calibrationData->m.fl.motor1 = -20;
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
          calibrationData->m.fr.motor1 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getCurrent2Inputs();
          if (_getButtonPressed() || (current > 39)) {
            if (current > 39) {
              calibrationData->m.fr.motor1 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            calibrationData->m.fr.motor1 ++;
            if (calibrationData->m.fr.motor1 > 30) {
              calibrationData->m.fr.motor1 = -20;
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
          calibrationData->m.rl.motor1 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getCurrent3Inputs();
          if (_getButtonPressed() || (current > 39)) {
            if (current > 39) {
              calibrationData->m.rl.motor1 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            calibrationData->m.rl.motor1 ++;
            if (calibrationData->m.rl.motor1 > 30) {
              calibrationData->m.rl.motor1 = -20;
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
          calibrationData->m.rr.motor1 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getCurrent3Inputs();
          if (_getButtonPressed() || (current > 39)) {
            if (current > 39) {
              calibrationData->m.rr.motor1 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage = 0;
            _deviceMode = CALIBRATION_AUTO_2;
          } else {
            calibrationData->m.rr.motor1 ++;
            if (calibrationData->m.rr.motor1 > 30) {
              calibrationData->m.rr.motor1 = -20;
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
          calibrationData->m.fl.motor2 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getCurrent2Inputs();
          if (_getButtonPressed() || (current > 39)) {
            if (current > 39) {
              calibrationData->m.fl.motor2 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            calibrationData->m.fl.motor2 ++;
            if (calibrationData->m.fl.motor2 > 30) {
              calibrationData->m.fl.motor2 = -20;
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
          calibrationData->m.fr.motor2 = -5;
          patternCounter ++;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getCurrent2Inputs();
          if (_getButtonPressed() || (current > 39)) {
            if (current > 39) {
              calibrationData->m.fr.motor2 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            calibrationData->m.fr.motor2 ++;
            if (calibrationData->m.fr.motor2 > 30) {
              calibrationData->m.fr.motor2 = -20;
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
          calibrationData->m.rl.motor2 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getCurrent3Inputs();
          if (_getButtonPressed() || (current > 39)) {
            if (current > 39) {
              calibrationData->m.rl.motor2 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage = 0;
            // end of calibration
            patternCounter ++;
            _deviceMode = CALIBRATION_SAVE;
          } else {
            calibrationData->m.rl.motor2 ++;
            if (calibrationData->m.rl.motor2 > 30) {
              calibrationData->m.rl.motor2 = -20;
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
          calibrationData->m.rr.motor2 = -5;
          _calibrationCounter ++;
        } else {
          // read current or button
          current = getCurrent3Inputs();
          if (_getButtonPressed() || (current > 39)) {
            if (current > 39) {
              calibrationData->m.rr.motor2 -= 10;
            }
            _calibrationCounter = 0;
            _calibrationStage ++;
          } else {
            calibrationData->m.rr.motor2 ++;
            if (calibrationData->m.rr.motor2 > 30) {
              calibrationData->m.rr.motor2 = -20;
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
  if (_modePressed) {
    _modePressed = false;
    return true;
  }
  return false;
}

// set mode button pressed
void setButtonCalibration(void) {
  _modePressed = true;
}
