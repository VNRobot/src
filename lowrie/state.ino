/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
State Manager file
*/
/*
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
    case ROBOT_CRAWL:
    {
      m_robotState.legLiftNow = LEG_LIFT * 2;
      m_robotState.robotStateNow = ROBOT_CRAWL;
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
    case ROBOT_CRAWL:
      Serial.print(F(" ROBOT_CRAWL "));
    break;
    default:
      Serial.println(F(" Wrong state "));
  }
}
*/