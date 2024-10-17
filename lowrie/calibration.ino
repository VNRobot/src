/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
motors calibration
*/

// device mode enumerator
enum calMode {
  CALIBRATION_INFO = 0,
  CALIBRATION_START = 1,
  CALIBRATION_FRONT = 2,
  CALIBRATION_REAR = 3,
  CALIBRATION_AUTO_1 = 4,
  CALIBRATION_AUTO_2 = 5,
  CALIBRATION_SAVE = 6,
  CALIBRATION_DONE = 7
};

// calibration counter
unsigned char calibrationCounter = 0;
// calibration stage
unsigned char calibrationStage = 0;
// mode button press flag
bool modePressed = false;
// device mode
unsigned char deviceMode = CALIBRATION_INFO;
// current buffer
short current = 0;

// get current of center motors
short _getCurrent1Inputs(void) {
  current = analogRead(A6) - analogRead(A7);
  if (current < 0) {
    return 0;
  }
  return current;
}

// get current of front motors
short _getCurrent2Inputs(void) {
  current = analogRead(A6) - analogRead(A3);
  if (current < 0) {
    return 0;
  }
  return current;
}

// get current of rear motors
short _getCurrent3Inputs(void) {
  current = analogRead(A6) - analogRead(A2);
  if (current < 0) {
    return 0;
  }
  return current;
}

