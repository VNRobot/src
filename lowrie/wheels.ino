/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nanos
Robot wheels motors motion patterns
*/

// calibration angle
#define WHEELS_CALIBRATION_ANGLE_MIN   -15
#define WHEELS_CALIBRATION_ANGLE_MAX   15

// legs motors structure
struct allWheels {
  char fl;
  char fr;
  char rl;
  char rr;
};

// motors attached flag
bool wheelsAttached = false;
// calibration data
allWheels wheelsCalibrationData = {0, 0, 0, 0};
// servo motor value
short wheelsMotorSpeedValue[4] = {0, 0, 0, 0};
// wheel speed multiplier
char wheelSpeed = 5;

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
    servo_ex_1.write(wheelsMotorSpeedValue[0]);
    servo_ex_2.write(wheelsMotorSpeedValue[1]);
    servo_ex_3.write(wheelsMotorSpeedValue[2]);
    servo_ex_4.write(wheelsMotorSpeedValue[3]);
  }
}

// attach wheels motors
void attachWheels(char wSpeed) {
  wheelSpeed = wSpeed;
  if (!wheelsAttached) {
    // set motors value
    wheelsMotorSpeedValue[0] = _limitWheelSpeedValue(90); 
    wheelsMotorSpeedValue[1] = _limitWheelSpeedValue(90); 
    wheelsMotorSpeedValue[2] = _limitWheelSpeedValue(90); 
    wheelsMotorSpeedValue[3] = _limitWheelSpeedValue(90); 
    // init motors one by one
    servo_ex_1.attach(EX1_MOTOR, 500, 2500);
    servo_ex_1.write(wheelsMotorSpeedValue[0]);
    delay(100);
    servo_ex_2.attach(EX2_MOTOR, 500, 2500);
    servo_ex_2.write(wheelsMotorSpeedValue[1]);
    delay(100);
    servo_ex_3.attach(EX3_MOTOR, 500, 2500);
    servo_ex_3.write(wheelsMotorSpeedValue[2]);
    delay(100);
    servo_ex_4.attach(EX4_MOTOR, 500, 2500);
    servo_ex_4.write(wheelsMotorSpeedValue[3]);
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
            if (wheelsCalibrationData.fl > WHEELS_CALIBRATION_ANGLE_MAX) {
              wheelsCalibrationData.fl = WHEELS_CALIBRATION_ANGLE_MIN;
            }
          }
          break;
          case 2:
          {
            wheelsCalibrationData.fr ++;
            if (wheelsCalibrationData.fr > WHEELS_CALIBRATION_ANGLE_MAX) {
              wheelsCalibrationData.fr = WHEELS_CALIBRATION_ANGLE_MIN;
            }
          }
          break;
          case 3:
          {
            wheelsCalibrationData.rl ++;
            if (wheelsCalibrationData.rl > WHEELS_CALIBRATION_ANGLE_MAX) {
              wheelsCalibrationData.rl = WHEELS_CALIBRATION_ANGLE_MIN;
            }
          }
          break;
          case 4:
          {
            wheelsCalibrationData.rr ++;
            if (wheelsCalibrationData.rr > WHEELS_CALIBRATION_ANGLE_MAX) {
              wheelsCalibrationData.rr = WHEELS_CALIBRATION_ANGLE_MIN;
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
    servo_ex_1.detach();
    servo_ex_2.detach();
    servo_ex_3.detach();
    servo_ex_4.detach();
    wheelsAttached = false;
    delay(100);
  }
}

// set direction. 20 deg max
void setDirectionWheels(char sleft, char sright) {
  if (wheelsAttached) {
    wheelsMotorSpeedValue[0] = _limitMotorValue(90 + (sleft * wheelSpeed) - wheelsCalibrationData.fl);
    wheelsMotorSpeedValue[1] = _limitMotorValue(90 - (sright * wheelSpeed) - wheelsCalibrationData.fr);
    wheelsMotorSpeedValue[2] = _limitMotorValue(90 + (sleft * wheelSpeed) - wheelsCalibrationData.rl);
    wheelsMotorSpeedValue[3] = _limitMotorValue(90 - (sright * wheelSpeed) - wheelsCalibrationData.rr);
    _doPWMWheels();
  }
}
