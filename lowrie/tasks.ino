/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
List of tasks. Every task contains movement patterns.
*/

// Array to store currently executed task. contains list of patterns
unsigned char currentTask[16] = {Q_DOSTAND, Q_DONE};
// pattern counter points to currentTask
unsigned char currentTaskPoint = 0;
// repeat counter
unsigned char repeatCounter = 0;
// -----------------tasks manager----------------------
// remember tasks
unsigned char tasksMemory[16] = {DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, 
                                 DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, 
                                 DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, 
                                 DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK};
// counter for tasks
unsigned char tasksCounter[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// common task
unsigned char commonTask = DEFAULT_TASK;
unsigned char commonTaskCount = 0;
// suggested task
unsigned char suggestedTask = DEFAULT_TASK;

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

// set walk back left task
void _setWalkBackLeftTask(void) {
  currentTask[0] = Q_SETPRIORITY_NORM;
  currentTask[1] = P_GOBACK;
  currentTask[2] = P_GOBACKLEFT;
  currentTask[3] = P_GOBACKLEFT;
  currentTask[4] = P_GOBACKLEFT;
  currentTask[5] = P_GOBACKLEFT;
  currentTask[6] = P_GOBACKLEFT;
  currentTask[7] = Q_RESETDIRECTION;
  currentTask[8] = Q_DONE;
}

// set walk back right task
void _setWalkBackRightTask(void) {
  currentTask[0] = Q_SETPRIORITY_NORM;
  currentTask[1] = P_GOBACK;
  currentTask[2] = P_GOBACKRIGHT;
  currentTask[3] = P_GOBACKRIGHT;
  currentTask[4] = P_GOBACKRIGHT;
  currentTask[5] = P_GOBACKRIGHT;
  currentTask[6] = P_GOBACKRIGHT;
  currentTask[7] = Q_RESETDIRECTION;
  currentTask[8] = Q_DONE;
}

// set stand Turn right task
void _setStandTurnRightTask(void) {
  currentTask[0] = Q_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGORIGHT;
  currentTask[2] = P_STANDGORIGHT;
  currentTask[3] = Q_RESETDIRECTION;
  currentTask[4] = Q_DONE;
}

// set stand Turn left task
void _setStandTurnLeftTask(void) {
  currentTask[0] = Q_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGOLEFT;
  currentTask[2] = P_STANDGOLEFT;
  currentTask[3] = Q_RESETDIRECTION;
  currentTask[4] = Q_DONE;
}

// set stand Turn right sharp task
void _setStandTurnRight2Task(void) {
  currentTask[0] = Q_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGORIGHT; 
  currentTask[2] = P_STANDGORIGHT;
  currentTask[3] = P_STANDGORIGHT; 
  currentTask[4] = Q_RESETDIRECTION;
  currentTask[5] = Q_DONE;
}

// set stand Turn left sharp task
void _setStandTurnLeft2Task(void) {
  currentTask[0] = Q_SETPRIORITY_NORM;
  currentTask[1] = P_STANDGOLEFT; 
  currentTask[2] = P_STANDGOLEFT;
  currentTask[3] = P_STANDGOLEFT; 
  currentTask[4] = Q_RESETDIRECTION;
  currentTask[5] = Q_DONE;
}

// set Turn right task
void _setWalkTurnRightTask(void) {
  currentTask[0] = Q_SETPRIORITY_LOW;
  currentTask[1] = P_GORIGHT;
  currentTask[2] = Q_DONE;
}

// set Turn left task
void _setWalkTurnLeftTask(void) {
  currentTask[0] = Q_SETPRIORITY_LOW;
  currentTask[1] = P_GOLEFT;
  currentTask[2] = Q_DONE;
}

// set walk task
void _setWalkTask(void) {
  currentTask[0] = Q_SETPRIORITY_LOW;
  currentTask[1] = P_GOFORWARD;
  currentTask[2] = Q_DONE;
}

// set walk back task
void _setWalkBackTask(void) {
  currentTask[0] = Q_SETPRIORITY_LOW;
  currentTask[1] = P_GOBACK;
  currentTask[2] = Q_REVERSEDIRECTION;
  currentTask[3] = Q_DONE;
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
    case GOBACK_TASK:
      _setWalkBackTask();
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
  m_robotState.patternNow = currentTask[currentTaskPoint];
  m_robotState.taskNow = task;
  //_printTaskNameDebug(task); // DEBUG
}

// set next pattern in task
void setNextPatternInTask(void) {
  // update task point to the next pattern
  if (currentTask[currentTaskPoint] == Q_DONE) {
    // task is done. do nothing
    m_robotState.patternNow = Q_DONE;
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
    m_robotState.patternNow = currentTask[currentTaskPoint];
  }
  return;
}

unsigned char getHighPriorityTask(void) {
  if (m_robotState.currentStateNow == C_DEAD_BATTERY) {
    return DOWN_TASK;
  }
  if (m_robotState.currentStateNow == C_HIGH_CURRENT) {
    return STAND_TASK;
  }
  switch (m_gyroState.stateGyro) {
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
unsigned char getNormalTask(int direction) {
  // slop up or down
  switch (m_gyroState.stateGyro) {
    case GYRO_FELL_FRONT:
      return GO_TASK;
    break;
    case GYRO_FELL_BACK:
      return GOBACK_TASK;
    break;
    default:
    break;
  }
  // check slop
  if (m_gyroState.aPitchAverage < -SLOP_ANGLE) {
    // down
    if (m_gyroState.aRollAverage < -OFFROAD_ANGLE) {
      return GOTURNLEFT_TASK;
    }
    if (m_gyroState.aRollAverage > OFFROAD_ANGLE) {
      return GOTURNRIGHT_TASK;
    }
    if (direction > 4) {
      return GOTURNLEFT_TASK;
    }
    if (direction < -4) {
      return GOTURNRIGHT_TASK;
    }
    return GO_TASK;
  }
  if (m_gyroState.aPitchAverage > SLOP_ANGLE) {
    // up
    if (m_gyroState.aRollAverage < -OFFROAD_ANGLE) {
      return GOBACKRIGHT_TASK;
    }
    if (m_gyroState.aRollAverage > OFFROAD_ANGLE) {
      return GOBACKLEFT_TASK;
    }
    return GOBACK_TASK;
  }
  if (m_gyroState.aRollAverage < -SLOP_ANGLE) {
    // left
    return GOTURNLEFT_TASK;
  }
  if (m_gyroState.aRollAverage > SLOP_ANGLE) {
    // left
    return GOTURNRIGHT_TASK;
  }
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
      {
        if (direction > 90) {
          return GOBACKLEFT_TASK;
        }
        if (direction < -90) {
          return GOBACKRIGHT_TASK;
        }
        if (direction > 4) {
          return GOTURNLEFT_TASK;
        }
        if (direction < -4) {
          return GOTURNRIGHT_TASK;
        }
        if (m_robotState.currentStateNow == C_LOW_BATTERY) {
          return DEFAULT_TASK;
        }
        return DEFAULT_TASK;
      }
    break;
    default:
      return DEFAULT_TASK;
    break;
  }
  return DEFAULT_TASK;
}

// -----------------tasks manager----------------------

// remember item
void _remeberItem(unsigned char item, unsigned char * itemArray) {
  // shift values
  for (i = 15; i > 0; i --) {
    itemArray[i] = itemArray[i - 1];
  }
  itemArray[0] = item;
}

// replace item
void _replaceItem(unsigned char item, unsigned char * itemArray) {
  itemArray[0] = item;
}

// count items
void _countItems(unsigned char * countArray, unsigned char * itemArray) {
  // reset counts
  for (i = 0; i < 17; i ++) {
    countArray[i] = 0;
  }
  // add counts
  for (i = 0; i < 16; i ++) {
    countArray[itemArray[i]] ++;
  }
}

// get common item
void _getCommonTask(unsigned char * countArray) {
  commonTask = DEFAULT_TASK;
  commonTaskCount = 0;
  for (i = 0; i < 17; i ++) {
    if (countArray[i] > commonTaskCount) {
      commonTaskCount = countArray[i];
      commonTask = i;
    }
  }
  // for now repeat same task 4 times
  if (commonTaskCount < 4) {
    commonTask = DEFAULT_TASK;
  }
}

// get opposite task
unsigned char getOppositeTask(void) {
  switch (commonTask) {
    case GOBACKLEFT_TASK:
      return STANDTURNRIGHT2_TASK;
    break;
    case GOBACKRIGHT_TASK:
      return STANDTURNLEFT2_TASK;
    break;
    case STANDTURNRIGHT_TASK:
      return STANDTURNLEFT2_TASK;
    break;
    case STANDTURNLEFT_TASK:
      return STANDTURNRIGHT2_TASK;
    break;
    case STANDTURNRIGHT2_TASK:
      return GOBACKLEFT_TASK;
    break;
    case STANDTURNLEFT2_TASK:
      return GOBACKRIGHT_TASK;
    break;
    case FLIP_TASK:
      return RECOVER_TASK;
    break;
    case RECOVER_TASK:
      return FLIP_TASK;
    break;
    case BEGIN_TASK:
      return DOWN_TASK;
    break;
    case GOBACK_TASK:
      return GO_TASK;
    break;
    case DOWN_TASK:
    case STAND_TASK:
    case GO_TASK:
    case GOTURNRIGHT_TASK:
    case GOTURNLEFT_TASK:
    case DEFAULT_TASK:
      return DEFAULT_TASK;
    break;
    default:
      return DEFAULT_TASK;
    break;
  }
}

// call manager
unsigned char doManageTasks(unsigned char taskNext) {
  // add task to array
  _remeberItem(taskNext, tasksMemory);
  // count items
  _countItems(tasksCounter, tasksMemory);
  // find common task item
  _getCommonTask(tasksCounter);
  // opposite suggested task
  suggestedTask = getOppositeTask();
  if (suggestedTask != DEFAULT_TASK) {
    taskNext = suggestedTask;
    _replaceItem(taskNext, tasksMemory);
  }
  return taskNext;
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
