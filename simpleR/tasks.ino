/*
Ladybug Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
List of tasks. Every task contains movement patterns.
*/

// Array to store currently executed task. contains list of patterns
unsigned char currentTask[24] = {P_DOSTAND, P_DONE};
// pattern counter points to currentTask
unsigned char currentTaskPoint = 0;
// current pattern
unsigned char _curPattern = P_DOSTAND;

// set demo task
void _setDemoTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_RESETDIRECTION;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_STANDGORIGHT;
  currentTask[7] = P_RESETDIRECTION;
  currentTask[8] = P_DOSTAND;
  currentTask[9] = P_GOFORWARD;
  currentTask[10] = P_GOFORWARD;
  currentTask[11] = P_GOLEFT;
  currentTask[12] = P_RESETDIRECTION;
  currentTask[13] = P_GOFORWARD;
  currentTask[14] = P_GORIGHT;
  currentTask[15] = P_RESETDIRECTION;
  currentTask[16] = P_GOFORWARD;
  currentTask[17] = P_DOSTAND;
  currentTask[18] = P_GOBACK;
  currentTask[19] = P_GOBACKLEFT;
  currentTask[20] = P_GOBACKRIGHT;
  currentTask[21] = P_RESTOREDIRECTION;
  currentTask[22] = P_DOSTAND;
  currentTask[23] = P_END;
}

// set begin task
void _setBeginTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_RESETGIRO;
  currentTask[3] = P_DOSTAND;
  currentTask[4] = P_ATTACH;
  currentTask[5] = P_RECOVERRIGHT;
  currentTask[6] = P_RECOVERLEFT;
  currentTask[7] = P_RECOVER;
  currentTask[8] = P_DETACH;
  currentTask[9] = P_ENABLEINPUTLEFT;
  currentTask[10] = P_ENABLEINPUTRIGHT;
  currentTask[11] = P_DOSTAND;
  currentTask[12] = P_DONE;
}

// set down task
void _setDownTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_END;
}

// set walk back left task
void _setWalkBackLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_GOBACKLEFT;
  currentTask[3] = P_GOBACKLEFT;
  currentTask[4] = P_ENABLEINPUTRIGHT;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_DISABLEINPUTRIGHT;
  currentTask[7] = P_STANDGOLEFT; 
  currentTask[8] = P_STANDGOLEFT;
  currentTask[9] = P_ENABLEINPUTLEFT;
  currentTask[10] = P_DOSTAND;
  currentTask[11] = P_DISABLEINPUTLEFT;
  currentTask[12] = P_RESETDIRECTION;
  currentTask[13] = P_DONE;
}

// set walk back right task
void _setWalkBackRightTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_GOBACKRIGHT;
  currentTask[3] = P_GOBACKRIGHT;
  currentTask[4] = P_ENABLEINPUTLEFT;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_DISABLEINPUTLEFT;
  currentTask[7] = P_STANDGORIGHT; 
  currentTask[8] = P_STANDGORIGHT;
  currentTask[9] = P_ENABLEINPUTRIGHT;
  currentTask[10] = P_DOSTAND;
  currentTask[11] = P_DISABLEINPUTRIGHT;
  currentTask[12] = P_RESETDIRECTION;
  currentTask[13] = P_DONE;
}

// set Turn right task
void _setWalkTurnRightTask(void) {
  currentTask[0] = P_ENABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_GORIGHT;
  currentTask[3] = P_GORIGHT;
  currentTask[4] = P_RESETDIRECTION;
  currentTask[5] = P_DISABLEINPUTLEFT;
  currentTask[6] = P_ENABLEINPUTRIGHT;
  currentTask[7] = P_GOFORWARD;
  currentTask[8] = P_GOFORWARD;
  currentTask[9] = P_DISABLEINPUTRIGHT;
  currentTask[10] = P_RESTOREDIRECTION;
  currentTask[11] = P_DONE;
}

