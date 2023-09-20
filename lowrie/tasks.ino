/*
Wlking Robot Lowrie
Licended GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
List of tasks. Every task contains movement patterns.
*/

// set calibration task
void _setCalibrationTask(void) {
  m_currentTask[0] = DODOWN;
  m_currentTask[1] = DOCALIBRATION;
  m_currentTask[2] = DOCALIBRATION_1;
  m_currentTask[3] = DOCALIBRATION_2;
  m_currentTask[4] = DODOWN;
  m_currentTask[5] = DONE;
}

// set demo task
void _setDemoTask(void) {
  m_currentTask[0] = DOSTAND; 
  m_currentTask[1] = STANDTOWALK;
  m_currentTask[2] = STANDWALK;
  m_currentTask[3] = DOREPEAT; 
  // demo dance not in use for now
  //m_currentTask[0] = DOSTAND; 
  //m_currentTask[1] = DOSTAND;
  //m_currentTask[2] = STANDTOWALK;
  //m_currentTask[3] = STANDWALK; 
  //m_currentTask[4] = STANDWALKLEFT;
  //m_currentTask[5] = STANDWALK; 
  //m_currentTask[6] = STANDWALKRIGHT; 
  //m_currentTask[7] = STANDWALK; 
  //m_currentTask[8] = STANDWALKSHIFTLEFT; 
  //m_currentTask[9] = STANDWALK; 
  //m_currentTask[10] = STANDWALKSHIFTRIGHT; 
  //m_currentTask[11] = STANDWALK;
  //m_currentTask[12] = WALKFORWARD; 
  //m_currentTask[13] = WALKLEFT;
  //m_currentTask[14] = WALKFORWARD; 
  //m_currentTask[15] = WALKRIGHT;
  //m_currentTask[16] = WALKFORWARD; 
  //m_currentTask[17] = WALKSHIFTLEFT;
  //m_currentTask[18] = WALKFORWARD; 
  //m_currentTask[19] = WALKSHIFTRIGHT;
  //m_currentTask[20] = WALKFORWARD; 
  //m_currentTask[21] = STANDWALK;
  //m_currentTask[22] = WALKBACK;
  //m_currentTask[23] = STANDWALK;
  //m_currentTask[24] = WALKTOSTAND;
  //m_currentTask[25] = DOSTAND; 
  //m_currentTask[26] = RESET;
}

// set begin task
void _setBeginTask(void) {
  m_currentTask[0] = DOSTAND;
  m_currentTask[1] = DOSTAND;
  m_currentTask[2] = STANDTOWALK;
  m_currentTask[3] = STANDWALK;
  m_currentTask[4] = STANDWALK;
  m_currentTask[5] = STANDWALK;
  m_currentTask[6] = STANDWALK;
  m_currentTask[7] = STANDWALK;
  m_currentTask[8] = STANDWALK;
  m_currentTask[9] = STANDWALK;
  m_currentTask[10] = DONE;
}

// set down task
void _setDownTask(void) {
  m_currentTask[0] = DODOWN;
  m_currentTask[1] = DOREPEAT;
}

// set walk back task
void _setWalkBackTask(void) {
  m_currentTask[0] = STANDWALK;
  m_currentTask[1] = WALKBACK;
  m_currentTask[2] = WALKBACK;
  m_currentTask[3] = WALKBACK;
  m_currentTask[4] = WALKBACK;
  m_currentTask[5] = STANDWALK;
  m_currentTask[6] = STANDWALKRIGHT; 
  m_currentTask[7] = STANDWALKRIGHT;
  m_currentTask[8] = STANDWALKRIGHT; 
  m_currentTask[9] = STANDWALKRIGHT;
  m_currentTask[10] = STANDWALKRIGHT; 
  m_currentTask[11] = STANDWALKRIGHT;
  m_currentTask[12] = RESETDIRECTION;
  m_currentTask[13] = STANDWALK;
  m_currentTask[14] = DONE;
}

// set turn around task
void _setTurnAroundTask(void) {
  m_currentTask[0] = STANDWALK;
  m_currentTask[1] = STANDWALKLEFT; 
  m_currentTask[2] = STANDWALKLEFT;
  m_currentTask[3] = STANDWALKLEFT; 
  m_currentTask[4] = STANDWALKLEFT;
  m_currentTask[5] = RESETDIRECTION;
  m_currentTask[6] = STANDWALK;
  m_currentTask[7] = DONE;
}

