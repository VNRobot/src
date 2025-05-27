/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

// static ballance
allMotors staticBallance = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// motors attached flag
bool attached = false;
// motors init values
short m1InitValue = 0;
short m2InitValue = 0;
short m1Value = 0;
short m2Value = 0;

// linear motor move
void _moveLinearServo(allMotors calibration, short calM1, short calM2) {
  bool doLoop = true;
  while (doLoop) {
    if (calM1 > m1Value) {
      m1Value ++;
    } else if (calM1 < m1Value) {
      m1Value --;
    }
    if (calM2 > m2Value) {
      m2Value ++;
    } else if (calM2 < m2Value) {
      m2Value --;
    }
    m_servo_fl_1.write(limitMotorValue(90 - 30 + m1Value + calibration.fl.motor1));
    m_servo_fl_2.write(limitMotorValue(90 + 30 - m2Value - calibration.fl.motor2));
    m_servo_fr_1.write(limitMotorValue(90 + 30 - m1Value - calibration.fr.motor1));
    m_servo_fr_2.write(limitMotorValue(90 - 30 + m2Value + calibration.fr.motor2));
    m_servo_rl_1.write(limitMotorValue(90 - 30 + m1Value + calibration.rl.motor1));
    m_servo_rl_2.write(limitMotorValue(90 + 30 - m2Value - calibration.rl.motor2));
    m_servo_rr_1.write(limitMotorValue(90 + 30 - m1Value - calibration.rr.motor1));
    m_servo_rr_2.write(limitMotorValue(90 - 30 + m2Value + calibration.rr.motor2));
    delay(30);
    if ((calM1 == m1Value) && (calM2 == m2Value)) {
      doLoop = false;
    }
  }
}

// limit angle value
short limitMotorValue(short mAngle) {
  if (mAngle > 180) {
    mAngle = 180;
  } else if (mAngle < 0) {
    mAngle = 0;
  }
  return mAngle;
}

// init servo motors
void initServo(allMotors calibration, short calM1, short calM2) {
  // init motors one by one
  m_servo_st_1.attach(ST1_MOTOR, 500, 2500);
  m_servo_st_1.write(90 - calibration.st.motor1);
  delay(100);
  m_servo_st_2.attach(ST2_MOTOR, 500, 2500);
  m_servo_st_2.write(90 - calibration.st.motor2);
  delay(100);
  m_servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  m_servo_fl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.fl.motor1));
  delay(100);
  m_servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  m_servo_fr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.fr.motor1));
  delay(100);
  m_servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  m_servo_rl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.rl.motor1));
  delay(100);
  m_servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  m_servo_rr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.rr.motor1));
  delay(100);
  m_servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  m_servo_fl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.fl.motor2));
  delay(100);
  m_servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  m_servo_fr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.fr.motor2));
  delay(100);
  m_servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  m_servo_rl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.rl.motor2));
  delay(100);
  m_servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  m_servo_rr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.rr.motor2));
  delay(100);
  attached = true;
  m1InitValue = calM1;
  m2InitValue = calM2;
  m1Value = calM1;
  m2Value = calM2;
  }

// attach wings
void detachServo(allMotors calibration) {
  if (attached) {
    _moveLinearServo(calibration, m1InitValue, m2InitValue);
    m_servo_st_1.detach();
    m_servo_st_2.detach();
    m_servo_fl_1.detach();
    m_servo_fr_1.detach();
    m_servo_rl_1.detach();
    m_servo_rr_1.detach();
    m_servo_fl_2.detach();
    m_servo_fr_2.detach();
    m_servo_rl_2.detach();
    m_servo_rr_2.detach();
  }
  attached = false;
  delay(1000);
}

// set servo motors
void setServo(allMotors calibration, short calM1, short calM2) {
  // set motors values after calibration
  m_servo_st_1.write(90 - calibration.st.motor1);
  m_servo_st_2.write(90 - calibration.st.motor2);
  _moveLinearServo(calibration, calM1, calM2);
}

// move motors.
void updateServo(allMotors calibration, allMotors motorValue, allMotors motorLift, motors centerMotors) {
  m_servo_st_1.write(limitMotorValue(90 - (centerMotors.motor1 + calibration.st.motor1)));
  m_servo_st_2.write(limitMotorValue(90 - (centerMotors.motor2 + calibration.st.motor2)));
  m_servo_fl_1.write(limitMotorValue(90 - 30 + (motorValue.fl.motor1 + calibration.fl.motor1 + motorLift.fl.motor1 + staticBallance.fl.motor1)));
  m_servo_fl_2.write(limitMotorValue(90 + 30 - (motorValue.fl.motor2 + calibration.fl.motor2 + motorLift.fl.motor2 + staticBallance.fl.motor2)));
  m_servo_fr_1.write(limitMotorValue(90 + 30 - (motorValue.fr.motor1 + calibration.fr.motor1 + motorLift.fr.motor1 + staticBallance.fr.motor1)));
  m_servo_fr_2.write(limitMotorValue(90 - 30 + (motorValue.fr.motor2 + calibration.fr.motor2 + motorLift.fr.motor2 + staticBallance.fr.motor2)));
  m_servo_rl_1.write(limitMotorValue(90 - 30 + (motorValue.rl.motor1 + calibration.rl.motor1 + motorLift.rl.motor1 + staticBallance.rl.motor1)));
  m_servo_rl_2.write(limitMotorValue(90 + 30 - (motorValue.rl.motor2 + calibration.rl.motor2 + motorLift.rl.motor2 + staticBallance.rl.motor2)));
  m_servo_rr_1.write(limitMotorValue(90 + 30 - (motorValue.rr.motor1 + calibration.rr.motor1 + motorLift.rr.motor1 + staticBallance.rr.motor1)));
  m_servo_rr_2.write(limitMotorValue(90 - 30 + (motorValue.rr.motor2 + calibration.rr.motor2 + motorLift.rr.motor2 + staticBallance.rr.motor2)));
}

void updateStaticBallanceServo(allMotors cBallance) {
  staticBallance.fl.motor1 = cBallance.fl.motor1;
  staticBallance.fl.motor2 = cBallance.fl.motor2;
  staticBallance.fr.motor1 = cBallance.fr.motor1;
  staticBallance.fr.motor2 = cBallance.fr.motor2;
  staticBallance.rl.motor1 = cBallance.rl.motor1;
  staticBallance.rl.motor2 = cBallance.rl.motor2;
  staticBallance.rr.motor1 = cBallance.rr.motor1;
  staticBallance.rr.motor2 = cBallance.rr.motor2;
}