// set Turn left task
void _setWalkTurnLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_ENABLEINPUTRIGHT;
  currentTask[2] = P_GOLEFT;
  currentTask[3] = P_GOLEFT;
  currentTask[4] = P_RESETDIRECTION;
  currentTask[5] = P_ENABLEINPUTLEFT;
  currentTask[6] = P_DISABLEINPUTRIGHT;
  currentTask[7] = P_GOFORWARD;
  currentTask[8] = P_GOFORWARD;
  currentTask[9] = P_DISABLEINPUTLEFT;
  currentTask[10] = P_RESTOREDIRECTION;
  currentTask[11] = P_DONE;
}

// set stand Turn right task
void _setStandTurnRightTask(void) {
  currentTask[0] = P_ENABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_STANDGORIGHT;
  currentTask[3] = P_STANDGORIGHT;
  currentTask[4] = P_DISABLEINPUTLEFT;
  currentTask[5] = P_ENABLEINPUTRIGHT;
  currentTask[6] = P_DOSTAND;
  currentTask[7] = P_DISABLEINPUTRIGHT;
  currentTask[8] = P_RESETDIRECTION;
  currentTask[9] = P_DONE;
}

// set stand Turn left task
void _setStandTurnLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_ENABLEINPUTRIGHT;
  currentTask[2] = P_STANDGOLEFT;
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_ENABLEINPUTLEFT;
  currentTask[5] = P_DISABLEINPUTRIGHT;
  currentTask[6] = P_DOSTAND;
  currentTask[7] = P_DISABLEINPUTLEFT;
  currentTask[8] = P_RESETDIRECTION;
  currentTask[9] = P_DONE;
}

// set stand Turn right sharp task
void _setStandTurnRight2Task(void) {
  currentTask[0] = P_ENABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_STANDGORIGHT;
  currentTask[3] = P_STANDGORIGHT;
  currentTask[4] = P_STANDGORIGHT;
  currentTask[5] = P_STANDGORIGHT;
  currentTask[6] = P_DISABLEINPUTLEFT;
  currentTask[7] = P_ENABLEINPUTRIGHT;
  currentTask[8] = P_DOSTAND;
  currentTask[9] = P_DISABLEINPUTRIGHT;
  currentTask[10] = P_RESETDIRECTION;
  currentTask[11] = P_DONE;
}

// set stand Turn left sharp task
void _setStandTurnLeft2Task(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_ENABLEINPUTRIGHT;
  currentTask[2] = P_STANDGOLEFT;
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_STANDGOLEFT;
  currentTask[5] = P_STANDGOLEFT;
  currentTask[6] = P_ENABLEINPUTLEFT;
  currentTask[7] = P_DISABLEINPUTRIGHT;
  currentTask[8] = P_DOSTAND;
  currentTask[9] = P_DISABLEINPUTLEFT;
  currentTask[10] = P_RESETDIRECTION;
  currentTask[11] = P_DONE;
}

// set walk task
void _setWalkTask(void) {
  currentTask[0] = P_ENABLEINPUTLEFT;
  currentTask[1] = P_ENABLEINPUTRIGHT;
  currentTask[2] = P_GOFORWARD;
  currentTask[3] = P_DONE;
}

// set stand task
void _setStandTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_END;
}

// set bend left task
void _setBendLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_ATTACH;
  currentTask[3] = P_RECOVERLEFT;
  currentTask[4] = P_RECOVER;
  currentTask[5] = P_DETACH;
  currentTask[6] = P_DOSTAND;
  currentTask[7] = P_RESETGIRO;
  currentTask[8] = P_ENABLEINPUTLEFT;
  currentTask[9] = P_ENABLEINPUTRIGHT;
  currentTask[10] = P_DONE;
}

// set bend right task
void _setBendRightTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_ATTACH;
  currentTask[3] = P_RECOVERRIGHT;
  currentTask[4] = P_RECOVER;
  currentTask[5] = P_DETACH;
  currentTask[6] = P_DOSTAND;
  currentTask[7] = P_RESETGIRO;
  currentTask[8] = P_ENABLEINPUTLEFT;
  currentTask[9] = P_ENABLEINPUTRIGHT;
  currentTask[10] = P_DONE;
}