// set shift right task
void _setWalkShiftRightTask(void) {
  m_currentTask[0] = WALKSHIFTRIGHT;
  m_currentTask[1] = DONE;
}

// set shift left task
void _setWalkShiftLeftTask(void) {
  m_currentTask[0] = WALKSHIFTLEFT;
  m_currentTask[1] = DONE;
}

// set Turn right task
void _setWalkTurnRightTask(void) {
  m_currentTask[0] = WALKSHIFTRIGHT;
  m_currentTask[1] = WALKRIGHT;
  m_currentTask[2] = DONE;
}

// set Turn left task
void _setWalkTurnLeftTask(void) {
  m_currentTask[0] = WALKSHIFTLEFT;
  m_currentTask[1] = WALKLEFT;
  m_currentTask[2] = DONE;
}

// set stand Turn right task
void _setStandTurnRightTask(void) {
  m_currentTask[0] = STANDWALKSHIFTRIGHT; 
  m_currentTask[1] = STANDWALKRIGHT;
  m_currentTask[2] = DONE;
}

// set stand Turn left task
void _setStandTurnLeftTask(void) {
  m_currentTask[0] = STANDWALKSHIFTLEFT; 
  m_currentTask[1] = STANDWALKLEFT;
  m_currentTask[2] = DONE;
}

// set stand Turn right sharp task
void _setStandTurnRight2Task(void) {
  m_currentTask[0] = STANDWALKSHIFTRIGHT; 
  m_currentTask[1] = STANDWALKRIGHT; 
  m_currentTask[2] = STANDWALKRIGHT;
  m_currentTask[3] = DONE;
}

// set stand Turn left sharp task
void _setStandTurnLeft2Task(void) {
  m_currentTask[0] = STANDWALKSHIFTLEFT; 
  m_currentTask[1] = STANDWALKLEFT; 
  m_currentTask[2] = STANDWALKLEFT;
  m_currentTask[3] = DONE;
}

// set walk task
void _setWalkTask(void) {
  m_currentTask[0] = WALKFORWARD;
  m_currentTask[1] = DONE;
}

// set standwalk task
void _setStandWalkTask(void) {
  m_currentTask[0] = STANDWALK;
  m_currentTask[1] = DONE;
}

// set stand task
void _setStandTask(void) {
  m_currentTask[0] = DOSTAND;
  m_currentTask[1] = DOREPEAT;
}

// set task by name
void applyTask(unsigned char task) {
  switch (task) {
    case CALIBRATIONTASK:
      // set calibration task
      _setCalibrationTask();
    break;
    case DEMOTASK:
      // set demo task
      _setDemoTask();
    break;
    case BEGINTASK:
      // set explore task
      _setBeginTask();
    break;
    case DOWNTASK:
      // set down task
      _setDownTask();
    break;
    case WALKBACKTASK:
      // set walk back task
      _setWalkBackTask();
    break;
    case TURNAROUNDTASK:
      // set turn around task
      _setTurnAroundTask();
    break;
    case WALKSHIFTRIGHTTASK:
      // set shift right task
      _setWalkShiftRightTask();
    break;
    case WALKSHIFTLEFTTASK:
      // set shift left task
      _setWalkShiftLeftTask();
    break;
    case WALKTURNRIGHTTASK:
      // set Turn right task
      _setWalkTurnRightTask();
    break;
    case WALKTURNLEFTTASK:
      // set Turn left task
      _setWalkTurnLeftTask();
    break;
    case STANDTURNRIGHTTASK:
      // set stand Turn right task
      _setStandTurnRightTask();
    break;
    case STANDTURNLEFTTASK:
      // set stand Turn left task
      _setStandTurnLeftTask();
    break;
    case STANDTURNRIGHT2TASK:
      // set stand Turn right sharp task
      _setStandTurnRight2Task();
    break;
    case STANDTURNLEFT2TASK:
      // set stand Turn left sharp task
      _setStandTurnLeft2Task();
    break;
    case WALKTASK:
      // set walk task
      _setWalkTask();
    break;
    case STANDWALKTASK:
      // set standwalk task
      _setStandWalkTask();
    break;
    case STANDTASK:
      // set stand task
      _setStandTask();
    break;
    default:
    break;
  }
}
