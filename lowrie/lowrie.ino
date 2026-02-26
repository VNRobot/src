/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

#include <EEPROM.h>

// software version hardcoded. should be changed manually
#define ROBOT_VERSION           77
// calibration current in ma default 640 or 2000 to disable
#define CALIBRATION_CURRENT     2000
// maximal pair of legs current in ma 2000 to disable
#define MAX_CURRENT             1000
// low battery level in mv
#define LOW_BATTERY             6200
#define DEAD_BATTERY            6000
// input grounded 0 - 1023
#define INPUT_GROUNDED          400
// main time delay in ms. bigger the number slower the robot
#define TIME_DELAY              10
// low hight in mm. upper arm is horizontal
#define HIGHT_LOW               88
// normal hight
#define HIGHT_DEFAULT           125
// normal leg lift in mm
#define LEG_LIFT                40
// center position in the leg forward shift. bigger the number more weight on front
#define FORWARD_BALLANCE_SHIFT  0
// sensors geometry
#define SENSOR_ANGLE            20     // down angle
#define SENSOR_HIGHT            10     // mm hight relative to legs
#define SENSOR_DISTANCE_MIN     12     // cm
#define SENSOR_DISTANCE_MAX     48     // cm
// robot phisics
#define OFFROAD_ANGLE           4
#define SLOP_ANGLE              12
#define FALLING_ANGLE           45
// main servo pattern counter end
#define SERVO_FULL_CYCLE        72
#define SERVO_HALF_CYCLE        36
#define SERVO_PAIR_SHIFT        18
#define MAIN_FULL_CYCLE         36
#define MAIN_HALF_CYCLE         18
// robot size
#define ROBOT_SIZE_DIVIDER      2
// ballance correction max value
#define STATIC_BALLANCE_MAX     60
#define DYNAMIC_BALLANCE_MAX    20
// pattern repeat counter
#define REPEAT_COUNTER_END      8

// input state
enum inState {
  IN_WALL_FRONTLEFT,
  IN_WALL_FRONTRIGHT,
  IN_WALL_LEFT,
  IN_WALL_RIGHT,
  IN_OBSTACLE_FRONTLEFT,
  IN_OBSTACLE_FRONTRIGHT,
  IN_OBSTACLE_LEFT,
  IN_OBSTACLE_RIGHT,
  IN_NORMAL             
};
// current state
enum cState {
  C_LOW_BATTERY,
  C_DEAD_BATTERY,
  C_HIGH_CURRENT,
  C_NORMAL             
};
// patterns
enum rPatterns {
  P_STANDGO,
  P_STANDGOLEFT,
  P_STANDGORIGHT,
  P_GOFORWARD,
  P_GOLEFT,
  P_GORIGHT,
  P_GOBACK,
  P_GOBACKLEFT,
  P_GOBACKRIGHT,
  Q_DOLOW,
  Q_DODOWN,
  Q_DOSTAND,
  Q_DORESET,
  Q_DORECOVER,
  Q_DOFLIP,
  Q_DONE,
  Q_SETDIRECTION,
  Q_RESETDIRECTION,
  Q_RESTOREDIRECTION,
  Q_REVERSEDIRECTION,
  Q_RESETGIRO,
  Q_REPEAT,
  Q_SETPRIORITY_HIGH,
  Q_SETPRIORITY_NORM,
  Q_SETPRIORITY_LOW,
  Q_END
};
// tasks
enum rTasks {
  BEGIN_TASK = 0,
  GOBACKLEFT_TASK,
  GOBACKRIGHT_TASK,
  GOTURNRIGHT_TASK,
  GOTURNLEFT_TASK,
  STANDTURNRIGHT_TASK,
  STANDTURNLEFT_TASK,
  STANDTURNRIGHT2_TASK,
  STANDTURNLEFT2_TASK,
  GO_TASK,
  GOBACK_TASK,
  STAND_TASK,
  DOWN_TASK,
  FLIP_TASK,
  RECOVER_TASK,
  RESET_TASK,
  DEFAULT_TASK
};
// gyro state
enum gState {
  GYRO_NORM,
  GYRO_UPSIDEDOWN,
  GYRO_RESET,
  GYRO_FELL_LEFT,
  GYRO_FELL_RIGHT,
  GYRO_FELL_FRONT,
  GYRO_FELL_BACK
};
// task priority
enum tPriority {
  PRIORITY_HIGH,
  PRIORITY_NORM,
  PRIORITY_LOW,
};
// robot state
enum rState {
  ROBOT_NORM,
  ROBOT_INO
};
// structure for one leg motors
struct motors {
  char motor1;
  char motor2;
};
// structure for one leg data
struct leg {
  short hight;
  short shift;
  bool lifted;
};
// legs motors structure
struct allMotors {
  motors fl;
  motors fr;
  motors rl;
  motors rr;
};
// legs motors structure
struct allLegs {
  leg fl;
  leg fr;
  leg rl;
  leg rr;
};
// acc and gyro data structure
typedef struct accRoll {
  short accRollX;                 // roll       right - positive   -90 0 90 upsidedown also 0
  short accPitchY;                // pitch      up - positive   -90 0 90 upsidedown also 0
  short accUpsideZ;               // z          upside down - negative
  short rollMin;
  short rollMax;
  unsigned char rollMinTime;
  unsigned char rollMaxTime;
  short direction;                // direction  turn right - positive
  unsigned char stateGyro;
} accRoll;
// leg timing phase. main m
struct phase {
  unsigned char m;
  unsigned char fl;
  unsigned char fr;
  unsigned char rl;
  unsigned char rr;
};
// robot state structure
typedef struct robotState {
  unsigned char inputStateNow;
  unsigned char currentStateNow;
  unsigned char robotStateNow;
  short legHightNow;
  short legLiftNow;
  char speedMuliplierNow;
  char flipStateLNow;
  char flipStateRNow;
  unsigned char taskPriorityNow;
  unsigned char patternNow;
  unsigned char taskNow;
  char speedNow;
} robotState;