// set bend front task
void _setBendFrontTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_ATTACH;
  currentTask[3] = P_RECOVER;
  currentTask[4] = P_DETACH;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_RESETGIRO;
  currentTask[7] = P_ENABLEINPUTLEFT;
  currentTask[8] = P_ENABLEINPUTRIGHT;
  currentTask[9] = P_DONE;
}

// set bend rear task
void _setBendRearTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_ATTACH;
  currentTask[3] = P_RECOVER;
  currentTask[4] = P_DETACH;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_RESETGIRO;
  currentTask[7] = P_ENABLEINPUTLEFT;
  currentTask[8] = P_ENABLEINPUTRIGHT;
  currentTask[9] = P_DONE;
}

// set bend left task
void _setRecoverLeftTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_ATTACH;
  currentTask[3] = P_RECOVERLEFT;
  currentTask[4] = P_RECOVERLEFT;
  currentTask[5] = P_RECOVER;
  currentTask[6] = P_DETACH;
  currentTask[7] = P_DOSTAND;
  currentTask[8] = P_RESETGIRO;
  currentTask[9] = P_ENABLEINPUTLEFT;
  currentTask[10] = P_ENABLEINPUTRIGHT;
  currentTask[11] = P_DONE;
}
// set bend right task
void _setRecoverRightTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_ATTACH;
  currentTask[3] = P_RECOVERRIGHT;
  currentTask[4] = P_RECOVERRIGHT;
  currentTask[5] = P_RECOVER;
  currentTask[6] = P_DETACH;
  currentTask[7] = P_DOSTAND;
  currentTask[8] = P_RESETGIRO;
  currentTask[9] = P_ENABLEINPUTLEFT;
  currentTask[10] = P_ENABLEINPUTRIGHT;
  currentTask[11] = P_DONE;
}
// set turn task
void _setRecoverTask(void) {
  currentTask[0] = P_DISABLEINPUTLEFT;
  currentTask[1] = P_DISABLEINPUTRIGHT;
  currentTask[2] = P_ATTACH;
  currentTask[3] = P_RECOVERLEFT;
  currentTask[4] = P_RECOVERRIGHT;
  currentTask[5] = P_RECOVER;
  currentTask[6] = P_DETACH;
  currentTask[7] = P_DOSTAND;
  currentTask[8] = P_RESETGIRO;
  currentTask[9] = P_ENABLEINPUTLEFT;
  currentTask[10] = P_ENABLEINPUTRIGHT;
  currentTask[11] = P_DONE;
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
    case STAND_TASK:
      _setStandTask();
    break;
    case BEND_LEFT_TASK:
      _setBendLeftTask();
    break;
    case BEND_RIGHT_TASK:
      _setBendRightTask();
    break;
    case BEND_FRONT_TASK:
      _setBendFrontTask();
    break;
    case BEND_REAR_TASK:
      _setBendRearTask();
    break;
    case RECOVER_LEFT_TASK:
      _setRecoverLeftTask();
    break;
    case RECOVER_RIGHT_TASK:
      _setRecoverRightTask();
    break;
    case RECOVER_TASK:
      _setRecoverTask();
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
    case STAND_TASK:
      Serial.println(F(" STAND_TASK "));
    break;
    case BEND_LEFT_TASK:
      Serial.println(F(" BEND_LEFT_TASK "));
    break;
    case BEND_RIGHT_TASK:
      Serial.println(F(" BEND_RIGHT_TASK "));
    break;
    case BEND_FRONT_TASK:
      Serial.println(F(" BEND_FRONT_TASK "));
    break;
    case BEND_REAR_TASK:
      Serial.println(F(" BEND_REAR_TASK "));
    break;
    case RECOVER_TASK:
      Serial.println(F(" RECOVER_TASK "));
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
