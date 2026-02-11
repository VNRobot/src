/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Manager file
*/

/*
input data
  current
    m_robotState.currentStateNow
      C_LOW_BATTERY,
      C_DEAD_BATTERY,
      C_HIGH_CURRENT,
      C_NORMAL  
  gyro
    m_gyroState.stateGyro          
      GYRO_NORM,
      GYRO_UPSIDEDOWN,
      GYRO_FELL_LEFT,
      GYRO_FELL_RIGHT,
      GYRO_FELL_FRONT,
      GYRO_FELL_BACK,
      GYRO_FOLLING_LEFT,
      GYRO_FOLLING_RIGHT,
      GYRO_FOLLING_FRONT,
      GYRO_FOLLING_BACK,
      GYRO_SHAKEN,
      GYRO_TURNED_RIGHT,
      GYRO_TURNED_LEFT
  inputs
    m_robotState.inputStateNow
      IN_WALL_FRONTLEFT,
      IN_WALL_FRONTRIGHT,
      IN_WALL_LEFT,
      IN_WALL_RIGHT,
      IN_OBSTACLE_FRONTLEFT,
      IN_OBSTACLE_FRONTRIGHT,
      IN_OBSTACLE_LEFT,
      IN_OBSTACLE_RIGHT,
      IN_NORMAL             
  tasks
    m_robotState.taskNow
      BEGIN_TASK,
      GOBACKLEFT_TASK,
      GOBACKRIGHT_TASK,
      GOTURNRIGHT_TASK,
      GOTURNLEFT_TASK,
      STANDTURNRIGHT_TASK,
      STANDTURNLEFT_TASK,
      STANDTURNRIGHT2_TASK,
      STANDTURNLEFT2_TASK,
      GO_TASK,
      STAND_TASK,
      DOWN_TASK,
      RECOVER_LEFT_TASK,
      RECOVER_RIGHT_TASK,
      FLIP_TASK,
      DEFAULT_TASK
    m_robotState.taskPriorityNow
      PRIORITY_HIGH,
      PRIORITY_NORM,
      PRIORITY_LOW,
    m_robotState.patternNow
      P_DOSTAND,
      P_STANDTOGO,
      P_GOTOSTAND,
      P_STANDGO,
      P_STANDGOLEFT,
      P_STANDGORIGHT,
      P_GOFORWARD,
      P_GOLEFT,
      P_GORIGHT,
      P_GOBACK,
      P_GOBACKLEFT,
      P_GOBACKRIGHT,
      P_DOLOW,
      P_DODOWN,
      P_RECOVERLEFT,
      P_RECOVERRIGHT,
      P_DOFLIP,
      P_DONE,
      P_SETDIRECTION,
      P_RESETDIRECTION,
      P_RESTOREDIRECTION,
      P_RESETGIRO,
      P_REPEAT,
      P_SETPRIORITY_HIGH,
      P_SETPRIORITY_NORM,
      P_SETPRIORITY_LOW,
      P_END
*/

/*
output state
  m_robotState.robotStateNow
    ROBOT_NORM,
    ROBOT_RUN,
    ROBOT_ROLL,
    ROBOT_CRAWL,
    ROBOT_SWIM,
    ROBOT_INO,
    ROBOT_BEND
*/

// remember tasks
unsigned char tasksMemory[16] = {DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, 
                                 DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, 
                                 DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, 
                                 DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK, DEFAULT_TASK};
