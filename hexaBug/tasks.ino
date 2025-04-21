/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
List of tasks. Every task contains movement patterns.
*/

// Array to store currently executed task. contains list of patterns
unsigned char currentTask[32] = {P_DOSTAND, P_DONE};
// pattern counter points to currentTask
unsigned char currentTaskPoint = 0;
// current pattern
unsigned char _curPattern = P_DOSTAND;

// set demo task
void _setDemoTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_DOSTAND;
  currentTask[4] = P_DOSTAND;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_DOSTAND;
  currentTask[7] = P_DOSTAND;
  currentTask[8] = P_DOSTAND;
  currentTask[9] = P_DOSTAND;
  currentTask[10] = P_DOSTAND;
  currentTask[11] = P_DOSTAND;
  currentTask[12] = P_DOSTAND;
  currentTask[13] = P_DOSTAND;
  currentTask[14] = P_DOSTAND;
  currentTask[15] = P_DOSTAND;
  currentTask[16] = P_DOSTAND;
  currentTask[17] = P_STANDTOGO;
  currentTask[18] = P_STANDGO;
  currentTask[19] = P_STANDGO;
  currentTask[20] = P_STANDGO;
  currentTask[21] = P_STANDGO;
  currentTask[22] = P_STANDGO;
  currentTask[23] = P_STANDGO;
  currentTask[24] = P_STANDGO;
  currentTask[25] = P_STANDGO;
  currentTask[26] = P_STANDGO;
  currentTask[27] = P_STANDGO;
  currentTask[28] = P_STANDGO;
  currentTask[29] = P_ENABLEINPUTS;
  currentTask[30] = P_GOFORWARDSLOW;
  currentTask[31] = P_DONE;
}

// set begin task
void _setBeginTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_RESETGIRO;
  currentTask[4] = P_STANDTOGO;
  currentTask[5] = P_STANDGO;
  currentTask[6] = P_STANDGO;
  currentTask[7] = P_STANDGO;
  currentTask[8] = P_STANDGO;
  currentTask[9] = P_STANDGO;
  currentTask[10] = P_ENABLEINPUTS;
  currentTask[11] = P_GOFORWARDSLOW;
  currentTask[12] = P_DONE;
}

// set down task
void _setDownTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOTOSTAND;
  currentTask[3] = P_DOSTAND;
  currentTask[4] = P_DODOWN;
  currentTask[5] = P_END;
}

// set walk back left task
void _setWalkBackLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOBACK;
  currentTask[3] = P_GOBACKLEFT;
  currentTask[4] = P_GOBACKLEFT;
  currentTask[5] = P_STANDGO;
  currentTask[6] = P_STANDGOLEFT; 
  currentTask[7] = P_STANDGOLEFT;
  currentTask[8] = P_STANDGOLEFT; 
  currentTask[9] = P_RESETDIRECTION;
  currentTask[10] = P_ENABLEINPUTS;
  currentTask[11] = P_STANDGO;
  currentTask[12] = P_DONE;
}

// set walk back right task
void _setWalkBackRightTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOBACK;
  currentTask[3] = P_GOBACKRIGHT;
  currentTask[4] = P_GOBACKRIGHT;
  currentTask[5] = P_STANDGO;
  currentTask[6] = P_STANDGORIGHT; 
  currentTask[7] = P_STANDGORIGHT;
  currentTask[8] = P_STANDGORIGHT; 
  currentTask[9] = P_RESETDIRECTION;
  currentTask[10] = P_ENABLEINPUTS;
  currentTask[11] = P_STANDGO;
  currentTask[12] = P_DONE;
}

// set Turn right task
void _setWalkTurnRightTask(void) {
  currentTask[0] = P_GORIGHT;
  currentTask[1] = P_RESETDIRECTION;
  currentTask[2] = P_GOFORWARD;
  currentTask[3] = P_GOFORWARD;
  currentTask[4] = P_RESTOREDIRECTION;
  currentTask[5] = P_DONE;
}

// set Turn left task
void _setWalkTurnLeftTask(void) {
  currentTask[0] = P_GOLEFT;
  currentTask[1] = P_RESETDIRECTION;
  currentTask[2] = P_GOFORWARD;
  currentTask[3] = P_GOFORWARD;
  currentTask[4] = P_RESTOREDIRECTION;
  currentTask[5] = P_DONE;
}

// set stand Turn right task
void _setStandTurnRightTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGORIGHT; 
  currentTask[2] = P_STANDGORIGHT;
  currentTask[3] = P_STANDGORIGHT;
  currentTask[4] = P_STANDGORIGHT;
  currentTask[5] = P_RESETDIRECTION;
  currentTask[6] = P_ENABLEINPUTS;
  currentTask[7] = P_STANDGO;
  currentTask[8] = P_DONE;
}

// set stand Turn left task
void _setStandTurnLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGOLEFT; 
  currentTask[2] = P_STANDGOLEFT;
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_STANDGOLEFT;
  currentTask[5] = P_RESETDIRECTION;
  currentTask[6] = P_ENABLEINPUTS;
  currentTask[7] = P_STANDGO;
  currentTask[8] = P_DONE;
}

// set stand Turn right sharp task
void _setStandTurnRight2Task(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGORIGHT; 
  currentTask[2] = P_STANDGORIGHT; 
  currentTask[3] = P_STANDGORIGHT;
  currentTask[4] = P_STANDGORIGHT; 
  currentTask[5] = P_STANDGORIGHT;
  currentTask[6] = P_STANDGORIGHT; 
  currentTask[7] = P_STANDGORIGHT;
  currentTask[8] = P_RESETDIRECTION;
  currentTask[9] = P_ENABLEINPUTS;
  currentTask[10] = P_STANDGO;
  currentTask[11] = P_DONE;
}

