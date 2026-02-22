/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Manager file
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
    case FLIP_TASK:
      return RESET_TASK;
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
    case ROBOT_NORM:
    {
      m_robotState.legLiftNow = LEG_LIFT;
      m_robotState.robotStateNow = ROBOT_NORM;
      m_robotState.legHightNow = HIGHT_DEFAULT;
      m_robotState.speedMuliplierNow = 2;
    }
    break;
    case ROBOT_INO:
    {
      m_robotState.legLiftNow = LEG_LIFT * 2;
      m_robotState.robotStateNow = ROBOT_INO;
      m_robotState.legHightNow = HIGHT_DEFAULT;
      m_robotState.speedMuliplierNow = 1;
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
    case GYRO_RESET:
    case GYRO_FELL_LEFT:
    case GYRO_FELL_RIGHT:
    case GYRO_FELL_FRONT:
    case GYRO_FELL_BACK:
    case GYRO_NORM:
      return ROBOT_NORM;
    break;
    case GYRO_FOLLING_LEFT:
    case GYRO_FOLLING_RIGHT:
    case GYRO_FOLLING_FRONT:
    case GYRO_FOLLING_BACK:
      // change walking state ?
      return ROBOT_INO;
    break;
    default:
    break;
  }
  return ROBOT_DEFAULT;
}

// do state manager
void doStateManager(void) {
  // check inputs
  newRobotState = _inputsCheck();
  // update state
  old2RobotState = oldRobotState;
  oldRobotState = m_robotState.robotStateNow;
  if (newRobotState != m_robotState.robotStateNow) {
    _setRobotState(newRobotState);
  }
}
