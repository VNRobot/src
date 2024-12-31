/*
Simple Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
motors calibration
*/

// device mode enumerator
enum calMode {
  CALIBRATION_INFO = 0,
  CALIBRATION_START = 1,
  CALIBRATION_LEFT = 2,
  CALIBRATION_RIGHT = 3,
  CALIBRATION_AUTO_1 = 4,
  CALIBRATION_AUTO_2 = 5,
  CALIBRATION_SAVE = 6,
  CALIBRATION_DONE = 7
};

// calibration counter
unsigned char calibrationCounter = 0;
// mode button press flag
bool modePressed = false;
// device mode
unsigned char deviceMode = CALIBRATION_INFO;
// current buffer
short current = 0;
// wing angle
int wingAngle = 0;

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
    setServo(calibrationData, wingAngle, wingAngle);
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
        Serial.print((int)getDirectionCorrectionGyro());
        // motors current
        Serial.print(F(" Battery  "));
        Serial.print((int)analogRead(A6));
        Serial.print(F(" Current1 "));
        Serial.print((int)_getCurrent1Inputs());
        Serial.print(F(" Current2 "));
        Serial.print((int)_getCurrent2Inputs());
        Serial.print(F(" Current3 "));
        Serial.println((int)_getCurrent3Inputs());
        delay(500);
      }
      break;
      case CALIBRATION_START: 
      {
        Serial.println(F("CALIBRATION_START"));
        deviceMode = CALIBRATION_LEFT;
      } 
      break;
      case CALIBRATION_LEFT:
      {
        Serial.println(F("CALIBRATION_LEFT"));
        if (calibrationCounter == 0) {
          // set initial leg calibration
          wingAngle = 50;
          calibrationData->left = -20;
          calibrationData->right = -20;
          calibrationData->m.wheel.motorL = -20;
          calibrationData->m.wheel.motorR = -20;
          calibrationCounter ++;
        } else {
          // read current or button
          current = _getCurrent2Inputs();
          if (modePressed || (current > 512)) {
            modePressed = false;
            if (current > 512) {
              calibrationData->left -= 20;
            }
            calibrationCounter = 0;
            deviceMode = CALIBRATION_RIGHT;
          } else {
            calibrationData->left ++;
            if (calibrationData->left > 40) {
              calibrationData->left = -20;
            }
          }
        }
      }
      break;
      case CALIBRATION_RIGHT:
      {
        Serial.println(F("CALIBRATION_RIGHT"));
        if (calibrationCounter == 0) {
          // set initial leg calibration
          calibrationData->right = -20;
          calibrationCounter ++;
        } else {
          // read current or button
          current = _getCurrent3Inputs();
          if (modePressed || (current > 512)) {
            modePressed = false;
            if (current > 512) {
              calibrationData->right -= 20;
            }
            calibrationCounter = 0;
            deviceMode = CALIBRATION_AUTO_1;
          } else {
            calibrationData->right ++;
            if (calibrationData->right > 40) {
              calibrationData->right = -20;
            }
          }
        }
      }
      break;
      case CALIBRATION_AUTO_1:
      {
        // tune motor 1
        Serial.println(F("CALIBRATION_FL"));
        // leg FL
        if (calibrationCounter == 0) {
          // set initial leg calibration
          wingAngle = 0;
          calibrationData->m.wheel.motorL = -20;
          calibrationCounter ++;
        } else {
          // read current or button
          current = _getCurrent1Inputs();
          if (modePressed || (current > 512)) {
            modePressed = false;
            if (current > 512) {
              calibrationData->m.wheel.motorL -= 15;
            }
            calibrationCounter = 0;
            deviceMode = CALIBRATION_AUTO_2;
          } else {
            calibrationData->m.wheel.motorL ++;
            if (calibrationData->m.wheel.motorL > 20) {
              calibrationData->m.wheel.motorL = -20;
            }
          }
        }
      }
      break;
      case CALIBRATION_AUTO_2: 
      {
        // tune motor 2
        Serial.println(F("CALIBRATION_FR"));
        // leg FL
        if (calibrationCounter == 0) {
          // set initial leg calibration
          calibrationData->m.wheel.motorR = -20;
          calibrationCounter ++;
        } else {
          // read current or button
          current = _getCurrent1Inputs();
          if (modePressed || (current > 512)) {
            modePressed = false;
            if (current > 512) {
              calibrationData->m.wheel.motorR -= 15;
            }
            calibrationCounter = 0;
            deviceMode = CALIBRATION_SAVE;
          } else {
            calibrationData->m.wheel.motorR ++;
            if (calibrationData->m.wheel.motorR > 20) {
              calibrationData->m.wheel.motorR = -20;
            }
          }
        }
      }
      break;
      case CALIBRATION_SAVE:
      {
        deviceMode = CALIBRATION_DONE;
        Serial.println(F("Calibration data"));
        Serial.print(F("Motors WL: "));
        Serial.print((int)calibrationData->m.wheel.motorL);
        Serial.print(F(" WR: "));
        Serial.println((int)calibrationData->m.wheel.motorR);
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
