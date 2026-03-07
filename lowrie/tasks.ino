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

// set stand walk task
void _setStandWalkTask(void) {
  currentTask[0] = Q_SETPRIORITY_LOW;
  currentTask[1] = P_STANDGO;
  currentTask[2] = Q_DONE;
}

// set task by name
void applyTaskZero(unsigned char task) {
  switch (task) {
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
  m_robotState.patternNow = currentTask[currentTaskPoint];
  m_robotState.taskNow = task;
  //_printTaskNameDebug(task); // DEBUG
}

// set next pattern in task
void setNextPatternInTaskZero(void) {
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

unsigned char getHighPriorityTaskZero(void) {
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
unsigned char getNormalTaskZero(int direction) {
  // slop up or down
  switch (m_gyroState.stateGyro) {
    case GYRO_FELL_FRONT:
      return STANDGO_TASK;
    break;
    case GYRO_FELL_BACK:
      return STANDGO_TASK;
    break;
    default:
    break;
  }
  // check slop
  if (m_gyroState.aPitchAverage < -SLOP_ANGLE) {
    // down
    if (m_gyroState.aRollAverage < -OFFROAD_ANGLE) {
      return STANDGO_TASK;
    }
    if (m_gyroState.aRollAverage > OFFROAD_ANGLE) {
      return STANDGO_TASK;
    }
    if (direction > 4) {
      return STANDGO_TASK;
    }
    if (direction < -4) {
      return STANDGO_TASK;
    }
    return STANDGO_TASK;
  }
  if (m_gyroState.aPitchAverage > SLOP_ANGLE) {
    // up
    if (m_gyroState.aRollAverage < -OFFROAD_ANGLE) {
      return STANDGO_TASK;
    }
    if (m_gyroState.aRollAverage > OFFROAD_ANGLE) {
      return STANDGO_TASK;
    }
    return STANDGO_TASK;
  }
  if (m_gyroState.aRollAverage < -SLOP_ANGLE) {
    // left
    return STANDGO_TASK;
  }
  if (m_gyroState.aRollAverage > SLOP_ANGLE) {
    // left
    return STANDGO_TASK;
  }
  // obstacle state
  switch (m_robotState.inputStateNow) {
    case IN_WALL_FRONTLEFT:
      return STANDGO_TASK;
    break;
    case IN_WALL_FRONTRIGHT:
      return STANDGO_TASK;
    break;
    case IN_WALL_LEFT:
      return STANDGO_TASK;
    break;
    case IN_WALL_RIGHT:
      return STANDGO_TASK;
    break;
    case IN_OBSTACLE_FRONTLEFT:
      return STANDGO_TASK;
    break;
    case IN_OBSTACLE_FRONTRIGHT:
      return STANDGO_TASK;
    break;
    case IN_OBSTACLE_LEFT:
      return STANDGO_TASK;
    break;
    case IN_OBSTACLE_RIGHT:
      return STANDGO_TASK;
    break;
    case IN_NORMAL:
      {
        if (direction > 90) {
          return STANDGO_TASK;
        }
        if (direction < -90) {
          return STANDGO_TASK;
        }
        if (direction > 4) {
          return STANDGO_TASK;
        }
        if (direction < -4) {
          return STANDGO_TASK;
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
  return DEFAULT_TASK;
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
