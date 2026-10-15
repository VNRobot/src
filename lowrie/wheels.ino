/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nanos
Robot wheels motors motion patterns
*/

// wheel speed
#define WHEEL_SPEED           6

// pin numbers for servo motors
enum cPinsWheels {
  FL_WHEEL = 2,
  FR_WHEEL = 3,
  RL_WHEEL = 4,
  RR_WHEEL = 5
};
// legs motors structure
struct allWheels {
  char fl;
  char fr;
  char rl;
  char rr;
};

// init servo library
Servo servo_w_fl;
Servo servo_w_fr;
Servo servo_w_rl;
Servo servo_w_rr;

// motors attached flag
bool wheelsAttached = false;
// calibration data
allWheels wheelsCalibrationData = {0, 0, 0, 0};
// servo motor value
short wheelsMotorSpeedValue[4] = {0, 0, 0, 0};

/*
uses
m_legsValue
m_getButtonPressed()
*/

// limit angle value
short _limitWheelSpeedValue(short mSpeed) {
  if (mSpeed > 150) {
    mSpeed = 150;
  } else if (mSpeed < 30) {
    mSpeed = 30;
  }
  return mSpeed;
}

// do servo pwm cycle 500, 2500
void _doPWMWheels(void) {
  if (wheelsAttached) {
    servo_w_fl.write(wheelsMotorSpeedValue[0]);
    servo_w_fr.write(wheelsMotorSpeedValue[1]);
    servo_w_rl.write(wheelsMotorSpeedValue[2]);
    servo_w_rr.write(wheelsMotorSpeedValue[3]);
  }
}

// init servo motors
void attachWheels(void) {
  if (!wheelsAttached) {
    // set motors value
    wheelsMotorSpeedValue[0] = _limitWheelSpeedValue(90); 
    wheelsMotorSpeedValue[1] = _limitWheelSpeedValue(90); 
    wheelsMotorSpeedValue[2] = _limitWheelSpeedValue(90); 
    wheelsMotorSpeedValue[3] = _limitWheelSpeedValue(90); 
    // init motors one by one
    servo_w_fl.attach(FL_WHEEL, 500, 2500);
    servo_w_fl.write(wheelsMotorSpeedValue[0]);
    delay(100);
    servo_w_fr.attach(FR_WHEEL, 500, 2500);
    servo_w_fr.write(wheelsMotorSpeedValue[1]);
    delay(100);
    servo_w_rl.attach(RL_WHEEL, 500, 2500);
    servo_w_rl.write(wheelsMotorSpeedValue[2]);
    delay(100);
    servo_w_rr.attach(RR_WHEEL, 500, 2500);
    servo_w_rr.write(wheelsMotorSpeedValue[3]);
    delay(100);
    wheelsAttached = true;
  }
}

// init servo motors
void initWheels(bool calibrationMode) {
  if (wheelsAttached) {
    // check for calibration mode
    if (calibrationMode) {
      // do calibration
      char calibrationStage = 0;
      wheelsCalibrationData.fl = 0;
      wheelsCalibrationData.fr = 0;
      wheelsCalibrationData.rl = 0;
      wheelsCalibrationData.rr = 0;
      // motors one by one
      while (calibrationMode) {
        wheelsMotorSpeedValue[0] = _limitMotorValue(90 - wheelsCalibrationData.fl);
        wheelsMotorSpeedValue[1] = _limitMotorValue(90 - wheelsCalibrationData.fr);
        wheelsMotorSpeedValue[2] = _limitMotorValue(90 - wheelsCalibrationData.rl);
        wheelsMotorSpeedValue[3] = _limitMotorValue(90 - wheelsCalibrationData.rr);
        _doPWMWheels();
        delay(300);
        // check button press
        if (m_getButtonPressed()) {
          calibrationStage ++;
        }
        switch (calibrationStage) {
          case 0:
            // assembly stage. do nothing
          break;
          case 1:
          {
            wheelsCalibrationData.fl ++;
            if (wheelsCalibrationData.fl > CALIBRATION_ANGLE_MAX) {
              wheelsCalibrationData.fl = CALIBRATION_ANGLE_MIN;
            }
          }
          break;
          case 2:
          {
            wheelsCalibrationData.fr ++;
            if (wheelsCalibrationData.fr > CALIBRATION_ANGLE_MAX) {
              wheelsCalibrationData.fr = CALIBRATION_ANGLE_MIN;
            }
          }
          break;
          case 3:
          {
            wheelsCalibrationData.rl ++;
            if (wheelsCalibrationData.rl > CALIBRATION_ANGLE_MAX) {
              wheelsCalibrationData.rl = CALIBRATION_ANGLE_MIN;
            }
          }
          break;
          case 4:
          {
            wheelsCalibrationData.rr ++;
            if (wheelsCalibrationData.rr > CALIBRATION_ANGLE_MAX) {
              wheelsCalibrationData.rr = CALIBRATION_ANGLE_MIN;
            }
          }
          break;
          case 5:
          {
            EEPROM.update(9, (unsigned char)(wheelsCalibrationData.fl + 128));
            EEPROM.update(10, (unsigned char)(wheelsCalibrationData.fr + 128));
            EEPROM.update(11, (unsigned char)(wheelsCalibrationData.rl + 128));
            EEPROM.update(12, (unsigned char)(wheelsCalibrationData.rr + 128));
            calibrationStage ++;
            calibrationMode = false;
          }
          break;
          default:
          break;
        }
      }
    }
    // read calibration
    wheelsCalibrationData.fl = (char)(EEPROM.read(9) - 128);
    wheelsCalibrationData.fr = (char)(EEPROM.read(10) - 128);
    wheelsCalibrationData.rl = (char)(EEPROM.read(11) - 128);
    wheelsCalibrationData.rr = (char)(EEPROM.read(12) - 128);
    wheelsMotorSpeedValue[0] = _limitMotorValue(90 - wheelsCalibrationData.fl);
    wheelsMotorSpeedValue[1] = _limitMotorValue(90 - wheelsCalibrationData.fr);
    wheelsMotorSpeedValue[2] = _limitMotorValue(90 - wheelsCalibrationData.rl);
    wheelsMotorSpeedValue[3] = _limitMotorValue(90 - wheelsCalibrationData.rr);
    _doPWMWheels();
    delay(100);
  }
}

// detach hardware
void detachWheels(void) {
  if (wheelsAttached) {
    servo_w_fl.detach();
    servo_w_fr.detach();
    servo_w_rl.detach();
    servo_w_rr.detach();
    wheelsAttached = false;
    delay(100);
  }
}

// set direction. 20 deg max
void setDirectionWheels(char sleft, char sright) {
  wheelsMotorSpeedValue[0] = _limitMotorValue(90 + (sleft * WHEEL_SPEED) - wheelsCalibrationData.fl);
  wheelsMotorSpeedValue[1] = _limitMotorValue(90 - (sright * WHEEL_SPEED) - wheelsCalibrationData.fr);
  wheelsMotorSpeedValue[2] = _limitMotorValue(90 + (sleft * WHEEL_SPEED) - wheelsCalibrationData.rl);
  wheelsMotorSpeedValue[3] = _limitMotorValue(90 - (sright * WHEEL_SPEED) - wheelsCalibrationData.rr);
  _doPWMWheels();
}
