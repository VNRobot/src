/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
List of tasks. Every task contains movement patterns.
*/

// pattern repeat counter
#define REPEAT_COUNTER_END      8

// Array to store currently executed task. contains list of patterns
unsigned char currentTask[16] = {Q_DOSTAND, Q_DONE};
// pattern counter points to currentTask
unsigned char currentTaskPoint = 0;
// repeat counter
unsigned char repeatCounter = 0;
// current pattern
unsigned char patternNow = Q_DOSTAND;
// current task
unsigned char taskNow = BEGIN_TASK;

// set down task
void _setDownTask(void) {
  currentTask[0] = Q_SETPRIORITY_HIGH;
  currentTask[1] = Q_DOSTAND;
  currentTask[2] = Q_DODOWN;
  currentTask[3] = Q_END;
}

// set stand task
void _setStandTask(void) {
  currentTask[0] = Q_SETPRIORITY_HIGH;
  currentTask[1] = Q_DOSTAND;
  currentTask[2] = Q_END;
}

// flip
void _setFlipTask(void) {
  currentTask[0] = Q_SETPRIORITY_HIGH;
  currentTask[1] = Q_DOLOW;
  currentTask[2] = Q_DORESET;
  currentTask[3] = Q_DOSTAND;
  currentTask[4] = P_STANDGO;
  currentTask[5] = Q_SETPRIORITY_NORM;
  currentTask[6] = P_STANDGO;
  currentTask[7] = Q_DONE;
}

// recover
void _setRecoverTask(void) {
  currentTask[0] = Q_SETPRIORITY_HIGH;
  currentTask[1] = Q_DOLOW;
  currentTask[2] = Q_DORECOVER;
  currentTask[3] = Q_DOSTAND;
  currentTask[4] = P_STANDGO;
  currentTask[5] = Q_DORESET;
  currentTask[6] = P_STANDGO;
  currentTask[7] = Q_SETPRIORITY_NORM;
  currentTask[8] = P_STANDGO;
  currentTask[9] = Q_DONE;
}
// reset
void _setResetTask(void) {
  currentTask[0] = Q_SETPRIORITY_HIGH;
  currentTask[1] = Q_DOLOW;
  currentTask[2] = Q_DORESET;
  currentTask[3] = Q_DOSTAND;
  currentTask[4] = P_STANDGO;
  currentTask[5] = Q_SETPRIORITY_NORM;
  currentTask[6] = P_STANDGO;
  currentTask[7] = Q_DONE;
}

// set begin task
void _setBeginTask(void) {
  currentTask[0] = Q_SETPRIORITY_HIGH;
  currentTask[1] = Q_DOSTAND;
  currentTask[2] = Q_DOSTAND;
  currentTask[3] = Q_RESETGIRO;
  currentTask[4] = P_STANDGO;
  currentTask[5] = Q_SETPRIORITY_NORM;
  currentTask[6] = Q_DONE;
}

// set stand walk task
void _setStandWalkTask(void) {
  currentTask[0] = Q_SETPRIORITY_LOW;
  currentTask[1] = P_STANDGO;
  currentTask[2] = Q_DONE;
}

// set task by name
void applyTask(unsigned char task) {
  taskNow = task;
  switch (taskNow) {
    case BEGIN_TASK:
      _setBeginTask();
    break;
    case DOWN_TASK:
      _setDownTask();
    break;
    case STANDGO_TASK:
      _setStandWalkTask();
    break;
    case STAND_TASK:
      _setStandTask();
    break;
    case FLIP_TASK:
      _setFlipTask();
    break;
    case RECOVER_TASK:
      _setRecoverTask();
    break;
    case RESET_TASK:
      _setResetTask();
    break;
    default:
    break;
  }
  repeatCounter = 0;
  currentTaskPoint = 0;
  patternNow = currentTask[currentTaskPoint];
  //_printTaskNameDebug(taskNow); // DEBUG
  //_printPatternNameDebug(patternNow); // DEBUG
}

// set next pattern in task
void _setNextPatternInTask(void) {
  // update task point to the next pattern
  if (currentTask[currentTaskPoint] == Q_DONE) {
    // task is done. do nothing
    patternNow = Q_DONE;
    return;
  }
  if ((currentTask[currentTaskPoint] == Q_REPEAT) && (repeatCounter < REPEAT_COUNTER_END)) {
    // update repeat counter
    repeatCounter ++;
    return;
  } 
  // generic pattern
  repeatCounter = 0;
  currentTaskPoint ++;
  if (currentTask[currentTaskPoint] != Q_REPEAT) {
    patternNow = currentTask[currentTaskPoint];
  }
  //_printPatternNameDebug(patternNow); // DEBUG
}

