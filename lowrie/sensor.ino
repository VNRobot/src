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
// motors values

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
  // process angle
  // m_sensorValue.motor1
  // m_sensorValue.motor2
  // set motor angle
  m_sensorMotorAngleValue[0] = _limitSensorMotorValue(90 - m_sensorCalibrationData.motor1 - m_sensorValue.motor1);
  m_sensorMotorAngleValue[1] = _limitSensorMotorValue(90 - m_sensorCalibrationData.motor2 - m_sensorValue.motor2);
  // move motors
  doPWMSensor();
}