//---------------global variables---------------------------
// motors calibration values for all motors
allMotors m_calibrationData = {0, 0, 0, 0, 0, 0, 0, 0};
// servo motor value
short m_motorAngleValue[8] = {0, 0, 0, 0, 0, 0, 0, 0};
// sequence counters
phase m_sequenceCounter = {0, 0, 0, 0, 0};
// robot state
robotState m_robotState = {
  IN_NORMAL,               // unsigned char inputStateNow;
  C_NORMAL,                // unsigned char currentStateNow;
  ROBOT_NORM,              // unsigned char robotStateNow;
  HIGHT_DEFAULT,           // short legHightNow;
  LEG_LIFT,                // short legLiftNow;
  2,                       // char speedMuliplierNow; 1 or 2
  1,                       // char flipStateLNow;
  1,                       // char flipStateRNow;
  PRIORITY_LOW,            // taskPriorityNow;
  Q_DOSTAND,               // patternNow
  STAND_TASK,              // taskNow
  0                        // speedNow
};
// gyro state
accRoll m_gyroState = {0, 0, 0, 0, 0, 0, 0, 0, GYRO_NORM};
// ballance correction
allLegs m_legCorrect = {0, 0, false, 0, 0, false, 0, 0, false, 0, 0, false};
//----------------------------------------------------------
// servo cycle is done flag
bool cycleDone = true;
// default task from rTasks
unsigned char defaultTask = GO_TASK;
// next task
unsigned char taskNext = STAND_TASK;
// variable for temporary use
unsigned char i;

// runs once on boot or reset
void setup() {
  // Start serial for debugging
  Serial.begin(9600);
  Serial.println(F("Device started"));
  delay(200);
  // init servo motors
  initServo();
  doPWMServo(200);
  setServo(HIGHT_LOW, HIGHT_LOW, 20);
  // init gyro MPU6050 using I2C
  initGyro();
  doPWMServo(200);
  updateGyro();
  doPWMServo(20);
  resetGyro();
  doPWMServo(20);
  updateGyro();
  // check for Mode button press or if not calibrated
  if (!doCalibration()) {
    setServo(HIGHT_DEFAULT, HIGHT_DEFAULT, 20);
    doPWMServo(200);
    // init current readings
    initCurrent();
    // init digital sensors
    initInputs();
    // update current readings
    updateCurrent();
    // read proximity sensors
    updateInputs();
    // explore mode
    Serial.println(F("Entering explore mode"));
    applyTask(doManageTasks(BEGIN_TASK));
    // load task and pattern. direction is 0
    setPattern();
    updateCountPatterns();
  }
}