unsigned char _getHighPriorityTask(unsigned char currentState, unsigned char gyroState) {
  if (currentState == C_DEAD_BATTERY) {
    return DOWN_TASK;
  }
  if (currentState == C_HIGH_CURRENT) {
    return STAND_TASK;
  }
  switch (gyroState) {
    case GYRO_UPSIDEDOWN:
      return FLIP_TASK;
    break;
    case GYRO_RESET:
      return RESET_TASK;
    break;
    case GYRO_FELL_LEFT:
      return RECOVER_TASK;
    break;
    case GYRO_FELL_RIGHT:
      return RECOVER_TASK;
    break;
    default:
    break;
  }
  return DEFAULT_TASK;
}

// process sensors return next task name
// could be more complex if remembers previos states
unsigned char _getNormalTask(int direction) {
  return DEFAULT_TASK;
}

// get pattern
unsigned char getPatternOfTask(void) {
  return patternNow;
}

// get task
unsigned char getTask(void) {
  return taskNow;
}

// set new task and new pattern
void setPatternAndTask(unsigned char defaultTask, unsigned char currentState, unsigned char gyroState, unsigned char directionGyro, unsigned char taskPriority) {
  if (patternNow == Q_END) {
    // this is the end. do nothing
    delay(1000);
    return;
  }
  // not high priority or end of high priority task
  if ((taskPriority != PRIORITY_HIGH) || (patternNow == Q_DONE)) {
    // override with high priority task
    unsigned char taskNext = _getHighPriorityTask(currentState, gyroState);
    if (taskNext != DEFAULT_TASK) {
      applyTask(taskNext);
      return;
    }
  }
  // any priority end of task
  if (patternNow == Q_DONE) {
    // check for normal priority
    unsigned char taskNext = _getNormalTask(directionGyro);
    if (taskNext == DEFAULT_TASK) {
      taskNext = defaultTask;
    }
    applyTask(taskNext);
    return;
  }
  // set next pattern
  _setNextPatternInTask();
}

// get walking mode
bool getWalkingModeInTask(void) {
  if (patternNow == P_STANDGO) {
    // walking mode
    return true;
  }
  return false;
}

// print task  name
void _printTaskNameDebug(unsigned char taskName) {
  Serial.println(F(" "));
  switch (taskName) {
    case BEGIN_TASK:
      Serial.println(F(" BEGIN_TASK "));
    break;
    case DOWN_TASK:
      Serial.println(F(" DOWN_TASK "));
    break;
    case STANDGO_TASK:
      Serial.println(F(" STANDGO_TASK "));
    break;
    case STAND_TASK:
      Serial.println(F(" STAND_TASK "));
    break;
    case FLIP_TASK:
      Serial.println(F(" FLIP_TASK "));
    break;
    case RECOVER_TASK:
      Serial.println(F(" RECOVER_TASK "));
    break;
    case DEFAULT_TASK:
      Serial.println(F(" DEFAULT_TASK "));
    break;
    default:
      Serial.print(F(" unknown task "));
      Serial.print((int)taskName);
      Serial.println(F(" "));
    break;
  }
}

// print task  name
void _printPatternNameDebug(unsigned char patternNow) {
  switch (patternNow) {
    case Q_DOSTAND:
      Serial.print(F(" Q_DOSTAND "));
    break;
    case P_STANDGO:
      Serial.print(F(" P_STANDGO "));
    break;
    case Q_DOLOW:
      Serial.print(F(" Q_DOLOW "));
    break;
    case Q_DODOWN:
      Serial.print(F(" Q_DODOWN "));
    break;
    case Q_DORESET:
      Serial.print(F(" Q_DORESET "));
    break;
    case Q_DONE:
      Serial.print(F(" Q_DONE "));
    break;
    case Q_RESETGIRO:
      Serial.print(F(" Q_RESETGIRO "));
    break;
    case Q_REPEAT:
      Serial.print(F(" Q_REPEAT "));
    break;
    case Q_SETPRIORITY_HIGH:
      Serial.print(F(" Q_SETPRIORITY_HIGH "));
    break;
    case Q_SETPRIORITY_NORM:
      Serial.print(F(" Q_SETPRIORITY_NORM "));
    break;
    case Q_SETPRIORITY_LOW:
      Serial.print(F(" Q_SETPRIORITY_LOW "));
    break;
    case Q_END:
      Serial.print(F(" Q_END "));
    break;
    default:
      Serial.print(F(" unknown pattern "));
    break;
  }
}
