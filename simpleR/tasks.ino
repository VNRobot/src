/*
Simple Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
List of tasks. Every task contains movement patterns.
*/

// Array to store currently executed task. contains list of patterns
unsigned char currentTask[8] = {P_DOSTAND, P_DONE};
// pattern counter points to currentTask
unsigned char currentTaskPoint = 0;

// set begin task
void _setBeginTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_STANDTOGO;
  currentTask[3] = P_STANDGO;
  currentTask[4] = P_ENABLEINPUTS;
  currentTask[5] = P_DONE;
}

// set walk back task
void _setWalkBackLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOBACKLEFT;
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_RESETDIRECTION;
  currentTask[5] = P_ENABLEINPUTS;
  currentTask[6] = P_DONE;
}

// set walk back task
void _setWalkBackRightTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOBACKRIGHT;
  currentTask[3] = P_STANDGORIGHT;
  currentTask[4] = P_RESETDIRECTION;
  currentTask[5] = P_ENABLEINPUTS;
  currentTask[6] = P_DONE;
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
  currentTask[2] = P_RESETDIRECTION;
  currentTask[3] = P_ENABLEINPUTS;
  currentTask[4] = P_DONE;
}

// set stand Turn left task
void _setStandTurnLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGOLEFT;
  currentTask[2] = P_RESETDIRECTION;
  currentTask[3] = P_ENABLEINPUTS;
  currentTask[4] = P_DONE;
}

// set stand Turn right sharp task
void _setStandTurnRight2Task(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGORIGHT; 
  currentTask[2] = P_STANDGORIGHT;
  currentTask[3] = P_RESETDIRECTION;
  currentTask[4] = P_ENABLEINPUTS;
  currentTask[5] = P_DONE;
}

// set stand Turn left sharp task
void _setStandTurnLeft2Task(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGOLEFT; 
  currentTask[2] = P_STANDGOLEFT;
  currentTask[3] = P_RESETDIRECTION;
  currentTask[4] = P_ENABLEINPUTS;
  currentTask[5] = P_DONE;
}

// set walk task
void _setWalkTask(void) {
  currentTask[0] = P_GOFORWARD;
  currentTask[1] = P_DONE;
}

// set stand walk task
void _setStandWalkTask(void) {
  currentTask[0] = P_STANDGO;
  currentTask[1] = P_DONE;
}

// set stand task
void _setStandTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_GOTOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_ENABLEINPUTS;
  currentTask[4] = P_DONE;
}

// set task by name
void applyTask(unsigned char task) {
  switch (task) {
    case BEGIN_TASK:
      _setBeginTask();
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
    default:
    break;
  }
  currentTaskPoint = 0;
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
    case BEGIN_TASK:
      Serial.println(" BEGIN_TASK ");
    break;
    case GOBACKLEFT_TASK:
      Serial.println(" GOBACKLEFT_TASK ");
    break;
    case GOBACKRIGHT_TASK:
      Serial.println(" GOBACKRIGHT_TASK ");
    break;
    case GOTURNRIGHT_TASK:
      Serial.println(" GOTURNRIGHT_TASK ");
    break;
    case GOTURNLEFT_TASK:
      Serial.println(" GOTURNLEFT_TASK ");
    break;
    case STANDTURNRIGHT_TASK:
      Serial.println(" STANDTURNRIGHT_TASK ");
    break;
    case STANDTURNLEFT_TASK:
      Serial.println(" STANDTURNLEFT_TASK ");
    break;
    case STANDTURNRIGHT2_TASK:
      Serial.println(" STANDTURNRIGHT2_TASK ");
    break;
    case STANDTURNLEFT2_TASK:
      Serial.println(" STANDTURNLEFT2_TASK ");
    break;
    case GO_TASK:
      Serial.println(" GO_TASK ");
    break;
    case STANDGO_TASK:
      Serial.println(" STANDGO_TASK ");
    break;
    case STAND_TASK:
      Serial.println(" STAND_TASK ");
    break;
    default:
      Serial.println(" unknown task ");
    break;
  }
  currentTaskPoint = 0;
}