// set new task and new pattern
void setTaskAndPattern(void) {
  if (m_robotState.patternNow == Q_END) {
    // this is the end. do nothing
    delay(10);
    return;
  }
  // not high priority or end of high priority task
  if ((m_robotState.taskPriorityNow != PRIORITY_HIGH) || (m_robotState.patternNow == Q_DONE)) {
    // override with high priority task
    taskNext = getHighPriorityTask();
    if (taskNext == DEFAULT_TASK) {
      taskNext = m_robotState.taskNow;
    } else {
      applyTask(doManageTasks(taskNext));
      return;
    }
  }
  // any priority end of task
  if (m_robotState.patternNow == Q_DONE) {
    // check for normal priority
    taskNext = getNormalTask(getDirectionGyro());
    if (taskNext == DEFAULT_TASK) {
      taskNext = defaultTask;
    }
    applyTask(doManageTasks(taskNext));
    return;
  }
  // set next pattern
  setNextPatternInTask();
}

// set motors and read sensors
void doCycle(void) {
  // update servo motors values, move motors
  updateLegsServo(getWalkPatterns());
  doPWMServo(TIME_DELAY);
  cycleDone = true;
}

// the loop function runs over and over again forever
void loop() {
  if (cycleDone) {
    // runs only after delay
    cycleDone = false;
    // update motor pattern point
    updateCountPatterns();
    // update current readings
    updateCurrent();
    // update gyro readings
    updateGyro();
    // read proximity sensors
    updateInputs();
    // once in a pattern after delay
    if (m_sequenceCounter.m == 0) {
      //printInputsDebug();
      //printCurrentStateDebug();
      //_printLineGyroDebug();
      //_printRollGyroDebug();
      // set robot state
      setRobotState();
    }
    // update ballance
    updateBallance();
    updateBallanceServo();
  }
  // once in a pattern
  if (m_sequenceCounter.m == 0) {
    // set new task and next pattern
    setTaskAndPattern();
    //printPatternNameDebug(m_robotState.patternNow); // DEBUG
    switch (m_robotState.patternNow) {
      case Q_SETDIRECTION:
      {
        setDirectionGyro();
      }
      break;
      case Q_RESETDIRECTION:
      {
        resetDirectionGyro();
      }
      break;
      case Q_REVERSEDIRECTION:
      {
        reverseDirectionGyro();
      }
      break;
      case Q_RESETGIRO:
      {
        resetGyro();
      }
      break;
      case Q_RESTOREDIRECTION:
      {
        restoreDirectionGyro();
      }
      break;
      case Q_DOLOW:
      {
        setServo(HIGHT_LOW, HIGHT_LOW, 20);
      }
      break;
      case Q_DOSTAND:
      {
        setServo(m_robotState.legHightNow, m_robotState.legHightNow, 20);
      }
      break;
      case Q_DORECOVER:
      {
        setServo(HIGHT_LOW, HIGHT_LOW, 0);
        if (m_gyroState.accRollX < 0) {
          m_robotState.flipStateLNow = 1;
          m_robotState.flipStateRNow = -1;
          setServo(180, 180, 0);
          m_robotState.flipStateLNow = -1;
          m_robotState.flipStateRNow = 1;
          setServo(180, 180, 0);
        } else {
          m_robotState.flipStateLNow = -1;
          m_robotState.flipStateRNow = 1;
          setServo(180, 180, 0);
          m_robotState.flipStateLNow = 1;
          m_robotState.flipStateRNow = -1;
          setServo(180, 180, 0);
        }
        m_robotState.flipStateLNow = 1;
        m_robotState.flipStateRNow = 1;
        setServo(HIGHT_LOW, HIGHT_LOW, 0);
      }
      break;
      case Q_DOFLIP:
        // do nothing for now
      case Q_DORESET:
      {
        if (m_gyroState.accUpsideZ < 0) {
          m_robotState.flipStateLNow = -1;
          m_robotState.flipStateRNow = -1;
        } else {
          m_robotState.flipStateLNow = 1;
          m_robotState.flipStateRNow = 1;
        }
      }
      break;
      case Q_REPEAT:
      case Q_DONE:
      // do nothing
      // immediatelly run loop again
      break;
      case Q_DODOWN:
      {
        // disable motors
        setServo(HIGHT_LOW, HIGHT_LOW, 20);
        detachServo();
      }
      break;
      case Q_SETPRIORITY_HIGH:
      {
        m_robotState.taskPriorityNow = PRIORITY_HIGH;
      }
      break;
      case Q_SETPRIORITY_NORM:
      {
        m_robotState.taskPriorityNow = PRIORITY_NORM;
      }
      break;
      case Q_SETPRIORITY_LOW:
      {
        m_robotState.taskPriorityNow = PRIORITY_LOW;
      }
      break;
      default:
      {
        // set pattern
        setPattern();
        doCycle();
      }
      break;
    }
  } else {
    // cycle in the middle of pattern
    doCycle();
  }
}

