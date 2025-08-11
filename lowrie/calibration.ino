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
  CALIBRATION_AUTO = 4,
  CALIBRATION_SAVE = 5,
  CALIBRATION_DONE = 6
};

// auto calibration enabled
bool autoCalibrationEnabled = true;
// calibration current
unsigned short calibrationCurrent = 640; //ma
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

void _calibrateMotor1(motors * calibrationSet, short current) {
  if (calibrationCounter == 0) {
    // set initial leg calibration
    calibrationSet->motor1 = -30;
    calibrationCounter ++;
  } else {
    if ((modePressed) || ((current > calibrationCurrent) && autoCalibrationEnabled)) {
      modePressed = false;
      if ((current > calibrationCurrent) && autoCalibrationEnabled) {
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
    if ((modePressed) || ((current > calibrationCurrent) && autoCalibrationEnabled)) {
      modePressed = false;
      if ((current > calibrationCurrent) && autoCalibrationEnabled) {
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
bool doCalibration(allMotors * calibrationData) {
  initGyro();
  delay(200);
  resetGyro();
  delay(20);
  // init motors one by one
  if (m_init.motorsCount == 12) {
    m_servo_hl_1.attach(HL1_MOTOR, 500, 2500);
    m_servo_hl_1.write(90 - 30 + 45);
    delay(100);
    m_servo_hl_2.attach(HL2_MOTOR, 500, 2500);
    m_servo_hl_2.write(90 + 30 - 45);
    delay(100);
    m_servo_hr_1.attach(HR1_MOTOR, 500, 2500);
    m_servo_hr_1.write(90 + 30 - 45);
    delay(100);
    m_servo_hr_2.attach(HR2_MOTOR, 500, 2500);
    m_servo_hr_2.write(90 - 30 + 45);
    delay(100);
  } else if (m_init.motorsCount == 10) {
    m_servo_hr_1.attach(HR1_MOTOR, 500, 2500);
    m_servo_hr_1.write(90);
    delay(100);
    m_servo_hr_2.attach(HR2_MOTOR, 500, 2500);
    m_servo_hr_2.write(90);
    delay(100);
  }
  m_servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  m_servo_fl_1.write(90 - 30 + 45);
  delay(100);
  m_servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  m_servo_fr_1.write(90 + 30 - 45);
  delay(100);
  m_servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  m_servo_rl_1.write(90 - 30 + 45);
  delay(100);
  m_servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  m_servo_rr_1.write(90 + 30 - 45);
  delay(100);
  m_servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  m_servo_fl_2.write(90 + 30 - 45);
  delay(100);
  m_servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  m_servo_fr_2.write(90 - 30 + 45);
  delay(100);
  m_servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  m_servo_rl_2.write(90 + 30 - 45);
  delay(100);
  m_servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  m_servo_rr_2.write(90 - 30 + 45);
  delay(100);
  Serial.println(F("Start calibration"));
  // set calibration mode
  deviceMode = CALIBRATION_INFO;
  // do loop
  while (true) {
    if (m_init.motorsCount == 12) {
      m_servo_hl_1.write(90 - 30 + 90 + calibrationData->hl.motor1);
      m_servo_hl_2.write(90 + 30 - 90 - calibrationData->hl.motor2);
      m_servo_hr_1.write(90 + 30 - 90 - calibrationData->hr.motor1);
      m_servo_hr_2.write(90 - 30 + 90 + calibrationData->hr.motor2);
    } else if (m_init.motorsCount == 10) {
      m_servo_hr_1.write(90 - calibrationData->hr.motor1);
      m_servo_hr_2.write(90 - calibrationData->hr.motor2);
    }
    m_servo_fl_1.write(90 - 30 + 90 + calibrationData->fl.motor1);
    m_servo_fl_2.write(90 + 30 - 90 - calibrationData->fl.motor2);
    m_servo_fr_1.write(90 + 30 - 90 - calibrationData->fr.motor1);
    m_servo_fr_2.write(90 - 30 + 90 + calibrationData->fr.motor2);
    m_servo_rl_1.write(90 - 30 + 90 + calibrationData->rl.motor1);
    m_servo_rl_2.write(90 + 30 - 90 - calibrationData->rl.motor2);
    m_servo_rr_1.write(90 + 30 - 90 - calibrationData->rr.motor1);
    m_servo_rr_2.write(90 - 30 + 90 + calibrationData->rr.motor2);
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
        Serial.print(F(" Current head "));
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
        // set initial leg calibration
        if (m_init.motorsCount == 12) {
          calibrationData->hl.motor1 = -30;
          calibrationData->hl.motor2 = -30;
          calibrationData->hr.motor1 = -30;
          calibrationData->hr.motor2 = -30;
          deviceMode = CALIBRATION_AUTO;
        } else if (m_init.motorsCount == 10) {
          calibrationData->hr.motor1 = 0;
          calibrationData->hr.motor2 = 0;
          deviceMode = CALIBRATION_FRONT;
        } else {
          deviceMode = CALIBRATION_AUTO;
        }
        calibrationData->fl.motor1 = -30;
        calibrationData->fl.motor2 = -30;
        calibrationData->fr.motor1 = -30;
        calibrationData->fr.motor2 = -30;
        calibrationData->rl.motor1 = -30;
        calibrationData->rl.motor2 = -30;
        calibrationData->rr.motor1 = -30;
        calibrationData->rr.motor2 = -30;
      } 
      break;
      case CALIBRATION_FRONT:
      {
        Serial.println(F("CALIBRATION_FRONT"));
        if (calibrationCounter == 0) {
          // set initial leg calibration
          calibrationData->hr.motor1 = -20;
          calibrationCounter ++;
        } else {
          // read current or button
          current = _getCurrent1Inputs();
          if (modePressed || ((current > calibrationCurrent) && autoCalibrationEnabled)) {
            modePressed = false;
            if ((current > calibrationCurrent) && autoCalibrationEnabled) {
              calibrationData->hr.motor1 -= 25;
            }
            calibrationCounter = 0;
            deviceMode = CALIBRATION_REAR;
          } else {
            calibrationData->hr.motor1 ++;
            if (calibrationData->hr.motor1 > 20) {
              calibrationData->hr.motor1 = -30;
            }
          }
        }
      }
      break;
      case CALIBRATION_REAR:
      {
        Serial.println(F("CALIBRATION_REAR"));
        if (calibrationCounter == 0) {
          // set initial leg calibration
          calibrationData->hr.motor2 = -20;
          calibrationCounter ++;
        } else {
          // read current or button
          current = _getCurrent1Inputs();
          if (modePressed || ((current > calibrationCurrent) && autoCalibrationEnabled)) {
            modePressed = false;
            if ((current > calibrationCurrent) && autoCalibrationEnabled) {
              calibrationData->hr.motor2 -= 25;
            }
            calibrationCounter = 0;
            deviceMode = CALIBRATION_AUTO;
          } else {
            calibrationData->hr.motor2 ++;
            if (calibrationData->hr.motor2 > 20) {
              calibrationData->hr.motor2 = -30;
            }
          }
        }
      }
      break;
      case CALIBRATION_AUTO:
      {
        switch (calibrationStage) {
          case 0:
          {
            if (m_init.motorsCount == 12) {
              Serial.println(F("CALIBRATION_HL_1"));
              _calibrateMotor1(& calibrationData->hl,  _getCurrent1Inputs());
            } else {
              calibrationCounter = 0;
              calibrationStage += 2;
            }
          }
          break;
          case 1:
            Serial.println(F("CALIBRATION_HL_2"));
            _calibrateMotor2(& calibrationData->hl,  _getCurrent1Inputs());
          break;
          case 2:
            Serial.println(F("CALIBRATION_FL_1"));
            _calibrateMotor1(& calibrationData->fl,  _getCurrent2Inputs());
          break;
          case 3:
            Serial.println(F("CALIBRATION_FL_2"));
            _calibrateMotor2(& calibrationData->fl,  _getCurrent2Inputs());
          break;
          case 4:
            Serial.println(F("CALIBRATION_RL_1"));
            _calibrateMotor1(& calibrationData->rl,  _getCurrent3Inputs());
          break;
          case 5:
            Serial.println(F("CALIBRATION_RL_2"));
            _calibrateMotor2(& calibrationData->rl,  _getCurrent3Inputs());
          break;
          case 6:
          {
            if (m_init.motorsCount == 12) {
              Serial.println(F("CALIBRATION_HR_1"));
              _calibrateMotor1(& calibrationData->hr,  _getCurrent1Inputs());
            } else {
              calibrationCounter = 0;
              calibrationStage += 2;
            }
          }
          break;
          case 7:
            Serial.println(F("CALIBRATION_HR_2"));
            _calibrateMotor2(& calibrationData->hr,  _getCurrent1Inputs());
          break;
          case 8:
            Serial.println(F("CALIBRATION_FR_1"));
            _calibrateMotor1(& calibrationData->fr,  _getCurrent2Inputs());
          break;
          case 9:
            Serial.println(F("CALIBRATION_FR_2"));
            _calibrateMotor2(& calibrationData->fr,  _getCurrent2Inputs());
          break;
          case 10:
            Serial.println(F("CALIBRATION_RR_1"));
            _calibrateMotor1(& calibrationData->rr,  _getCurrent3Inputs());
          break;
          case 11:
            Serial.println(F("CALIBRATION_RR_2"));
            _calibrateMotor2(& calibrationData->rr,  _getCurrent3Inputs());
          break;
          // add wings calibration here
          case 12:
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
        Serial.print((int)calibrationData->fl.motor1);
        Serial.print(F(" FL2: "));
        Serial.print((int)calibrationData->fl.motor2);
        Serial.print(F(" FR1: "));
        Serial.print((int)calibrationData->fr.motor1);
        Serial.print(F(" FR2: "));
        Serial.print((int)calibrationData->fr.motor2);
        Serial.print(F(" RL1: "));
        Serial.print((int)calibrationData->rl.motor1);
        Serial.print(F(" RL2: "));
        Serial.print((int)calibrationData->rl.motor2);
        Serial.print(F(" RR1: "));
        Serial.print((int)calibrationData->rr.motor1);
        Serial.print(F(" RR2: "));
        Serial.println((int)calibrationData->rr.motor2);
      } 
      break;
      case CALIBRATION_DONE:
      { 
        Serial.println(F("Done. Please wait."));
        if (m_init.motorsCount == 12) {
          m_servo_hl_1.detach();
          m_servo_hl_2.detach();
        }
        if (m_init.motorsCount >= 10) {
          m_servo_hr_1.detach();
          m_servo_hr_2.detach();
        }
        m_servo_fl_1.detach();
        m_servo_fr_1.detach();
        m_servo_rl_1.detach();
        m_servo_rr_1.detach();
        m_servo_fl_2.detach();
        m_servo_fr_2.detach();
        m_servo_rl_2.detach();
        m_servo_rr_2.detach();
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
