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
#define TIME_DELAY              12
// low hight in mm. upper arm is horizontal
#define HIGHT_LOW               63
// normal hight
#define HIGHT_DEFAULT           125
// normal leg lift in mm
#define LEG_LIFT                40
// center position in the leg forward shift. bigger the number more weight on front
#define FORWARD_BALLANCE_SHIFT  0
// sensors geometry
#define SENSOR_ANGLE            0     // down angle
#define SENSOR_HIGHT            0     // hight relative to legs
// robot phisics
#define OFFROAD_ANGLE           10
#define FALLING_ANGLE           45

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
  P_DOSTAND,
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
  P_DOFLIP,
  P_DORECOVER,
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
  STAND_TASK,
  DOWN_TASK,
  FLIP_TASK,
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
  GYRO_FELL_BACK,
  GYRO_FOLLING_LEFT,
  GYRO_FOLLING_RIGHT,
  GYRO_FOLLING_FRONT,
  GYRO_FOLLING_BACK,
  GYRO_SHAKEN,
  GYRO_TURNED_RIGHT,
  GYRO_TURNED_LEFT
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
  ROBOT_INO,
  ROBOT_DEFAULT
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
  char flipStateNow;
  unsigned char taskPriorityNow;
  unsigned char patternNow;
  unsigned char taskNow;
  char speedNow;
  unsigned char fullCycleNow;
  unsigned char halfCycleNow;
  unsigned char pairCycleNow;
} robotState;

//---------------global variables---------------------------
// motors calibration values for all motors
allMotors m_calibrationData = {0, 0, 0, 0, 0, 0, 0, 0};
// servo motor value
short m_motorAngleValue[8] = {0, 0, 0, 0, 0, 0, 0, 0};
// sequence counter 0 to m_robotState.fullCycleNow - 1
phase m_sequenceCounter = {0, 0, 0, 0, 0};
// robot state
robotState m_robotState = {
  IN_NORMAL,               // unsigned char inputStateNow;
  C_NORMAL,                // unsigned char currentStateNow;
  ROBOT_NORM,              // unsigned char robotStateNow;
  HIGHT_DEFAULT,           // short legHightNow;
  LEG_LIFT,                // short legLiftNow;
  2,                       // char speedMuliplierNow; 1 - 3
  1,                       // char flipStateNow;
  PRIORITY_LOW,            // taskPriorityNow;
  P_DOSTAND,               // patternNow
  STAND_TASK,              // taskNow
  0,                       // speedNow
  36,                      // fullCycleNow
  18,                      // halfCycleNow
  0                        // pairCycleNow
};
// gyro state
accRoll m_gyroState = {0, 0, 0, 0, 0, 0, 0, 0, GYRO_NORM};
// ballance correction
allLegs m_legCorrect = {0, 0, 0, 0, 0, 0, 0, 0};
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
    applyTask(doTaskManager(BEGIN_TASK));
    // load task and pattern. direction is 0
    setPattern();
    updateCountPatterns();
  }
}

// set new task and new pattern
void setTaskAndPattern(void) {
  if (m_robotState.patternNow == P_END) {
    // this is the end. do nothing
    delay(10);
    return;
  }
  // not high priority or end of high priority task
  if ((m_robotState.taskPriorityNow != PRIORITY_HIGH) || (m_robotState.patternNow == P_DONE)) {
    // override with high priority task
    taskNext = getHighPriorityTask();
    if (taskNext == DEFAULT_TASK) {
      taskNext = m_robotState.taskNow;
    } else {
      applyTask(doTaskManager(taskNext));
      // update robot state
      doStateManager();
      return;
    }
  }
  // any priority end of task
  if (m_robotState.patternNow == P_DONE) {
    // check for normal priority
    taskNext = getNormalTask();
    if (taskNext == DEFAULT_TASK) {
      taskNext = defaultTask;
    }
    applyTask(doTaskManager(taskNext));
    // update robot state
    doStateManager();
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
    // update gyro readings
    updateGyro();
    // update current readings
    updateCurrent();
    // read proximity sensors
    updateInputs();
    // once in a pattern after delay
    if (m_sequenceCounter.m == 0) {
      //printInputsDebug();
      //printCurrentStateDebug();
      //printLineGyroDebug();
      //printRollGyroDebug();
    }
    // update ballance
    updateBallance();
    updateBallanceServo();
  }
  // once in a pattern
  if (m_sequenceCounter.m == 0) {
    // set new task and next pattern
    setTaskAndPattern();
    if (m_robotState.taskNow != taskNext) {
      m_robotState.taskNow = taskNext;
      //printTaskNameDebug(m_robotState.taskNow); // DEBUG
    }
    //printPatternNameDebug(m_robotState.patternNow); // DEBUG
    switch (m_robotState.patternNow) {
      case P_SETDIRECTION:
      {
        setDirectionGyro();
      }
      break;
      case P_RESETDIRECTION:
      {
        resetDirectionGyro();
      }
      break;
      case P_RESETGIRO:
      {
        resetGyro();
      }
      break;
      case P_RESTOREDIRECTION:
      {
        restoreDirectionGyro();
      }
      break;
      case P_DOLOW:
      {
        setServo(HIGHT_LOW, HIGHT_LOW, 20);
      }
      break;
      case P_DOSTAND:
      {
        setServo(m_robotState.legHightNow, m_robotState.legHightNow, 20);
      }
      break;
      case P_DORECOVER:
      {
        // smart recover
        if (m_gyroState.accRollX > 0) {
          // recover left
          setServo(170, 40, 0);
          setServo(170, 170, 0);
          setServo(40, 40, 0);
        } else {
          // recover right
          setServo(40, 170, 0);
          setServo(170, 170, 0);
          setServo(40, 40, 0);
        }
      }
      break;
      case P_DOFLIP:
      {
        // do flip
        if (m_robotState.flipStateNow == 1) {
          m_robotState.flipStateNow = -1;
        } else {
          m_robotState.flipStateNow = 1;
        }
      }
      case P_REPEAT:
      case P_DONE:
      // do nothing
      // immediatelly run loop again
      break;
      case P_DODOWN:
      {
        // disable motors
        setServo(HIGHT_LOW, HIGHT_LOW, 20);
        detachServo();
      }
      break;
      case P_SETPRIORITY_HIGH:
      {
        m_robotState.taskPriorityNow = PRIORITY_HIGH;
      }
      break;
      case P_SETPRIORITY_NORM:
      {
        m_robotState.taskPriorityNow = PRIORITY_NORM;
      }
      break;
      case P_SETPRIORITY_LOW:
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
