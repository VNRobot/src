/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

// software version hardcoded. should be changed manually
#define ROBOT_VERSION           77
// calibration current in ma default 640 or 2000 to disable
#define CALIBRATION_CURRENT     2000
// maximal pair of legs current in ma 2000 to disable
#define MAX_CURRENT             2000
// low battery level in mv
#define LOW_BATTERY             6000
// input grounded 0 - 1023
#define INPUT_GROUNDED          400
// main servo pattern counter end
#define SERVO_FULL_CYCLE        36
#define SERVO_HALF_CYCLE        18
// center position in the leg forward shift. bigger the number more weight on front
#define FORWARD_BALLANCE_SHIFT  0
// main time delay in ms. bigger the number slower the robot
#define TIME_DELAY              8
// low hight in mm. upper arm is horizontal
#define HIGHT_LOW               63
// normal hight
#define HIGHT_DEFAULT           125
// normal leg lift in mm
#define LEG_LIFT                40
// normal input sensors distance in sm
#define NORMAL_DISTANCE         50

// input state
enum inState {
  IN_LOW_BATTERY,
  IN_HIGH_CURRENT_F,
  IN_HIGH_CURRENT_R,
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

// patterns
enum rPatterns {
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
  P_RESETDIRECTION,
  P_RESTOREDIRECTION,
  P_RESETGIRO,
  P_ENABLEINPUTS,
  P_DISABLEINPUTS,
  P_CRAWLSTART,
  P_SWIMSTART,
  P_INOSTART,
  P_NORMALSTART,
  P_REPEAT,
  P_END
};
// tasks
enum rTasks {
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
  STANDGO_TASK,
  STAND_TASK,
  DOWN_TASK,
  RECOVER_LEFT_TASK,
  RECOVER_RIGHT_TASK,
  FLIP_TASK,
  DEFAULT_TASK
};
// gyro state
enum gState {
  GYRO_NORM,
  GYRO_SHAKEN,
  GYRO_SWIM,
  GYRO_UPSIDEDOWN,
  GYRO_HIT_SIDE,
  GYRO_HIT_FRONT,
  GYRO_FELL_LEFT,
  GYRO_FELL_RIGHT,
  GYRO_FELL_FRONT,
  GYRO_FELL_BACK,
  GYRO_DOWN_HILL,
  GYRO_UP_HILL,
  GYRO_FOLLING_LEFT,
  GYRO_FOLLING_RIGHT,
  GYRO_FOLLING_FRONT,
  GYRO_FOLLING_BACK 
};
// robot state
enum rState {
  ROBOT_NORM,
  ROBOT_CRAWL,
  ROBOT_SWIM,
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
  int accAngleX;
  int accAngleY;
  int rollMin;
  int rollMax;
  unsigned char rollMinTime;
  unsigned char rollMaxTime;
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
  bool sensorsEnabledNow;
  unsigned char inputDistanceNow;
  unsigned char inputStateNow;
  unsigned char robotStateNow;
  unsigned char halfCycleNow;
  unsigned char shiftCycleNow;
  unsigned char timeDelayNow;
  short legHightNow;
  short legLiftNow;
  char speedMuliplierNow;
  char flipStateNow;
} robotState;

//---------------global variables---------------------------
// motors calibration values for all motors
allMotors m_calibrationData = {0, 0, 0, 0, 0, 0, 0, 0};
// servo motor value
short m_motorAngleValue[8] = {0, 0, 0, 0, 0, 0, 0, 0};
// sequence counter 0 to SERVO_FULL_CYCLE - 1
phase m_sequenceCounter = {0, 0, 0, 0, 0};
// robot state
robotState m_robotState = {
  false,                   // bool sensorsEnabledNow;
  NORMAL_DISTANCE / 2,     // unsigned char inputDistanceNow;
  IN_NORMAL,               // unsigned char inputStateNow;
  ROBOT_NORM,              // unsigned char robotStateNow;
  SERVO_HALF_CYCLE,        // unsigned char halfCycleNow;
  0,                       // unsigned char shiftCycleNow;
  TIME_DELAY,              // unsigned char timeDelayNow;
  HIGHT_DEFAULT,           // short legHightNow;
  LEG_LIFT,                // short legLiftNow;
  2,                       // char speedMuliplierNow;
  1                        // char flipStateNow;
};
// gyro state
accRoll m_gyroState = {
  0,        //int accAngleX;
  0,        //int accAngleY;
  0,        //int rollMin;
  0,        //int rollMax;
  0,        //unsigned char rollMinTime;
  0,        //unsigned char rollMaxTime;
  GYRO_NORM //unsigned char stateGyro;
};
//----------------------------------------------------------

// current pattern defined in rPatterns
unsigned char patternNow = P_DOSTAND;
// default task from rTasks
unsigned char defaultTask = GO_TASK;
// current task
unsigned char taskNow = STAND_TASK;
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
  setServo(HIGHT_LOW);
  // init gyro MPU6050 using I2C
  initGyro();
  doPWMServo(200);
  resetGyro();
  doPWMServo(20);
  // update gyro readings
  updateGyro();
  // check for Mode button press or if not calibrated
  if (!doCalibration()) {
    setServo(HIGHT_DEFAULT);
    doPWMServo(200);
    setServo(HIGHT_LOW);
    doPWMServo(2000);
    setServo(HIGHT_DEFAULT);
    doPWMServo(200);
    // init digital sensors
    initInputs();
    // update inputs direction is 0
    updateInputs(0);
    // explore mode
    Serial.println(F("Entering explore mode"));
    applyTask(BEGIN_TASK);
    // load task and pattern. direction is 0
    setPattern(patternNow, 0);
    updateCountPatterns();
  }
}

