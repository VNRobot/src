/*
Walking Robot TurtleV1
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

// set down task
void _setDownTask(void) {
  currentTask[0] = P_SETPRIORITY_HIGH;
  currentTask[1] = P_GOTOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_DODOWN;
  currentTask[4] = P_END;
}

// set stand task
void _setStandTask(void) {
  currentTask[0] = P_SETPRIORITY_HIGH;
  currentTask[1] = P_GOTOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_END;
}

// set bend left task
void _setRecoverLeftTask(void) {
  currentTask[0] = P_SETPRIORITY_HIGH;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_RECOVERLEFT;
  currentTask[3] = P_DOSTAND;
  currentTask[4] = P_RESETGIRO;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_SETPRIORITY_NORM;
  currentTask[7] = P_DOSTAND;
  currentTask[8] = P_GETCURRENT;
  currentTask[9] = P_STANDTOGO;
  currentTask[10] = P_STANDGO;
  currentTask[11] = P_DONE;
}
// set bend right task
void _setRecoverRightTask(void) {
  currentTask[0] = P_SETPRIORITY_HIGH;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_RECOVERRIGHT;
  currentTask[3] = P_DOSTAND;
  currentTask[4] = P_RESETGIRO;
  currentTask[5] = P_DOSTAND;
  currentTask[6] = P_SETPRIORITY_NORM;
  currentTask[7] = P_DOSTAND;
  currentTask[8] = P_GETCURRENT;
  currentTask[9] = P_STANDTOGO;
  currentTask[10] = P_STANDGO;
  currentTask[11] = P_DONE;
}

void _setFlipTask(void) {
  currentTask[0] = P_SETPRIORITY_HIGH;
  currentTask[1] = P_GOTOSTAND;
  currentTask[2] = P_DOLOW;
  currentTask[3] = P_DOFLIP;
  currentTask[4] = P_DOSTAND;
  currentTask[5] = P_RESETGIRO;
  currentTask[6] = P_DOSTAND;
  currentTask[7] = P_SETPRIORITY_NORM;
  currentTask[8] = P_DOSTAND;
  currentTask[9] = P_GETCURRENT;
  currentTask[10] = P_STANDTOGO;
  currentTask[11] = P_STANDGO;
  currentTask[12] = P_DONE;
}

// set begin task
void _setBeginTask(void) {
  currentTask[0] = P_SETPRIORITY_NORM;
  currentTask[1] = P_DOSTAND;
  currentTask[2] = P_DOSTAND;
  currentTask[3] = P_GETCURRENT;
  currentTask[4] = P_RESETGIRO;
  currentTask[5] = P_NORMALSTART;
  currentTask[6] = P_STANDTOGO;
  currentTask[7] = P_STANDGO;
  currentTask[8] = P_STANDGO;
  currentTask[9] = P_GOFORWARD;
  currentTask[10] = P_DONE;
}

// set walk back left task
void _setWalkBackLeftTask(void) {
  currentTask[0] = P_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOBACK;
  currentTask[3] = P_GOBACKLEFT;
  currentTask[4] = P_GOBACKLEFT;
  currentTask[5] = P_GOBACKLEFT;
  currentTask[6] = P_GOBACKLEFT;
  currentTask[7] = P_GOBACKLEFT;
  currentTask[8] = P_STANDGO;
  currentTask[9] = P_RESETDIRECTION;
  currentTask[10] = P_DONE;
}

// set walk back right task
void _setWalkBackRightTask(void) {
  currentTask[0] = P_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_GOBACK;
  currentTask[3] = P_GOBACKRIGHT;
  currentTask[4] = P_GOBACKRIGHT;
  currentTask[5] = P_GOBACKRIGHT;
  currentTask[6] = P_GOBACKRIGHT;
  currentTask[7] = P_GOBACKRIGHT;
  currentTask[8] = P_STANDGO;
  currentTask[9] = P_RESETDIRECTION;
  currentTask[10] = P_DONE;
}

// set stand Turn right task
void _setStandTurnRightTask(void) {
  currentTask[0] = P_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGORIGHT;
  currentTask[2] = P_STANDGORIGHT;
  currentTask[3] = P_STANDGORIGHT;
  currentTask[4] = P_STANDGO;
  currentTask[5] = P_RESETDIRECTION;
  currentTask[6] = P_DONE;
}

// set stand Turn left task
void _setStandTurnLeftTask(void) {
  currentTask[0] = P_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGOLEFT;
  currentTask[2] = P_STANDGOLEFT;
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_STANDGO;
  currentTask[5] = P_RESETDIRECTION;
  currentTask[6] = P_DONE;
}

// set stand Turn right sharp task
void _setStandTurnRight2Task(void) {
  currentTask[0] = P_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGORIGHT; 
  currentTask[2] = P_STANDGORIGHT; 
  currentTask[3] = P_STANDGORIGHT;
  currentTask[4] = P_STANDGORIGHT; 
  currentTask[5] = P_STANDGO;
  currentTask[6] = P_RESETDIRECTION;
  currentTask[7] = P_DONE;
}

// set stand Turn left sharp task
void _setStandTurnLeft2Task(void) {
  currentTask[0] = P_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGOLEFT; 
  currentTask[2] = P_STANDGOLEFT; 
  currentTask[3] = P_STANDGOLEFT;
  currentTask[4] = P_STANDGOLEFT; 
  currentTask[5] = P_STANDGO;
  currentTask[6] = P_RESETDIRECTION;
  currentTask[7] = P_DONE;
}

// set Turn right task
void _setWalkTurnRightTask(void) {
  currentTask[0] = P_SETPRIORITY_LOW;
  currentTask[1] = P_GORIGHT;
  currentTask[2] = P_GORIGHT;
  currentTask[3] = P_RESETDIRECTION;
  currentTask[4] = P_DONE;
}

// set Turn left task
void _setWalkTurnLeftTask(void) {
  currentTask[0] = P_SETPRIORITY_LOW;
  currentTask[1] = P_GOLEFT;
  currentTask[2] = P_GOLEFT;
  currentTask[3] = P_RESETDIRECTION;
  currentTask[4] = P_DONE;
}

// set walk task
void _setWalkTask(void) {
  currentTask[0] = P_SETPRIORITY_LOW;
  currentTask[1] = P_GOFORWARD;
  currentTask[2] = P_DONE;
}

// set standwalk task
void _setStandWalkTask(void) {
  currentTask[0] = P_SETPRIORITY_LOW;
  currentTask[1] = P_STANDGO;
  currentTask[2] = P_DONE;
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

unsigned char getHighPriorityTask(void) {
  if (m_robotState.currentStateNow == C_LOW_BATTERY) {
    return DOWN_TASK;
  }
  if (m_robotState.currentStateNow == C_HIGH_CURRENT) {
    return STAND_TASK;
  }
  if (m_robotState.currentStateNow == C_LOW_CURRENT) {
    return FLIP_TASK;
  }
  switch (m_gyroState.stateGyro) {
    case GYRO_UPSIDEDOWN:
      return FLIP_TASK;
    break;
    case GYRO_FELL_LEFT:
      return RECOVER_LEFT_TASK;
    break;
    case GYRO_FELL_RIGHT:
      return RECOVER_RIGHT_TASK;
    break;
    case GYRO_FELL_FRONT:
      return FLIP_TASK;
    break;
    case GYRO_FELL_BACK:
      return FLIP_TASK;
    break;
    case GYRO_FOLLING_LEFT:
      return RECOVER_LEFT_TASK;
    break;
    case GYRO_FOLLING_RIGHT:
      return RECOVER_RIGHT_TASK;
    break;
    case GYRO_FOLLING_FRONT:
      return DEFAULT_TASK;
    break;
    case GYRO_FOLLING_BACK:
      return DEFAULT_TASK;
    break;
    case GYRO_SHAKEN:
      return DEFAULT_TASK;
    break;
    default:
    break;
  }
  return DEFAULT_TASK;
}

// process sensors return next task name
// could be more complex if remembers previos states
unsigned char getNormalTask(void) {
  // obstacle state
  switch (m_robotState.inputStateNow) {
    case IN_WALL_FRONTLEFT:
      return GOBACKRIGHT_TASK;
    break;
    case IN_WALL_FRONTRIGHT:
      return GOBACKLEFT_TASK;
    break;
    case IN_WALL_LEFT:
      return STANDTURNRIGHT2_TASK;
    break;
    case IN_WALL_RIGHT:
      return STANDTURNLEFT2_TASK;
    break;
    case IN_OBSTACLE_FRONTLEFT:
      return STANDTURNRIGHT_TASK;
    break;
    case IN_OBSTACLE_FRONTRIGHT:
      return STANDTURNLEFT_TASK;
    break;
    case IN_OBSTACLE_LEFT:
      return GOTURNRIGHT_TASK;
    break;
    case IN_OBSTACLE_RIGHT:
      return GOTURNLEFT_TASK;
    break;
    case IN_NORMAL:
      return DEFAULT_TASK;
    break;
    default:
      return DEFAULT_TASK;
    break;
  }
  return DEFAULT_TASK;
}
