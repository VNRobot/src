/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
List of tasks. Every task contains movement patterns.
*/

// Array to store currently executed task. contains list of patterns
unsigned char currentTask[32] = {P_DOSTAND, P_DONE};
// pattern counter points to currentTask
unsigned char currentTaskPoint = 0;
// pattern in task buffer
unsigned char patternInTaskBuffer = P_DOSTAND;
// repeat counter
unsigned char repeatCounter = 0;
unsigned char repeatCounterEnd = 8;

// set begin task
void _setBeginTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_RESETGIRO;
  currentTask[4] = P_NORMALSTART;
  currentTask[5] = P_STANDTOGO;
  currentTask[6] = P_STANDGO;
  currentTask[7] = P_STANDGO;
  currentTask[8] = P_GOFORWARD;
  currentTask[9] = P_DONE;
}

// set down task
void _setDownTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_GOTOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_DOSTAND;
  currentTask[4] = P_DODOWN;
  currentTask[5] = P_END;
}

// set walk back left task
void _setWalkBackLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOBACKLEFT;
  currentTask[3] = P_GOBACKLEFT;
  currentTask[4] = P_RESETDIRECTION;
  currentTask[5] = P_ENABLEINPUTS;
  currentTask[6] = P_DONE;
}

// set walk back right task
void _setWalkBackRightTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOBACKRIGHT;
  currentTask[3] = P_GOBACKRIGHT;
  currentTask[4] = P_RESETDIRECTION;
  currentTask[5] = P_ENABLEINPUTS;
  currentTask[6] = P_DONE;
}

// set Turn right task
void _setWalkTurnRightTask(void) {
  currentTask[0] = P_GORIGHT;
  currentTask[1] = P_GORIGHT;
  currentTask[2] = P_RESETDIRECTION;
  currentTask[3] = P_DONE;
}

// set Turn left task
void _setWalkTurnLeftTask(void) {
  currentTask[0] = P_GOLEFT;
  currentTask[1] = P_GOLEFT;
  currentTask[2] = P_RESETDIRECTION;
  currentTask[3] = P_DONE;
}

// set stand Turn right task
void _setStandTurnRightTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGORIGHT;
  currentTask[2] = P_STANDGORIGHT;
  currentTask[3] = P_RESETDIRECTION;
  currentTask[4] = P_ENABLEINPUTS;
  currentTask[5] = P_DONE;
}

// set stand Turn left task
void _setStandTurnLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGOLEFT;
  currentTask[2] = P_STANDGOLEFT;
  currentTask[3] = P_RESETDIRECTION;
  currentTask[4] = P_ENABLEINPUTS;
  currentTask[5] = P_DONE;
}

// set stand Turn right sharp task
void _setStandTurnRight2Task(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGORIGHT; 
  currentTask[2] = P_STANDGORIGHT; 
  currentTask[3] = P_STANDGORIGHT;
  currentTask[4] = P_STANDGORIGHT; 
  currentTask[5] = P_RESETDIRECTION;
  currentTask[6] = P_ENABLEINPUTS;
  currentTask[7] = P_DONE;
}

// set stand Turn left sharp task
void _setStandTurnLeft2Task(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_STANDGOLEFT; 
  currentTask[2] = P_STANDGOLEFT; 
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_STANDGOLEFT; 
  currentTask[5] = P_RESETDIRECTION;
  currentTask[6] = P_ENABLEINPUTS;
  currentTask[7] = P_DONE;
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
  currentTask[1] = P_GOTOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_END;
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

void _setFlipTask(void) {
  currentTask[0] = P_DISABLEINPUTS;
  currentTask[1] = P_GOTOSTAND;
  currentTask[2] = P_DOLOW;
  currentTask[3] = P_DOFLIP;
  currentTask[4] = P_DOSTAND;
  currentTask[5] = P_RESETGIRO;
  currentTask[6] = P_STANDTOGO;
  currentTask[7] = P_ENABLEINPUTS;
  currentTask[8] = P_STANDGO;
  currentTask[9] = P_DONE;

}

// set task by name
void applyTask(unsigned char task) {
  switch (task) {
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
    case FLIP_TASK:
      _setFlipTask();
    break;
    default:
    break;
  }
  repeatCounter = 0;
  currentTaskPoint = 0;
  patternInTaskBuffer = currentTask[currentTaskPoint];
}

// get next pattern in task
unsigned char getNextPatternInTask(void) {
  // update task point to the next pattern
  if (currentTask[currentTaskPoint] == P_DONE) {
    // task is done. do nothing
    patternInTaskBuffer = P_DONE;
    return patternInTaskBuffer;
  }
  if ((currentTask[currentTaskPoint] == P_REPEAT) && (repeatCounter < repeatCounterEnd)) {
    // update repeat counter
    repeatCounter ++;
    return patternInTaskBuffer;
  } 
  // generic pattern
  repeatCounter = 0;
  currentTaskPoint ++;
  if (currentTask[currentTaskPoint] != P_REPEAT) {
    patternInTaskBuffer = currentTask[currentTaskPoint];
  }
  return patternInTaskBuffer;
}

// get pattern in task
unsigned char getPatternInTask(void) {
  return patternInTaskBuffer;
}
/*
// print task  name
void printTaskname(unsigned char task) {
  switch (task) {
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
*/