// print task  name
void printPatternNameDebug(unsigned char patternNow) {
  switch (patternNow) {
    case Q_DOSTAND:
      Serial.print(F(" Q_DOSTAND "));
    break;
    case P_STANDGO:
      Serial.print(F(" P_STANDGO "));
    break;
    case P_STANDGOLEFT:
      Serial.print(F(" P_STANDGOLEFT "));
    break;
    case P_STANDGORIGHT:
      Serial.print(F(" P_STANDGORIGHT "));
    break;
    case P_GOFORWARD:
      Serial.print(F(" P_GOFORWARD "));
    break;
    case P_GOLEFT:
      Serial.print(F(" P_GOLEFT "));
    break;
    case P_GORIGHT:
      Serial.print(F(" P_GORIGHT "));
    break;
    case P_GOBACK:
      Serial.print(F(" P_GOBACK "));
    break;
    case P_GOBACKLEFT:
      Serial.print(F(" P_GOBACKLEFT "));
    break;
    case P_GOBACKRIGHT:
      Serial.print(F(" P_GOBACKRIGHT "));
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
    case Q_RESETDIRECTION:
      Serial.print(F(" Q_RESETDIRECTION "));
    break;
    case Q_RESTOREDIRECTION:
      Serial.print(F(" Q_RESTOREDIRECTION "));
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

// print sensor inputs state
void printInputsDebug(void) {
  // print input state
  switch (m_robotState.inputStateNow) {
    case IN_WALL_FRONTLEFT:
      Serial.println(F(" IN_WALL_FRONTLEFT "));
    break;
    case IN_WALL_FRONTRIGHT:
      Serial.println(F(" IN_WALL_FRONTRIGHT "));
    break;
    case IN_WALL_LEFT:
      Serial.println(F(" IN_WALL_LEFT "));
    break;
    case IN_WALL_RIGHT:
      Serial.println(F(" IN_WALL_RIGHT "));
    break;
    case IN_OBSTACLE_FRONTLEFT:
      Serial.println(F(" IN_OBSTACLE_FRONTLEFT "));
    break;
    case IN_OBSTACLE_FRONTRIGHT:
      Serial.println(F(" IN_OBSTACLE_FRONTRIGHT "));
    break;
    case IN_OBSTACLE_LEFT:
      Serial.println(F(" IN_OBSTACLE_LEFT "));
    break;
    case IN_OBSTACLE_RIGHT:
      Serial.println(F(" IN_OBSTACLE_RIGHT "));
    break;
    case IN_NORMAL:
      Serial.println(F(" IN_NORMAL "));
    break;
    default:
      Serial.println(F(" Wrong inputs state "));
  }
}

// print current state
void printCurrentStateDebug(void) {
  // print input state
  switch (m_robotState.currentStateNow) {
    case C_DEAD_BATTERY:
      Serial.print(F(" C_DEAD_BATTERY "));
    break;
    case C_LOW_BATTERY:
      Serial.print(F(" C_LOW_BATTERY "));
    break;
    case C_HIGH_CURRENT:
      Serial.print(F(" C_HIGH_CURRENT "));
    break;
    case C_NORMAL:
      Serial.print(F(" C_NORMAL "));
    break;
    default:
      Serial.println(F(" Wrong inputs state "));
  }
}

// print gyro values
void _printLineGyroDebug(void) {
  Serial.print(" aRoll ");
  Serial.print((int)m_gyroState.accRollX);
  Serial.print(" aPitch ");
  Serial.print((int)m_gyroState.accPitchY);
  Serial.print(" Z ");
  Serial.print((int)m_gyroState.accUpsideZ);
  Serial.print(" direction ");
  Serial.println((int)m_gyroState.direction);
}

// print gyro values
void _printRollGyroDebug(void) {
  Serial.print(" rollMin ");
  Serial.print((int)m_gyroState.rollMin);
  Serial.print(" rollMax ");
  Serial.print((int)m_gyroState.rollMax);
  Serial.print(" rollMinTime ");
  Serial.print((int)m_gyroState.rollMinTime);
  Serial.print(" rollMaxTime ");
  Serial.println((int)m_gyroState.rollMaxTime);
}
