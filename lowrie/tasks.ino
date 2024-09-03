/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
List of tasks. Every task contains movement patterns.
*/

// Array to store currently executed task. contains list of patterns
unsigned char _currentTask[28] = {DOCALIBRATION, DOREPEAT};
// pattern counter points to _currentTask
unsigned char _currentTaskPoint = 0;
// current pattern
unsigned char _curPattern = DOCALIBRATION;

// set calibration task
void _setCalibrationTask(void) {
  _currentTask[0] = DODOWN;
  _currentTask[1] = DOCALIBRATION;
  _currentTask[2] = DOCALIBRATION_1;
  _currentTask[3] = DOCALIBRATION_2;
  _currentTask[4] = DODOWN;
  _currentTask[5] = DONE;
}

// set demo task
void _setDemoTask(void) {
  _currentTask[0] = DOSTAND; 
  _currentTask[1] = STANDTOWALK;
  _currentTask[2] = STANDWALK;
  _currentTask[3] = DOREPEAT; 
  // demo dance not in use for now
  //_currentTask[0] = DOSTAND; 
  //_currentTask[1] = DOSTAND;
  //_currentTask[2] = STANDTOWALK;
  //_currentTask[3] = STANDWALK; 
  //_currentTask[4] = STANDWALKLEFT;
  //_currentTask[5] = STANDWALK; 
  //_currentTask[6] = STANDWALKRIGHT; 
  //_currentTask[7] = STANDWALK; 
  //_currentTask[8] = STANDWALKSHIFTLEFT; 
  //_currentTask[9] = STANDWALK; 
  //_currentTask[10] = STANDWALKSHIFTRIGHT; 
  //_currentTask[11] = STANDWALK;
  //_currentTask[12] = WALKFORWARD; 
  //_currentTask[13] = WALKLEFT;
  //_currentTask[14] = WALKFORWARD; 
  //_currentTask[15] = WALKRIGHT;
  //_currentTask[16] = WALKFORWARD; 
  //_currentTask[17] = WALKSHIFTLEFT;
  //_currentTask[18] = WALKFORWARD; 
  //_currentTask[19] = WALKSHIFTRIGHT;
  //_currentTask[20] = WALKFORWARD; 
  //_currentTask[21] = STANDWALK;
  //_currentTask[22] = WALKBACK;
  //_currentTask[23] = STANDWALK;
  //_currentTask[24] = WALKTOSTAND;
  //_currentTask[25] = DOSTAND; 
  //_currentTask[26] = RESET;
}

// set begin task
void _setBeginTask(void) {
  _currentTask[0] = DOSTAND;
  _currentTask[1] = DOSTAND;
  _currentTask[2] = STANDTOWALK;
  _currentTask[3] = STANDWALK;
  _currentTask[4] = STANDWALK;
  _currentTask[5] = STANDWALK;
  _currentTask[6] = STANDWALK;
  _currentTask[7] = STANDWALK;
  _currentTask[8] = STANDWALK;
  _currentTask[9] = STANDWALK;
  _currentTask[10] = DONE;
}

// set down task
void _setDownTask(void) {
  _currentTask[0] = DODOWN;
  _currentTask[1] = DOREPEAT;
}

// set walk back task
void _setWalkBackTask(void) {
  _currentTask[0] = STANDWALK;
  _currentTask[1] = WALKBACK;
  _currentTask[2] = WALKBACK;
  _currentTask[3] = WALKBACK;
  _currentTask[4] = WALKBACK;
  _currentTask[5] = STANDWALK;
  _currentTask[6] = STANDWALKRIGHT; 
  _currentTask[7] = STANDWALKRIGHT;
  _currentTask[8] = STANDWALKRIGHT; 
  _currentTask[9] = STANDWALKRIGHT;
  _currentTask[10] = STANDWALKRIGHT; 
  _currentTask[11] = STANDWALKRIGHT;
  _currentTask[12] = RESETDIRECTION;
  _currentTask[13] = STANDWALK;
  _currentTask[14] = DONE;
}