// calibration process starts here
bool doCalibration(allMotors * calibrationData) {
  Serial.println(F("Start calibration"));
  // set calibration mode
  deviceMode = CALIBRATION_INFO;
  // do loop
  while (true) {
    setServo(calibrationData, -60, 60);
    delay(200);
    if (analogRead(A6) < 400) {
      Serial.println(F("Button pressed"));
      modePressed = true;
      while (analogRead(A6) < 400) {
        delay(100);
      }
    }
    // 
    switch (deviceMode) {
      case CALIBRATION_INFO:
      {
        Serial.println(F("CALIBRATION_INFO"));
        if (modePressed) {
          modePressed = false;
          deviceMode = CALIBRATION_START;
        }
        // print proximity sensors
        Serial.print(F("Left eye "));
        Serial.print((int)analogRead(A0));
        Serial.print(F(" Right eye "));
        Serial.println((int)analogRead(A1));
        // print gyro data
        updateGyro(0);
        Serial.print(F("Direction "));
        Serial.println((int)getDirectionCorrectionGyro());
        // motors current
        Serial.print(F("Battery  "));
        Serial.print((int)analogRead(A6));
        Serial.print(F(" Current center "));
        Serial.print((int)_getCurrent1Inputs());
        Serial.print(F(" front "));
        Serial.print((int)_getCurrent2Inputs());
        Serial.print(F(" rear "));
        Serial.println((int)_getCurrent3Inputs());
      }
      break;
      case CALIBRATION_START: 
      {
        Serial.println(F("CALIBRATION_START"));
        deviceMode = CALIBRATION_FRONT;
      } 
      break;
      case CALIBRATION_FRONT:
      {
        Serial.println(F("CALIBRATION_FRONT"));
        if (calibrationStage == 0) {
          if (calibrationCounter == 0) {
            // set initial leg calibration
            calibrationData->front = -20;
            calibrationData->rear = -20;
            calibrationData->m.fl.motor1 = -30;
            calibrationData->m.fl.motor2 = -30;
            calibrationData->m.fr.motor1 = -30;
            calibrationData->m.fr.motor2 = -30;
            calibrationData->m.rl.motor1 = -30;
            calibrationData->m.rl.motor2 = -30;
            calibrationData->m.rr.motor1 = -30;
            calibrationData->m.rr.motor2 = -30;
            calibrationCounter ++;
          } else {
            // read current or button
            current = _getCurrent1Inputs();
            Serial.print(F(" current "));
            Serial.println((int)current);
            if (modePressed || (current > 139)) {
              modePressed = false;
              if (current > 139) {
                calibrationData->front -= 25;
              }
              calibrationCounter = 0;
              calibrationStage = 0;
              deviceMode = CALIBRATION_REAR;
            } else {
              calibrationData->front ++;
              if (calibrationData->front > 20) {
                calibrationData->front = -30;
              }
            }
          }
        }
      }
      break;
      case CALIBRATION_REAR:
      {
        Serial.println(F("CALIBRATION_REAR"));
        if (calibrationStage == 0) {
          if (calibrationCounter == 0) {
            // set initial leg calibration
            calibrationData->rear = -20;
            calibrationCounter ++;
          } else {
            // read current or button
            current = _getCurrent1Inputs();
            if (modePressed || (current > 139)) {
              modePressed = false;
              if (current > 139) {
                calibrationData->rear -= 25;
              }
              calibrationCounter = 0;
              calibrationStage = 0;
              deviceMode = CALIBRATION_AUTO_1;
            } else {
              calibrationData->rear ++;
              if (calibrationData->rear > 20) {
                calibrationData->rear = -30;
              }
            }
          }
        }
      }
      break;
      case CALIBRATION_AUTO_1:
      {
        Serial.println(F("CALIBRATION_AUTO_1"));
        // tune motor 1
        switch (calibrationStage) {
          case 0:
          {
            // leg FL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.fl.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent2Inputs();
              if (modePressed || (current > 139)) {
                modePressed = false;
                if (current > 139) {
                  calibrationData->m.fl.motor1 -= 10;
                }
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.fl.motor1 ++;
                if (calibrationData->m.fl.motor1 > 30) {
                  calibrationData->m.fl.motor1 = -30;
                }
              }
            }
          }
          break;
          case 2:
          {
            // leg FR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.fr.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent2Inputs();
              if (modePressed || (current > 139)) {
                modePressed = false;
                if (current > 139) {
                  calibrationData->m.fr.motor1 -= 10;
                }
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.fr.motor1 ++;
                if (calibrationData->m.fr.motor1 > 30) {
                  calibrationData->m.fr.motor1 = -30;
                }
              }
            }
          }
          break;
          case 1:
          {
            // leg RL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.rl.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent3Inputs();
              if (modePressed || (current > 139)) {
                modePressed = false;
                if (current > 139) {
                  calibrationData->m.rl.motor1 -= 10;
                }
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.rl.motor1 ++;
                if (calibrationData->m.rl.motor1 > 30) {
                  calibrationData->m.rl.motor1 = -30;
                }
              }
            }
          }
          break;
          case 3:
          {
            // leg RR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.rr.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent3Inputs();
              if (modePressed || (current > 139)) {
                modePressed = false;
                if (current > 139) {
                  calibrationData->m.rr.motor1 -= 10;
                }
                calibrationCounter = 0;
                calibrationStage = 0;
                deviceMode = CALIBRATION_AUTO_2;
              } else {
                calibrationData->m.rr.motor1 ++;
                if (calibrationData->m.rr.motor1 > 30) {
                  calibrationData->m.rr.motor1 = -30;
                }
              }
            }
          }
          break;
          default:
          break;
        }
      }
      break;
      case CALIBRATION_AUTO_2: 
      {
        Serial.println(F("CALIBRATION_AUTO_2"));
        // tune motor 2
        switch (calibrationStage) {
          case 2:
          {
            // leg FL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.fl.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent2Inputs();
              if (modePressed || (current > 139)) {
                modePressed = false;
                if (current > 139) {
                  calibrationData->m.fl.motor2 -= 10;
                }
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.fl.motor2 ++;
                if (calibrationData->m.fl.motor2 > 30) {
                  calibrationData->m.fl.motor2 = -30;
                }
              }
            }
          }
          break;
          case 0:
          {
            // leg FR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.fr.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent2Inputs();
              if (modePressed || (current > 139)) {
                modePressed = false;
                if (current > 139) {
                  calibrationData->m.fr.motor2 -= 10;
                }
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.fr.motor2 ++;
                if (calibrationData->m.fr.motor2 > 30) {
                  calibrationData->m.fr.motor2 = -30;
                }
              }
            }
          }
          break;
          case 3:
          {
            // leg RL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.rl.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent3Inputs();
              if (modePressed || (current > 139)) {
                modePressed = false;
                if (current > 139) {
                  calibrationData->m.rl.motor2 -= 10;
                }
                calibrationCounter = 0;
                calibrationStage = 0;
                // end of calibration
                deviceMode = CALIBRATION_SAVE;
              } else {
                calibrationData->m.rl.motor2 ++;
                if (calibrationData->m.rl.motor2 > 30) {
                  calibrationData->m.rl.motor2 = -30;
                }
              }
            }
          }
          break;
          case 1:
          {
            // leg RR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.rr.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent3Inputs();
              if (modePressed || (current > 139)) {
                modePressed = false;
                if (current > 139) {
                  calibrationData->m.rr.motor2 -= 10;
                }
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.rr.motor2 ++;
                if (calibrationData->m.rr.motor2 > 30) {
                  calibrationData->m.rr.motor2 = -30;
                }
              }
            }
          }
          break;
          default:
          break;
        }
      } 
      break;
      case CALIBRATION_SAVE:
      {
        deviceMode = CALIBRATION_DONE;
        Serial.println(F("Calibration data"));
        Serial.print(F("Motors FL1: "));
        Serial.print((int)calibrationData->m.fl.motor1);
        Serial.print(F(" FL2: "));
        Serial.print((int)calibrationData->m.fl.motor2);
        Serial.print(F(" FR1: "));
        Serial.print((int)calibrationData->m.fr.motor1);
        Serial.print(F(" FR2: "));
        Serial.print((int)calibrationData->m.fr.motor2);
        Serial.print(F(" RL1: "));
        Serial.print((int)calibrationData->m.rl.motor1);
        Serial.print(F(" RL2: "));
        Serial.print((int)calibrationData->m.rl.motor2);
        Serial.print(F(" RR1: "));
        Serial.print((int)calibrationData->m.rr.motor1);
        Serial.print(F(" RR2: "));
        Serial.println((int)calibrationData->m.rr.motor2);
      } 
      break;
      case CALIBRATION_DONE:
      { 
        Serial.println(F("Done. Please reset or restart."));
        return true;
      }
      break;
      default:
        Serial.println(F("Unknown factory stage"));
      break;
    }
  }
  return false;
}
