/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot sensors motors motion
*/

// pin numbers for servo motors
enum sPinsServo {
  ST1_MOTOR = 2,
  ST2_MOTOR = 3
};

// init servo library
Servo servo_st_1;
Servo servo_st_2;

// motors attached flag
bool sensorAttached = false;
// turn direction
bool turnRightDirection = false;
// head angle
char headAngle = 0;
char headAngleOld = 0;
char headAngleCounter = 0;
// array pointer
unsigned char sensorArrayPointer = 0;
// motor angle value
short sensorMotorAngleValue[2] = {0, 0};
// calibration value
motors sensorCalibrationData = {0, 0};
// sensor angle positive - right
motors sensorValue = {0, 0};

/*
uses
m_sequenceCounter
m_inputDistanceFU
m_inputDistanceFL
m_inputDistanceRU
m_inputDistanceRL
m_getButtonPressed()
*/

// limit angle value
short _limitSensorMotorValue(short mAngle) {
  if (mAngle > 130) {
    mAngle = 130;
  } else if (mAngle < 50) {
    mAngle = 50;
  }
  return mAngle;
}

// init servo motors
void initSensor(bool calibrationMode) {
  // set motors value
  sensorMotorAngleValue[0] = _limitSensorMotorValue(90);
  sensorMotorAngleValue[1] = _limitSensorMotorValue(90);
  // init motors one by one
  servo_st_1.attach(ST1_MOTOR, 500, 2500);
  servo_st_1.write(sensorMotorAngleValue[0]);
  delay(100);
  servo_st_2.attach(ST2_MOTOR, 500, 2500);
  servo_st_2.write(sensorMotorAngleValue[1]);
  delay(100);
  sensorAttached = true;
  // check for calibration mode
  if (calibrationMode) {
    // do calibration
    unsigned char calibrationStage = 0;
    sensorCalibrationData.motor1 = 0;
    sensorCalibrationData.motor2 = 0;
    // motors one by one
    while (calibrationMode) {
      sensorMotorAngleValue[0] = _limitMotorValue(90 - sensorCalibrationData.motor1);
      sensorMotorAngleValue[1] = _limitMotorValue(90 - sensorCalibrationData.motor2);
      doPWMSensor();
      delay(200);
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
          sensorCalibrationData.motor1 ++;
          if (sensorCalibrationData.motor1 > CALIBRATION_ANGLE_MAX) {
            sensorCalibrationData.motor1 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 2:
        {
          sensorCalibrationData.motor2 ++;
          if (sensorCalibrationData.motor2 > CALIBRATION_ANGLE_MAX) {
            sensorCalibrationData.motor2 = CALIBRATION_ANGLE_MIN;
          }
        }
        break;
        case 3:
        {
          EEPROM.update(11, (unsigned char)(sensorCalibrationData.motor1 + 128));
          EEPROM.update(12, (unsigned char)(sensorCalibrationData.motor2 + 128));
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
  sensorCalibrationData.motor1 = (char)(EEPROM.read(11) - 128);
  sensorCalibrationData.motor2 = (char)(EEPROM.read(12) - 128);
  sensorMotorAngleValue[0] = _limitMotorValue(90 - sensorCalibrationData.motor1);
  sensorMotorAngleValue[1] = _limitMotorValue(90 - sensorCalibrationData.motor2);
  doPWMSensor();
  delay(100);
}

// detach hardware
void detachSensorZero(void) {
  if (sensorAttached) {
    servo_st_1.detach();
    servo_st_2.detach();
  }
  sensorAttached = false;
  doPWMSensor();
  delay(100);
}

// do servo pwm cycle 500, 2500
void doPWMSensor(void) {
  servo_st_1.write(sensorMotorAngleValue[0]);
  servo_st_2.write(sensorMotorAngleValue[1]);
}

// set servo motors
void setSensorZero(char angle) {
  // set motor angle
  sensorMotorAngleValue[0] = _limitSensorMotorValue(90 - sensorCalibrationData.motor1 - angle);
  sensorMotorAngleValue[1] = _limitSensorMotorValue(90 - sensorCalibrationData.motor2 - angle);
  // move motors
  doPWMSensor();
}

// move leg motors.
void updateSensor(void) {
  if (m_sequenceCounter.m == 0) {
    // _printSensorArrays();
    turnRightDirection = !turnRightDirection;
  }
  // process angle m_sequenceCounter.m MAIN_FULL_CYCLE
  headAngle = (m_sequenceCounter.m - MAIN_HALF_CYCLE) / 6;
  if (!turnRightDirection) {
    headAngle = -headAngle;
  }
  // find timing
  if (headAngleOld == headAngle) {
    headAngleCounter ++;
  } else {
    headAngleCounter = 0;
    headAngleOld = headAngle;
  }
  // time to get inputs
  if (headAngleCounter == 4) {
    sensorArrayPointer = headAngle + 3;
    if (turnRightDirection) {
      sensorArrayPointer --;
    }
    // read analog sensors. norm: 36 - 42 average 38      max - 60
    m_inputDistanceFU[sensorArrayPointer] = _getDistance(analogRead(A0));
    m_inputDistanceFL[sensorArrayPointer] = _getDistance(analogRead(A1));
    m_inputDistanceRU[sensorArrayPointer] = _getDistance(analogRead(A3));
    m_inputDistanceRL[sensorArrayPointer] = _getDistance(analogRead(A2));
  }
  // set angle value
  sensorValue.motor1 = headAngle * 10;
  sensorValue.motor2 = headAngle * 10;
  // set motor angle
  sensorMotorAngleValue[0] = _limitSensorMotorValue(90 - sensorCalibrationData.motor1 - sensorValue.motor1);
  sensorMotorAngleValue[1] = _limitSensorMotorValue(90 - sensorCalibrationData.motor2 - sensorValue.motor2);
  // move motors
  doPWMSensor();
}

// calculate sensor distance 
/*
Gets analog inputs
 - Reads analog sensors raw values and converts to cm
    cm     value    700-     devider
    5      640      60        12
    10     420      280       28
    15     320      380       25
    20     260      440       22
    25     250      450       18
    60     100      600       10
*/
unsigned short _getDistance(int sensorRead) {
  if (sensorRead > 700) {
    sensorRead == 700;
  }
  return (unsigned short)((700 - sensorRead) / 10);
}

// print arrays
void _printSensorArrays(void) {
  Serial.println(F(" FU "));
  _printSensorArray(m_inputDistanceFU);
  Serial.println(F(" FL "));
  _printSensorArray(m_inputDistanceFL);
  Serial.println(F(" RU "));
  _printSensorArray(m_inputDistanceRU);
  Serial.println(F(" RL "));
  _printSensorArray(m_inputDistanceRL);
}

// print one array
void _printSensorArray(unsigned short * distanceArray) {
  for (i = 0; i < 6; i ++) {
    Serial.print(F(" "));
    Serial.print((int)distanceArray[i]);
  }
  Serial.println(F(" "));
}