// set turn around task
void _setTurnAroundTask(void) {
  _currentTask[0] = STANDWALK;
  _currentTask[1] = STANDWALKLEFT; 
  _currentTask[2] = STANDWALKLEFT;
  _currentTask[3] = STANDWALKLEFT; 
  _currentTask[4] = STANDWALKLEFT;
  _currentTask[5] = RESETDIRECTION;
  _currentTask[6] = STANDWALK;
  _currentTask[7] = DONE;
}

// set shift right task
void _setWalkShiftRightTask(void) {
  _currentTask[0] = WALKSHIFTRIGHT;
  _currentTask[1] = DONE;
}

// set shift left task
void _setWalkShiftLeftTask(void) {
  _currentTask[0] = WALKSHIFTLEFT;
  _currentTask[1] = DONE;
}

// set Turn right task
void _setWalkTurnRightTask(void) {
  _currentTask[0] = WALKSHIFTRIGHT;
  _currentTask[1] = WALKRIGHT;
  _currentTask[2] = DONE;
}

// set Turn left task
void _setWalkTurnLeftTask(void) {
  _currentTask[0] = WALKSHIFTLEFT;
  _currentTask[1] = WALKLEFT;
  _currentTask[2] = DONE;
}

// set stand Turn right task
void _setStandTurnRightTask(void) {
  _currentTask[0] = STANDWALKSHIFTRIGHT; 
  _currentTask[1] = STANDWALKRIGHT;
  _currentTask[2] = DONE;
}

// set stand Turn left task
void _setStandTurnLeftTask(void) {
  _currentTask[0] = STANDWALKSHIFTLEFT; 
  _currentTask[1] = STANDWALKLEFT;
  _currentTask[2] = DONE;
}

// set stand Turn right sharp task
void _setStandTurnRight2Task(void) {
  _currentTask[0] = STANDWALKSHIFTRIGHT; 
  _currentTask[1] = STANDWALKRIGHT; 
  _currentTask[2] = STANDWALKRIGHT;
  _currentTask[3] = DONE;
}

// set stand Turn left sharp task
void _setStandTurnLeft2Task(void) {
  _currentTask[0] = STANDWALKSHIFTLEFT; 
  _currentTask[1] = STANDWALKLEFT; 
  _currentTask[2] = STANDWALKLEFT;
  _currentTask[3] = DONE;
}

// set walk task
void _setWalkTask(void) {
  _currentTask[0] = WALKFORWARD;
  _currentTask[1] = DONE;
}

// set standwalk task
void _setStandWalkTask(void) {
  _currentTask[0] = STANDWALK;
  _currentTask[1] = DONE;
}

// set stand task
void _setStandTask(void) {
  _currentTask[0] = DOSTAND;
  _currentTask[1] = DOREPEAT;
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

// get task pointer
unsigned char getPointerInTask(void) {
  return _currentTaskPoint;
}

// set task pointer
void setPointerInTask(unsigned char pointer) {
  _currentTaskPoint = pointer;
}

// get current pattern in task
unsigned char getPatternInTask(void) {
  // process cycling commands
  switch (_currentTask[_currentTaskPoint]) {
    case RESET:
    {
      _currentTaskPoint = 0;
    }
    break;
    case DOREPEAT:
    {
      if (_currentTaskPoint > 0) {
        _currentTaskPoint --;
      }
    }
    break;
    default:
    break;
  }
  _curPattern = _currentTask[_currentTaskPoint];
  // update task point for the next pattern
  switch (_curPattern) {
    case DONE:
      _currentTaskPoint = 0;
    break;
    case DOCALIBRATION:
    case DOCALIBRATION_1:
    case DOCALIBRATION_2:
      // do nothing
    break;
    default:
      _currentTaskPoint ++;
    break;
  }
  return _curPattern;
}