// set stand Turn left sharp task
void _setStandTurnLeft2Task(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGOLEFT; 
  currentTask[2] = P_STANDGOLEFT; 
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_STANDGOLEFT; 
  currentTask[5] = P_STANDGOLEFT;
  currentTask[6] = P_STANDGOLEFT; 
  currentTask[7] = P_STANDGOLEFT;
  currentTask[8] = P_RESETDIRECTION;
  currentTask[9] = P_ENABLEINPUTS;
  currentTask[10] = P_STANDGO;
  currentTask[11] = P_DONE;
}

// set walk task
void _setWalkTask(void) {
  currentTask[0] = P_ENABLEINPUTS;
  currentTask[1] = P_GOFORWARD;
  currentTask[2] = P_DONE;
}

// set standwalk task
void _setStandWalkTask(void) {
  currentTask[0] = P_ENABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_DONE;
}

// set stand task
void _setStandTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOTOSTAND;
  currentTask[3] = P_DOSTAND;
  currentTask[4] = P_END;
}

// set bend left task
void _setRecoverLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_RECOVERLEFT;
  currentTask[4] = P_DOSTAND;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_RESETGIRO;
  currentTask[7] = P_STANDTOGO;
  currentTask[8] = P_ENABLEINPUTS;
  currentTask[9] = P_STANDGO;
  currentTask[10] = P_DONE;
}
// set bend right task
void _setRecoverRightTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_RECOVERRIGHT;
  currentTask[4] = P_DOSTAND;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_RESETGIRO;
  currentTask[7] = P_STANDTOGO;
  currentTask[8] = P_ENABLEINPUTS;
  currentTask[9] = P_STANDGO;
  currentTask[10] = P_DONE;
}

// set task by name
void applyTask(unsigned char task) {
  switch (task) {
    case DEMO_TASK:
      _setDemoTask();
    break;
    case BEGIN_TASK:
      _setBeginTask();
    break;
    case DOWN_TASK:
      _setDownTask();
    break;
    case GOBACKLEFT_TASK:
      _setWalkBackLeftTask();
    break;
    case GOBACKRIGHT_TASK:
      _setWalkBackRightTask();
    break;
    case GOTURNRIGHT_TASK:
      _setWalkTurnRightTask();
    break;
    case GOTURNLEFT_TASK:
      _setWalkTurnLeftTask();
    break;
    case STANDTURNRIGHT_TASK:
      _setStandTurnRightTask();
    break;
    case STANDTURNLEFT_TASK:
      _setStandTurnLeftTask();
    break;
    case STANDTURNRIGHT2_TASK:
      _setStandTurnRight2Task();
    break;
    case STANDTURNLEFT2_TASK:
      _setStandTurnLeft2Task();
    break;
    case GO_TASK:
      _setWalkTask();
    break;
    case STANDGO_TASK:
      _setStandWalkTask();
    break;
    case STAND_TASK:
      _setStandTask();
    break;
    case RECOVER_LEFT_TASK:
      _setRecoverLeftTask();
    break;
    case RECOVER_RIGHT_TASK:
      _setRecoverRightTask();
    break;
    default:
    break;
  }
  currentTaskPoint = 0;
}

// get task pointer
unsigned char getPointerInTask(void) {
  return currentTaskPoint;
}

// set task pointer
void setPointerInTask(unsigned char pointer) {
  currentTaskPoint = pointer;
}

// get next pattern in task
unsigned char getNextPatternInTask(void) {
  // update task point to the next pattern
  if (currentTask[currentTaskPoint] != P_DONE) {
    currentTaskPoint ++;
  }
  return currentTask[currentTaskPoint];
}

// get pattern in task
unsigned char getPatternInTask(void) {
  return currentTask[currentTaskPoint];
}

// print task  name
void printTaskname(unsigned char task) {
  switch (task) {
    case DEMO_TASK:
      Serial.println(F(" DEMO_TASK "));
    break;
    case BEGIN_TASK:
      Serial.println(F(" BEGIN_TASK "));
    break;
    case DOWN_TASK:
      Serial.println(F(" DOWN_TASK "));
    break;
    case GOBACKLEFT_TASK:
      Serial.println(F(" GOBACKLEFT_TASK "));
    break;
    case GOBACKRIGHT_TASK:
      Serial.println(F(" GOBACKRIGHT_TASK "));
    break;
    case GOTURNRIGHT_TASK:
      Serial.println(F(" GOTURNRIGHT_TASK "));
    break;
    case GOTURNLEFT_TASK:
      Serial.println(F(" GOTURNLEFT_TASK "));
    break;
    case STANDTURNRIGHT_TASK:
      Serial.println(F(" STANDTURNRIGHT_TASK "));
    break;
    case STANDTURNLEFT_TASK:
      Serial.println(F(" STANDTURNLEFT_TASK "));
    break;
    case STANDTURNRIGHT2_TASK:
      Serial.println(F(" STANDTURNRIGHT2_TASK "));
    break;
    case STANDTURNLEFT2_TASK:
      Serial.println(F(" STANDTURNLEFT2_TASK "));
    break;
    case GO_TASK:
      Serial.println(F(" GO_TASK "));
    break;
    case STANDGO_TASK:
      Serial.println(F(" STANDGO_TASK "));
    break;
    case STAND_TASK:
      Serial.println(F(" STAND_TASK "));
    break;
    case RECOVER_LEFT_TASK:
      Serial.println(F(" RECOVER_LEFT_TASK "));
    break;
    case RECOVER_RIGHT_TASK:
      Serial.println(F(" RECOVER_RIGHT_TASK "));
    break;
    default:
      Serial.println(F(" unknown task "));
    break;
  }
  currentTaskPoint = 0;
}
