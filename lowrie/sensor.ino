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
Servo m_servo_st_1;
Servo m_servo_st_2;

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
void initSensor(void) {
  // set motors value
  m_sensorMotorAngleValue[0] = _limitSensorMotorValue(90 - m_sensorCalibrationData.motor1);
  m_sensorMotorAngleValue[1] = _limitSensorMotorValue(90 - m_sensorCalibrationData.motor2);
  // init motors one by one
  m_servo_st_1.attach(ST1_MOTOR, 500, 2500);
  doPWMSensor();
  delay(100);
  m_servo_st_2.attach(ST2_MOTOR, 500, 2500);
  doPWMSensor();
  delay(100);
  sensorAttached = true;
  doPWMSensor();
}

// detach hardware
void detachSensorZero(void) {
  if (sensorAttached) {
    m_servo_st_1.detach();
    m_servo_st_2.detach();
  }
  sensorAttached = false;
  doPWMSensor();
  delay(100);
}

// do servo pwm cycle 500, 2500
void doPWMSensor(void) {
  m_servo_st_1.write(m_sensorMotorAngleValue[0]);
  m_servo_st_2.write(m_sensorMotorAngleValue[1]);
}

// set servo motors
void setSensorZero(char angle) {
  // set motor angle
  m_sensorMotorAngleValue[0] = _limitSensorMotorValue(90 - m_sensorCalibrationData.motor1 - angle);
  m_sensorMotorAngleValue[1] = _limitSensorMotorValue(90 - m_sensorCalibrationData.motor2 - angle);
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
  m_sensorValue.motor1 = headAngle * 10;
  m_sensorValue.motor2 = headAngle * 10;
  // set motor angle
  m_sensorMotorAngleValue[0] = _limitSensorMotorValue(90 - m_sensorCalibrationData.motor1 - m_sensorValue.motor1);
  m_sensorMotorAngleValue[1] = _limitSensorMotorValue(90 - m_sensorCalibrationData.motor2 - m_sensorValue.motor2);
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