// the loop function runs over and over again forever
void loop() {
  if (m_sequenceCounter.m == 0) {
    // check emergency task
    taskNext = getHighPriorityTaskByInputs();
    if ((taskNext != DEFAULT_TASK) && (taskNow != taskNext)) {
      taskNow = taskNext;
      // apply new task
      applyTask(taskNow);
      // debug print
      //printTaskname(taskNow);
      // get new task pattern
      patternNow = getPatternInTask();
    } else {
      // check for normal task end
      if ((patternNow == P_DONE) || (((patternNow == P_STANDGO) || (patternNow == P_GOFORWARD)) && (m_robotState.sensorsEnabledNow))) {
        // get next task
        taskNow = getNormalTaskByInputs(defaultTask);
        // apply new task
        applyTask(taskNow);
        // debug print
        //printTaskname(taskNow);
        // get new task pattern
        patternNow = getPatternInTask();
      } else {
        if (patternNow != P_END) {
          // get next pattern
          patternNow = getNextPatternInTask();
        }
      }
    }
    // debug print
    // printPatternName(patternNow);
    switch (patternNow) {
      case P_STANDGO:
      case P_GOFORWARD:
      {
        setPattern(patternNow, getDirectionGyro());
        doCycle();
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
      case P_DISABLEINPUTS:
      {
        m_robotState.sensorsEnabledNow = false;
      }
      break;
      case P_ENABLEINPUTS:
      {
        m_robotState.sensorsEnabledNow = true;
      }
      break;
      case P_DOLOW:
      {
        setServo(HIGHT_LOW);
      }
      break;
      case P_DOSTAND:
      {
        setServo(m_robotState.legHightNow);
      }
      break;
      case P_DOFLIP:
      {
        doFlipServo();
      }
      case P_REPEAT:
      case P_DONE:
      // do nothing
      // immediatelly run loop again
      break;
      case P_DODOWN:
      {
        // disable motors
        setServo(HIGHT_LOW);
        detachServo();
      }
      break;
      case P_CRAWLSTART:
      {
        m_robotState.legLiftNow = LEG_LIFT * 3;
        m_robotState.inputDistanceNow = NORMAL_DISTANCE / 2;
        m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
        m_robotState.robotStateNow = ROBOT_CRAWL;
        m_robotState.legHightNow = HIGHT_DEFAULT;
        m_robotState.shiftCycleNow = 0;
        m_robotState.speedMuliplierNow = 2;
        m_robotState.timeDelayNow = TIME_DELAY;
      }
      break;
      case P_SWIMSTART:
      {
        m_robotState.legLiftNow = LEG_LIFT * 3;
        m_robotState.inputDistanceNow = NORMAL_DISTANCE / 2;
        m_robotState.halfCycleNow = 0;
        m_robotState.robotStateNow = ROBOT_SWIM;
        m_robotState.legHightNow = HIGHT_DEFAULT + 30;
        m_robotState.shiftCycleNow = 0;
        m_robotState.speedMuliplierNow = 2;
        m_robotState.timeDelayNow = TIME_DELAY * 2;
      }
      break;
      case P_INOSTART:
      {
        m_robotState.legLiftNow = LEG_LIFT * 2;
        m_robotState.inputDistanceNow = NORMAL_DISTANCE;
        m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
        m_robotState.robotStateNow = ROBOT_INO;
        m_robotState.legHightNow = HIGHT_DEFAULT - 30;
        m_robotState.shiftCycleNow = 8;
        m_robotState.speedMuliplierNow = 4;
        m_robotState.timeDelayNow = TIME_DELAY * 5;
      }
      break;
      case P_NORMALSTART:
      {
        m_robotState.legLiftNow = LEG_LIFT;
        m_robotState.inputDistanceNow = NORMAL_DISTANCE;
        m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
        m_robotState.robotStateNow = ROBOT_NORM;
        m_robotState.legHightNow = HIGHT_DEFAULT;
        m_robotState.shiftCycleNow = 0;
        m_robotState.speedMuliplierNow = 2;
        m_robotState.timeDelayNow = TIME_DELAY;
      }
      break;
      default:
      {
        setPattern(patternNow, 0);
        doCycle();
      }
      break;
    }
  } else {
    doCycle();
  }
}

// set motors and read sensors
void doCycle(void) {
  // update servo motors values, move motors
  updateLegsServo(getWalkPatterns());
  doPWMServo(m_robotState.timeDelayNow);
  // update motor pattern point
  updateCountPatterns();
  // read proximity sensors
  m_robotState.inputStateNow = updateInputs(getDirectionGyro());
  // update gyro readings
  updateGyro();
  // update ballance
  //updateBallanceServo(getStaticBallance(m_gyroState, getTouchInputs(), getWalkingMode()));
}
