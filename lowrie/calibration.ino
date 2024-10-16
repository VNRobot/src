/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
motors calibration
*/

// device mode enumerator
enum rMode {
  CALIBRATION_INFO,
  CALIBRATION_START,
  CALIBRATION_FRONT,
  CALIBRATION_REAR,
  CALIBRATION_AUTO_1,
  CALIBRATION_AUTO_2,
  CALIBRATION_SAVE,
  CALIBRATION_DONE
};

// calibration counter
unsigned char _calibrationCounter = 0;
// calibration stage
unsigned char _calibrationStage = 0;
// mode button press flag
bool _modePressed = false;
// device mode
unsigned char _deviceMode = CALIBRATION_INFO;

// calibration process starts here
bool doCalibration(allMotors * calibration) {
  // set calibration mode
  setModeCalibration(CALIBRATION_INFO);
  // factory mode is used for legs calibration
  Serial.println("Entering factory mode");
  // init servo motors
  initServo(-60, 60);
  // set motors values after calibration
  setServo(m_calibration, -60, 60);

  while (true) {
    delay(200);
  }
  return true;
}

// set device mode
void setModeCalibration(unsigned char mode) {
  _deviceMode = mode;
}

// get device mode
unsigned char getModeCalibration(void) {
  return _deviceMode;
}
/*
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

// factory mode call
void factoryModeCall(void) {
  // get next pattern
  patternNow = getNextPatternInTask();
  setPattern(patternNow);
  // update factory stage
  if (_readButtonPress()) {
    if (getModeCalibration() < CALIBRATION_START) {
      setModeCalibration(CALIBRATION_START);
    } else {
      setButtonCalibration();
    }
    Serial.print("Factory stage set to ");
    Serial.println((int)getModeCalibration());
  }
  // factory mode stages
  switch (getModeCalibration()) {
    case CALIBRATION_INFO: 
      // print proximity sensors
      Serial.print("Left eye ");
      Serial.print((int)analogRead(A0));
      Serial.print(" Right eye ");
      Serial.println((int)analogRead(A1));
      // print gyro data
      Serial.print("Direction ");
      Serial.println((int)getDirectionCorrectionGyro());
      // motors current
      Serial.print("Battery  ");
      Serial.print((int)analogRead(A6));
      Serial.print(" Current center ");
      Serial.print((int)getCurrent1Inputs());
      Serial.print(" front ");
      Serial.print((int)getCurrent2Inputs());
      Serial.print(" rear ");
      Serial.println((int)getCurrent3Inputs());
    break;
    case CALIBRATION_START: 
    {
      setModeCalibration(CALIBRATION_FRONT);
      Serial.println("Starting legs calibration");
    } 
    break;
    case CALIBRATION_FRONT: 
    case CALIBRATION_REAR:
    case CALIBRATION_AUTO_1: 
    case CALIBRATION_AUTO_2: 
    {
      setPointerInTask(legsCalibration(getPointerInTask(), & m_calibration));
    } 
    break;
    case CALIBRATION_SAVE:
    {
      setModeCalibration(CALIBRATION_DONE);
      Serial.println("Saving calibration data");
      writeCalibrationEeprom(m_calibration);
      writeSoftwareVersionEeprom();
      Serial.print("Motors FL1: ");
      Serial.print((int)m_calibration.m.fl.motor1);
      Serial.print(" FL2: ");
      Serial.print((int)m_calibration.m.fl.motor2);
      Serial.print(" FR1: ");
      Serial.print((int)m_calibration.m.fr.motor1);
      Serial.print(" FR2: ");
      Serial.print((int)m_calibration.m.fr.motor2);
      Serial.print(" RL1: ");
      Serial.print((int)m_calibration.m.rl.motor1);
      Serial.print(" RL2: ");
      Serial.print((int)m_calibration.m.rl.motor2);
      Serial.print(" RR1: ");
      Serial.print((int)m_calibration.m.rr.motor1);
      Serial.print(" RR2: ");
      Serial.println((int)m_calibration.m.rr.motor2);
    } 
    break;
    case CALIBRATION_DONE: 
      Serial.println("Done. Please reset or restart.");
    break;
    default:
      Serial.println("Unknown factory stage");
    break;
  }
}
*/