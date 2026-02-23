/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
State Manager file
*/

// new an old robot state
unsigned char newRobotState = ROBOT_NORM;
unsigned char oldRobotState = ROBOT_NORM;
unsigned char old2RobotState = ROBOT_NORM;

//------------------robot state manager-----------------------

// set robot state
void _setState(unsigned char newState) {
  switch (newState) {
    case ROBOT_NORM:
    {
      m_robotState.legLiftNow = LEG_LIFT;
      m_robotState.robotStateNow = ROBOT_NORM;
      m_robotState.legHightNow = HIGHT_DEFAULT;
      m_robotState.speedMuliplierNow = 2;
    }
    break;
    case ROBOT_INO:
    {
      m_robotState.legLiftNow = LEG_LIFT * 2;
      m_robotState.robotStateNow = ROBOT_INO;
      m_robotState.legHightNow = HIGHT_DEFAULT;
      m_robotState.speedMuliplierNow = 1;
    }
    break;
    default:
    break;
  }
}

// check for slop
unsigned char _inputsCheck(void) {
  // check current
  if (m_robotState.currentStateNow == C_DEAD_BATTERY) {
    return ROBOT_NORM;
  }
  if (m_robotState.currentStateNow == C_HIGH_CURRENT) {
    return ROBOT_NORM;
  }
   // check gyro high priority state
  switch (m_gyroState.stateGyro) {
    case GYRO_UPSIDEDOWN:
    case GYRO_RESET:
    case GYRO_FELL_LEFT:
    case GYRO_FELL_RIGHT:
    case GYRO_FELL_FRONT:
    case GYRO_FELL_BACK:
      return ROBOT_NORM;
    break;
      default:
    break;
  }
  // check normal priority state
  // folling left
  if (m_gyroState.accRollX < -OFFROAD_ANGLE - 1) {
    return ROBOT_INO;
  }
  // folling right
  if (m_gyroState.accRollX > OFFROAD_ANGLE + 1) {
    return ROBOT_INO;
  }
  // folling front
  if (m_gyroState.accPitchY < -OFFROAD_ANGLE - 1) {
    return ROBOT_INO;
  }
  // folling back
  if (m_gyroState.accPitchY > OFFROAD_ANGLE + 1) {
    return ROBOT_INO;
  }
  return ROBOT_NORM;
}

// do state manager
void setRobotState(void) {
  // check inputs
  newRobotState = _inputsCheck();
  // update state
  old2RobotState = oldRobotState;
  oldRobotState = m_robotState.robotStateNow;
  if (newRobotState != m_robotState.robotStateNow) {
    _setState(newRobotState);
  }
  //_printStateDebug(newRobotState);
}

// print state
void _printStateDebug(unsigned char stateNow) {
  // print state
  switch (stateNow) {
    case ROBOT_NORM:
      Serial.print(F(" ROBOT_NORM "));
    break;
    case ROBOT_INO:
      Serial.print(F(" ROBOT_INO "));
    break;
    default:
      Serial.println(F(" Wrong state "));
  }
}
