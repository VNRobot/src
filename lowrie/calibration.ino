/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
motors calibration
*/

// device mode enumerator
enum calMode {
  CALIBRATION_INFO = 0,
  CALIBRATION_START = 1,
  CALIBRATION_AUTO = 2,
  CALIBRATION_SAVE = 3,
  CALIBRATION_DONE = 4
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

// true if software version from eeprom is right
bool rightSoftwareVersionEeprom() {
  unsigned char version = EEPROM.read(0);
  // software version address is 0
  if (version == ROBOT_VERSION) {
    return true;
  } else {
    return false;
  }
}

// write calibration to eeprom
void writeCalibrationEeprom(void) {
  // calibration address is 1 to 8
  EEPROM.update(1, (unsigned char)(m_calibrationData.fl.motor1 + 128));
  EEPROM.update(2, (unsigned char)(m_calibrationData.fl.motor2 + 128));
  EEPROM.update(3, (unsigned char)(m_calibrationData.fr.motor1 + 128));
  EEPROM.update(4, (unsigned char)(m_calibrationData.fr.motor2 + 128));
  EEPROM.update(5, (unsigned char)(m_calibrationData.rl.motor1 + 128));
  EEPROM.update(6, (unsigned char)(m_calibrationData.rl.motor2 + 128));
  EEPROM.update(7, (unsigned char)(m_calibrationData.rr.motor1 + 128));
  EEPROM.update(8, (unsigned char)(m_calibrationData.rr.motor2 + 128));
  // software version address is 0
#ifdef BOARD_ESP32
  EEPROM.write(0, ROBOT_VERSION);
  EEPROM.commit();
#else
  EEPROM.update(0, ROBOT_VERSION);
#endif
}
/*
// get current of first set of outputs
short _getCurrent1Inputs(void) {
  current = analogRead(A6) - analogRead(A7);
  doPWMServo(20);
  current += analogRead(A6) - analogRead(A7);
  doPWMServo(20);
  current += analogRead(A6) - analogRead(A7);
  doPWMServo(20);
  current += analogRead(A6) - analogRead(A7);
  current *= 2;
  //Serial.print(F(" current1 "));
  //Serial.println((int)current);
  if (current < 0) {
    return 0;
  }
  return current;
}
*/
// get current of front motors
short _getCurrent2Inputs(void) {
  current = analogRead(A6) - analogRead(A3);
  doPWMServo(20);
  current += analogRead(A6) - analogRead(A3);
  doPWMServo(20);
  current += analogRead(A6) - analogRead(A3);
  doPWMServo(20);
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
  doPWMServo(20);
  current += analogRead(A6) - analogRead(A2);
  doPWMServo(20);
  current += analogRead(A6) - analogRead(A2);
  doPWMServo(20);
  current += analogRead(A6) - analogRead(A2);
  current *= 2;
  //Serial.print(F(" current3 "));
  //Serial.println((int)current);
  if (current < 0) {
    return 0;
  }
  return current;
}

void _calibrateMotor1(motors * calibrationSet, short current) {
  if (calibrationCounter == 0) {
    // set initial leg calibration
    calibrationSet->motor1 = -30;
    calibrationCounter ++;
  } else {
    if ((modePressed) || (current > CALIBRATION_CURRENT)) {
      modePressed = false;
      if (current > CALIBRATION_CURRENT) {
        calibrationSet->motor1 -= 15;
      }
      calibrationCounter = 0;
      calibrationStage ++;
    } else {
      calibrationSet->motor1 ++;
      if (calibrationSet->motor1 > 30) {
        calibrationSet->motor1 = -30;
      }
    }
  }
}

void _calibrateMotor2(motors * calibrationSet, short current) {
  if (calibrationCounter == 0) {
    // set initial leg calibration
    calibrationSet->motor2 = -30;
    calibrationCounter ++;
  } else {
    if ((modePressed) || (current > CALIBRATION_CURRENT)) {
      modePressed = false;
      if (current > CALIBRATION_CURRENT) {
        calibrationSet->motor2 -= 15;
      }
      calibrationCounter = 0;
      calibrationStage ++;
    } else {
      calibrationSet->motor2 ++;
      if (calibrationSet->motor2 > 30) {
        calibrationSet->motor2 = -30;
      }
    }
  }
}

// calibration process starts here
bool doCalibration(void) {
  if (rightSoftwareVersionEeprom() && (analogRead(A6) > INPUT_GROUNDED)) {
    // read values
    // calibration address is 1 to 8
    m_calibrationData.fl.motor1 = (char)(EEPROM.read(1) - 128);
    m_calibrationData.fl.motor2 = (char)(EEPROM.read(2) - 128);
    m_calibrationData.fr.motor1 = (char)(EEPROM.read(3) - 128);
    m_calibrationData.fr.motor2 = (char)(EEPROM.read(4) - 128);
    m_calibrationData.rl.motor1 = (char)(EEPROM.read(5) - 128);
    m_calibrationData.rl.motor2 = (char)(EEPROM.read(6) - 128);
    m_calibrationData.rr.motor1 = (char)(EEPROM.read(7) - 128);
    m_calibrationData.rr.motor2 = (char)(EEPROM.read(8) - 128);
    return false;
  }
  Serial.println(F("Start calibration"));
  doPWMServo(2000);
  // set calibration mode
  deviceMode = CALIBRATION_INFO;
  // do loop
  while (true) {
    m_motorAngleValue[0] = _limitMotorValue(90 + m_calibrationData.fl.motor1);
    m_motorAngleValue[1] = _limitMotorValue(90 - m_calibrationData.fl.motor2);
    m_motorAngleValue[2] = _limitMotorValue(90 - m_calibrationData.fr.motor1);
    m_motorAngleValue[3] = _limitMotorValue(90 + m_calibrationData.fr.motor2);
    m_motorAngleValue[4] = _limitMotorValue(90 + m_calibrationData.rl.motor1);
    m_motorAngleValue[5] = _limitMotorValue(90 - m_calibrationData.rl.motor2);
    m_motorAngleValue[6] = _limitMotorValue(90 - m_calibrationData.rr.motor1);
    m_motorAngleValue[7] = _limitMotorValue(90 + m_calibrationData.rr.motor2);
    doPWMServo(200);
    if (analogRead(A6) < INPUT_GROUNDED) {
      //Serial.println(F("Button pressed"));
      modePressed = true;
      while (analogRead(A6) < INPUT_GROUNDED) {
        doPWMServo(100);
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
        updateGyro();
        // print proximity sensors
        Serial.print(F("Left eye "));
        Serial.print((int)((1600000 / analogRead(A0)) / analogRead(A0)));
        Serial.print(F(" Right eye "));
        Serial.print((int)((1600000 / analogRead(A1)) / analogRead(A1)));
        Serial.print(F(" Direction "));
        Serial.print((int)m_gyroState.direction);
        // motors current
        Serial.print(F(" Battery mv "));
        Serial.print((int)analogRead(A6));
        Serial.print(F(" front ma "));
        Serial.print((int)_getCurrent2Inputs());
        Serial.print(F(" rear ma "));
        Serial.println((int)_getCurrent3Inputs());
        doPWMServo(500);
      }
      break;
      case CALIBRATION_START: 
      {
        Serial.println(F("CALIBRATION_START"));
        // set initial leg calibration
        deviceMode = CALIBRATION_AUTO;
        m_calibrationData.fl.motor1 = -20;
        m_calibrationData.fl.motor2 = -20;
        m_calibrationData.fr.motor1 = -20;
        m_calibrationData.fr.motor2 = -20;
        m_calibrationData.rl.motor1 = -20;
        m_calibrationData.rl.motor2 = -20;
        m_calibrationData.rr.motor1 = -20;
        m_calibrationData.rr.motor2 = -20;
      } 
      break;
      case CALIBRATION_AUTO:
      {
        switch (calibrationStage) {
          case 0:
            Serial.println(F("CALIBRATION_FL_1"));
            _calibrateMotor1(& m_calibrationData.fl,  _getCurrent2Inputs());
          break;
          case 1:
            Serial.println(F("CALIBRATION_FL_2"));
            _calibrateMotor2(& m_calibrationData.fl,  _getCurrent2Inputs());
          break;
          case 2:
            Serial.println(F("CALIBRATION_RL_1"));
            _calibrateMotor1(& m_calibrationData.rl,  _getCurrent3Inputs());
          break;
          case 3:
            Serial.println(F("CALIBRATION_RL_2"));
            _calibrateMotor2(& m_calibrationData.rl,  _getCurrent3Inputs());
          break;
          case 4:
            Serial.println(F("CALIBRATION_FR_1"));
            _calibrateMotor1(& m_calibrationData.fr,  _getCurrent2Inputs());
          break;
          case 5:
            Serial.println(F("CALIBRATION_FR_2"));
            _calibrateMotor2(& m_calibrationData.fr,  _getCurrent2Inputs());
          break;
          case 6:
            Serial.println(F("CALIBRATION_RR_1"));
            _calibrateMotor1(& m_calibrationData.rr,  _getCurrent3Inputs());
          break;
          case 7:
            Serial.println(F("CALIBRATION_RR_2"));
            _calibrateMotor2(& m_calibrationData.rr,  _getCurrent3Inputs());
          break;
          // add wings calibration here
          case 8:
            calibrationCounter = 0;
            calibrationStage = 0;
            // end of calibration
            deviceMode = CALIBRATION_SAVE;
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
        Serial.print((int)m_calibrationData.fl.motor1);
        Serial.print(F(" FL2: "));
        Serial.print((int)m_calibrationData.fl.motor2);
        Serial.print(F(" FR1: "));
        Serial.print((int)m_calibrationData.fr.motor1);
        Serial.print(F(" FR2: "));
        Serial.print((int)m_calibrationData.fr.motor2);
        Serial.print(F(" RL1: "));
        Serial.print((int)m_calibrationData.rl.motor1);
        Serial.print(F(" RL2: "));
        Serial.print((int)m_calibrationData.rl.motor2);
        Serial.print(F(" RR1: "));
        Serial.print((int)m_calibrationData.rr.motor1);
        Serial.print(F(" RR2: "));
        Serial.println((int)m_calibrationData.rr.motor2);
        // save calibration
        writeCalibrationEeprom();
      } 
      break;
      case CALIBRATION_DONE:
      { 
        Serial.println(F("Done. Please restart me."));
        // disable motors
        detachServo();
        return true;
      }
      break;
      default:
        Serial.println(F("Unknown factory stage"));
      break;
    }
  }
}
