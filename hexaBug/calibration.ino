/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
motors calibration
*/

// device mode enumerator
enum calMode {
  CALIBRATION_INFO = 0,
  CALIBRATION_START = 1,
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
  delay(20);
  current += analogRead(A6) - analogRead(A7);
  delay(20);
  current += analogRead(A6) - analogRead(A7);
  delay(20);
  current += analogRead(A6) - analogRead(A7);
  current *= 2;
  //Serial.print(F(" current1 "));
  //Serial.println((int)current);
  if (current < 0) {
    return 0;
  }
  return current;
}

// get current of front motors
short _getCurrent2Inputs(void) {
  current = analogRead(A6) - analogRead(A3);
  delay(20);
  current += analogRead(A6) - analogRead(A3);
  delay(20);
  current += analogRead(A6) - analogRead(A3);
  delay(20);
  current += analogRead(A6) - analogRead(A3);
  current *= 2;
  //Serial.print(F(" current2 "));
  //Serial.println((int)current);
  if (current < 0) {
    return 0;
  }
  return current;
}

// get current of rear motors
short _getCurrent3Inputs(void) {
  current = analogRead(A6) - analogRead(A2);
  delay(20);
  current += analogRead(A6) - analogRead(A2);
  delay(20);
  current += analogRead(A6) - analogRead(A2);
  delay(20);
  current += analogRead(A6) - analogRead(A2);
  current *= 2;
  //Serial.print(F(" current3 "));
  //Serial.println((int)current);
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
    delay(100);
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
        if (modePressed) {
          modePressed = false;
          deviceMode = CALIBRATION_START;
        }
        // print gyro data
        updateGyro(0);
        // print proximity sensors
        Serial.print(F("Left eye "));
        Serial.print((int)((1600000 / analogRead(A0)) / analogRead(A0)));
        Serial.print(F(" Right eye "));
        Serial.print((int)((1600000 / analogRead(A1)) / analogRead(A1)));
        Serial.print(F(" Direction "));
        Serial.print((int)getDirectionGyro());
        // motors current
        Serial.print(F(" Battery  "));
        Serial.print((int)analogRead(A6));
        Serial.print(F(" Current center "));
        Serial.print((int)_getCurrent1Inputs());
        Serial.print(F(" front "));
        Serial.print((int)_getCurrent2Inputs());
        Serial.print(F(" rear "));
        Serial.println((int)_getCurrent3Inputs());
        delay(500);
      }
      break;
      case CALIBRATION_START: 
      {
        Serial.println(F("CALIBRATION_START"));
        deviceMode = CALIBRATION_AUTO_1;
        // set initial leg calibration
        calibrationData->m.sl.motor1 = -30;
        calibrationData->m.sl.motor2 = -30;
        calibrationData->m.sr.motor1 = -30;
        calibrationData->m.sr.motor2 = -30;
        calibrationData->m.fl.motor1 = -30;
        calibrationData->m.fl.motor2 = -30;
        calibrationData->m.fr.motor1 = -30;
        calibrationData->m.fr.motor2 = -30;
        calibrationData->m.rl.motor1 = -30;
        calibrationData->m.rl.motor2 = -30;
        calibrationData->m.rr.motor1 = -30;
        calibrationData->m.rr.motor2 = -30;
        calibrationCounter = 0;
        calibrationStage = 0;
      } 
      break;
      case CALIBRATION_AUTO_1:
      {
        // tune motor 1
        switch (calibrationStage) {
          case 0:
          {
            Serial.println(F("CALIBRATION_FL_1"));
            // leg FL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.fl.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent2Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.fl.motor1 -= 15;
                //}
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
          case 3:
          {
            Serial.println(F("CALIBRATION_FR_1"));
            // leg FR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.fr.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent2Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.fr.motor1 -= 15;
                //}
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
          case 2:
          {
            Serial.println(F("CALIBRATION_RL_1"));
            // leg RL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.rl.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent3Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.rl.motor1 -= 15;
                //}
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
          case 5:
          {
            Serial.println(F("CALIBRATION_RR_1"));
            // leg RR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.rr.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent3Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.rr.motor1 -= 15;
                //}
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
          case 1:
          {
            Serial.println(F("CALIBRATION_SL_1"));
            // leg RL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.sl.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent1Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.sl.motor1 -= 15;
                //}
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.sl.motor1 ++;
                if (calibrationData->m.sl.motor1 > 30) {
                  calibrationData->m.sl.motor1 = -30;
                }
              }
            }
          }
          break;
          case 4:
          {
            Serial.println(F("CALIBRATION_SR_1"));
            // leg RR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.sr.motor1 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent1Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.sr.motor1 -= 15;
                //}
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.sr.motor1 ++;
                if (calibrationData->m.sr.motor1 > 30) {
                  calibrationData->m.sr.motor1 = -30;
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
        // tune motor 2
        switch (calibrationStage) {
          case 0:
          {
            Serial.println(F("CALIBRATION_FL_2"));
            // leg FL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.fl.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent2Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.fl.motor2 -= 15;
                //}
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
          case 3:
          {
            Serial.println(F("CALIBRATION_FR_2"));
            // leg FR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.fr.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent2Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.fr.motor2 -= 15;
                //}
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
          case 2:
          {
            Serial.println(F("CALIBRATION_RL_2"));
            // leg RL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.rl.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent3Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.rl.motor2 -= 15;
                //}
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.rl.motor2 ++;
                if (calibrationData->m.rl.motor2 > 30) {
                  calibrationData->m.rl.motor2 = -30;
                }
              }
            }
          }
          break;
          case 5:
          {
            Serial.println(F("CALIBRATION_RR_2"));
            // leg RR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.rr.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent3Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.rr.motor2 -= 15;
                //}
                calibrationCounter = 0;
                calibrationStage = 0;
                // end of calibration
                deviceMode = CALIBRATION_SAVE;
              } else {
                calibrationData->m.rr.motor2 ++;
                if (calibrationData->m.rr.motor2 > 30) {
                  calibrationData->m.rr.motor2 = -30;
                }
              }
            }
          }
          break;
          case 1:
          {
            Serial.println(F("CALIBRATION_SL_2"));
            // leg RL
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.sl.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent1Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.sl.motor2 -= 15;
                //}
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.sl.motor2 ++;
                if (calibrationData->m.sl.motor2 > 30) {
                  calibrationData->m.sl.motor2 = -30;
                }
              }
            }
          }
          break;
          case 4:
          {
            Serial.println(F("CALIBRATION_SR_2"));
            // leg RR
            if (calibrationCounter == 0) {
              // set initial leg calibration
              calibrationData->m.sr.motor2 = -30;
              calibrationCounter ++;
            } else {
              // read current or button
              current = _getCurrent1Inputs();
              if (modePressed) { // || (current > 512)) {
                modePressed = false;
                //if (current > 512) {
                //  calibrationData->m.sr.motor2 -= 15;
                //}
                calibrationCounter = 0;
                calibrationStage ++;
              } else {
                calibrationData->m.sr.motor2 ++;
                if (calibrationData->m.sr.motor2 > 30) {
                  calibrationData->m.sr.motor2 = -30;
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