// counter for tasks
unsigned char tasksCounter[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// common task
unsigned char commonTask = DEFAULT_TASK;
unsigned char commonTaskCount = 0;
// suggested task
unsigned char suggestedTask = DEFAULT_TASK;
// new an old robot state
unsigned char newRobotState = ROBOT_NORM;
unsigned char oldRobotState = ROBOT_NORM;
unsigned char old2RobotState = ROBOT_NORM;

//------------------robot task manager-----------------------

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
  for (i = 0; i < 16; i ++) {
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
  for (i = 0; i < 16; i ++) {
    if (countArray[i] > commonTaskCount) {
      commonTaskCount = countArray[i];
      commonTask = i;
    }
  }
  // for now repeat same task 6 times
  if (commonTaskCount < 6) {
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
    case RECOVER_LEFT_TASK:
    case RECOVER_RIGHT_TASK:
      return FLIP_TASK;
    break;
    case FLIP_TASK:
      return BEGIN_TASK;
    break;
    case BEGIN_TASK:
      return DOWN_TASK;
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
unsigned char doTaskManager(unsigned char taskNext) {
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

//------------------robot state manager-----------------------

// set robot state
void _setRobotState(unsigned char newState) {
  switch (newState) {
    case ROBOT_CRAWL:
    {
      m_robotState.legLiftNow = LEG_LIFT * 3;
      m_robotState.inputDistanceNow = NORMAL_DISTANCE;
      m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
      m_robotState.robotStateNow = ROBOT_CRAWL;
      m_robotState.legHightNow = HIGHT_DEFAULT - 20;
      m_robotState.shiftCycleNow = 0;
      m_robotState.speedMuliplierNow = 2;
      m_robotState.surfaceAngleDevider = 0;
      m_robotState.timeDelayNow = TIME_DELAY;
      m_robotState.edgeEnabled = false;
      m_robotState.bendHight = 0;
      m_robotState.bendShift = 0;
      m_robotState.bendAngle = 0;

    }
    break;
    case ROBOT_SWIM:
    {
      m_robotState.legLiftNow = LEG_LIFT * 3;
      m_robotState.inputDistanceNow = NORMAL_DISTANCE / 2;
      m_robotState.halfCycleNow = 0;
      m_robotState.robotStateNow = ROBOT_SWIM;
      m_robotState.legHightNow = HIGHT_DEFAULT + 30;
      m_robotState.shiftCycleNow = 0;
      m_robotState.speedMuliplierNow = 2;
      m_robotState.surfaceAngleDevider = 0;
      m_robotState.timeDelayNow = TIME_DELAY * 2;
      m_robotState.edgeEnabled = false;
      m_robotState.bendHight = 0;
      m_robotState.bendShift = 0;
      m_robotState.bendAngle = 0;
    }
    break;
    case ROBOT_INO:
    {
      m_robotState.legLiftNow = LEG_LIFT * 3;
      m_robotState.inputDistanceNow = NORMAL_DISTANCE;
      m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
      m_robotState.robotStateNow = ROBOT_INO;
      m_robotState.legHightNow = HIGHT_DEFAULT + 10;
      m_robotState.shiftCycleNow = 8;
      m_robotState.speedMuliplierNow = 2;
      m_robotState.surfaceAngleDevider = 0;
      m_robotState.timeDelayNow = TIME_DELAY * 4;
      m_robotState.edgeEnabled = false;
      m_robotState.bendHight = 0;
      m_robotState.bendShift = 0;
      m_robotState.bendAngle = 0;
    }
    break;
    case ROBOT_NORM:
    {
      m_robotState.legLiftNow = LEG_LIFT;
      m_robotState.inputDistanceNow = NORMAL_DISTANCE;
      m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
      m_robotState.robotStateNow = ROBOT_NORM;
      m_robotState.legHightNow = HIGHT_DEFAULT;
      m_robotState.shiftCycleNow = 0;
      m_robotState.speedMuliplierNow = 2;
      m_robotState.surfaceAngleDevider = 0;
      m_robotState.timeDelayNow = TIME_DELAY;
      m_robotState.edgeEnabled = false;
      m_robotState.bendHight = 0;
      m_robotState.bendShift = 0;
      m_robotState.bendAngle = 0;
    }
    break;
    case ROBOT_BEND:
    {
      m_robotState.legLiftNow = LEG_LIFT / 2;
      m_robotState.inputDistanceNow = NORMAL_DISTANCE / 2;
      m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
      m_robotState.robotStateNow = ROBOT_BEND;
      m_robotState.legHightNow = HIGHT_DEFAULT;
      m_robotState.shiftCycleNow = 0;
      m_robotState.speedMuliplierNow = 2;
      m_robotState.surfaceAngleDevider = 6;
      m_robotState.timeDelayNow = TIME_DELAY;
      m_robotState.edgeEnabled = true;
      m_robotState.bendHight = BEND_HIGHT;
      m_robotState.bendShift = BEND_SHIFT;
      m_robotState.bendAngle = BEND_ANGLE;
    }
    break;
    case ROBOT_RUN:
    {
      m_robotState.legLiftNow = LEG_LIFT;
      m_robotState.inputDistanceNow = NORMAL_DISTANCE;
      m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
      m_robotState.robotStateNow = ROBOT_RUN;
      m_robotState.legHightNow = HIGHT_DEFAULT;
      m_robotState.shiftCycleNow = 0;
      m_robotState.speedMuliplierNow = 2;
      m_robotState.surfaceAngleDevider = 4;
      m_robotState.timeDelayNow = TIME_DELAY;
      m_robotState.edgeEnabled = false;
      m_robotState.bendHight = 0;
      m_robotState.bendShift = 0;
      m_robotState.bendAngle = 0;
    }
    break;
    case ROBOT_ROLL:
    {
      m_robotState.legLiftNow = LEG_LIFT;
      m_robotState.inputDistanceNow = NORMAL_DISTANCE;
      m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
      m_robotState.robotStateNow = ROBOT_ROLL;
      m_robotState.legHightNow = HIGHT_DEFAULT;
      m_robotState.shiftCycleNow = 0;
      m_robotState.speedMuliplierNow = 2;
      m_robotState.surfaceAngleDevider = -4;
      m_robotState.timeDelayNow = TIME_DELAY;
      m_robotState.edgeEnabled = false;
      m_robotState.bendHight = 0;
      m_robotState.bendShift = 0;
      m_robotState.bendAngle = 0;
    }
    break;
    default:
    break;
  }
}

// check for slop
unsigned char _inputsCheck(void) {
  // check current
  if (m_robotState.currentStateNow == C_DEAD_BATTERY) {
    return ROBOT_NORM;
  }
  if (m_robotState.currentStateNow == C_HIGH_CURRENT) {
    return ROBOT_NORM;
  }
   // check gyro state
  switch (m_gyroState.stateGyro) {
    case GYRO_UPSIDEDOWN:
    case GYRO_FELL_LEFT:
    case GYRO_FELL_RIGHT:
    case GYRO_FELL_FRONT:
    case GYRO_FELL_BACK:
      return ROBOT_NORM;
    break;
    case GYRO_SHAKEN:
      return ROBOT_INO;
    break;
    case GYRO_FOLLING_LEFT:
    case GYRO_FOLLING_RIGHT:
    case GYRO_FOLLING_FRONT:
    case GYRO_FOLLING_BACK:
      return ROBOT_CRAWL;
    break;
    default:
    break;
  }
  return ROBOT_DEFAULT;
}

// do state manager
void doStateManager(void) {
  // check inputs
  newRobotState = ROBOT_NORM; //_inputsCheck();
  /*
  // default task low priority
  if ((m_robotState.taskPriorityNow == PRIORITY_LOW) && (newRobotState == ROBOT_DEFAULT)) {
    // inputs ok.
    if ((m_robotState.robotStateNow == ROBOT_NORM) && (oldRobotState == ROBOT_NORM) && (old2RobotState == ROBOT_NORM)) {
      newRobotState = ROBOT_BEND;
    }
  }
  if (m_robotState.robotStateNow == ROBOT_BEND) {
    newRobotState = ROBOT_NORM;
  }
  if (newRobotState == ROBOT_DEFAULT) {
    newRobotState = m_robotState.robotStateNow;
  }
    */
  // update state
  old2RobotState = oldRobotState;
  oldRobotState = m_robotState.robotStateNow;
  if (newRobotState != m_robotState.robotStateNow) {
    _setRobotState(newRobotState);
  }
}

/*
output state
  m_robotState.robotStateNow
    ROBOT_NORM,
    ROBOT_RUN,
    ROBOT_ROLL,
    ROBOT_CRAWL,
    ROBOT_SWIM,
    ROBOT_INO,
    ROBOT_BEND
*/
