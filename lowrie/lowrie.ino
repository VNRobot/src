/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

// software version hardcoded. should be changed manually
#define ROBOT_VERSION 55
// main servo pattern counter end
#define SERVO_FULL_CYCLE 36
#define SERVO_HALF_CYCLE 18
// motor angle correction for lowrie -30
#define MOTOR_CORRECTION  0 //-30
// calibration motor angle 90 - vertical, 0 - horizontal
#define CALIBRATION_ANGLE 0 //90
// calibration current in ma default 640 or 2000 to disable
#define CALIBRATION_CURRENT 2000 //64
// center position in the leg forward shift // bigger the number more weight on front
#define FORWARD_BALLANCE_SHIFT 14 // 0
// center motors direction. -1 for lowrie 1.1
#define CENTER_MOTOR_DIRECTION -1 //1
// center motor direction max angle. for lowrie is 10
#define CENTER_MAX_TURN 10
// nalf distance between motors in mm
#define BETWEEN_MOTORS 30 //16
// low battery level in mv
#define LOW_BATTERY 6000
// input grounded 0 - 1023
#define INPUT_GROUNDED 400

// input state
enum inState {
  IN_LOW_BATTERY,
  IN_HIGH_CURRENT_1,
  IN_HIGH_CURRENT_2,
  IN_HIGH_CURRENT_3,
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
  P_STANDGOSHIFTLEFT,
  P_STANDGOSHIFTRIGHT,
  P_GOSHIFTLEFT,
  P_GOSHIFTRIGHT,
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
  P_SHORTDELAY,
  P_LONGDELAY,
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
  GOSHIFTRIGHT_TASK,
  GOSHIFTLEFT_TASK,
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
  motors hl;
  motors hr;
  motors fl;
  motors fr;
  motors rl;
  motors rr;
};
// legs motors structure
struct allLegs {
  leg hl;
  leg hr;
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
  unsigned char hl;
  unsigned char hr;
  unsigned char fl;
  unsigned char fr;
  unsigned char rl;
  unsigned char rr;
};
// touch with 3k resistor
struct touch {
  bool set1;
  bool set2;
  bool set3;
};
// acc and gyro data structure
typedef struct robotSetup {
  unsigned char motorsCount; // motors count
  unsigned char shiftCycle;
  unsigned char timeDelayShort;
  unsigned char timeDelayLong;
  unsigned short maxInputCurrent; // maximal pair of legs current
  unsigned char normalInputDistance; // normal distance sensor beam to ground
  short defaultHight; // default height in mm
} robotSetup;
// robot state structure
typedef struct robotState {
  unsigned char robotStateNow;
  unsigned char halfCycleNow;
  unsigned char shiftCycleNow;
  unsigned char timeDelayNow;
  short legHightNow;
  short legLiftNow;
  char centerMotorValueNow;
  bool stepSteeringNow;
  unsigned char inputDistanceNow;
  char speedMuliplierNow; // walking speed multipier max = 4 for ino
} robotState;

// motors calibration values for optional 12 motors
allMotors calibrationData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// inputs state defined in inState
unsigned char inputState = IN_NORMAL;
// gyro state
accRoll gyroState;
// sequence counter 0 to SERVO_FULL_CYCLE - 1
phase sequenceCounter = {0, 0, 0, 0, 0, 0, 0};
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
//---------------global variables---------------------------
// servo motor value
short m_motorAngleValue[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// init data structure
//        m_init.    motorsCount shiftCycle timeDelayShort timeDelayLong maxInputCurrent normalInputDistance defaultHight;
// 8 motors small turtle robot
robotSetup m_init = {8,          0,         8,            10,           1800,           50,                 120};
// 8 motors small inline robot
//robotSetup m_init = {8,          8,         60,            60,           1800,           42,                 100};
// 10 motors small robot
//robotSetup m_init = {10,         0,         8,             8,            1800,           42,                 130};
// 12 motors small robot
//robotSetup m_init = {12,         4,         14,             16,          1800,           42,                 125};
// 8 motors big robot
//robotSetup m_init = {8,          0,         20,            20,           2500,           50,                 125};
// 10 motors big robot
//robotSetup m_init = {10,         0,         30,            40,           2500,           42,                 125};
// 12 motors big robot
//robotSetup m_init = {12,         0,         30,            40,           2500,           42,                 125};
//----------------------------------------------------------
//
//------------------------dynamic state---------------------
// robot state          robotStateNow      halfCycleNow      shiftCycleNow           timeDelayNow          legHightNow  legLiftNow  centerMotorValueNow  stepSteeringNow  inputDistanceNow speedMuliplierNow
robotState m_robotState = {ROBOT_NORM, SERVO_HALF_CYCLE, m_init.shiftCycle, m_init.timeDelayShort, m_init.defaultHight,         40,                   0,            true, m_init.normalInputDistance / 2,  2};
// enable sensors flag toggled by P_ENABLEINPUTS and P_DISABLEINPUTS
bool sensorsEnabled = false;
//----------------------------------------------------------

// read button press in blocking mode
// return true when pressed and released
bool _readButtonPress(void) {
  bool buffer = false;
  while (checkModeButtonPressedInputs()) {
    buffer = true;
    delay(100);
  }
  return buffer;
}

// runs once on boot or reset
void setup() {
  // Start serial for debugging
  Serial.begin(9600);
  Serial.println(F("Device started"));
  delay(200);
  initEEPROM();
  // check for Mode button press or if not calibrated
  if (_readButtonPress() || (ROBOT_VERSION != readSoftwareVersionEeprom())) {
    // factory mode is used for legs calibration
    Serial.println(F("Entering factory mode"));
    // do calibration
    if (doCalibration(& calibrationData)) {
      writeCalibrationEeprom(calibrationData);
      writeSoftwareVersionEeprom(ROBOT_VERSION);
      delay(6000);
    }
  } else {
    // read values by using pointer to struct
    readCalibrationEeprom(& calibrationData);
    // init servo motors hight in mm
    initValueServo(calibrationData, 70, 0);
    initServo();
    doPWMServo(200);
    setServo(calibrationData, m_init.defaultHight);
    doPWMServo(200);
    // init digital sensors
    initInputs();
    // update inputs direction is 0
    updateInputs(sequenceCounter.m, sensorsEnabled, 0);
    // init gyro MPU6050 using I2C
    initGyro();
    doPWMServo(200);
    resetGyro();
    doPWMServo(20);
    // explore mode
    Serial.println(F("Entering explore mode"));
    applyTask(BEGIN_TASK);
    // update gyro readings
    gyroState = updateGyro(sequenceCounter.m);
    // load task and pattern. direction is 0
    if (m_init.motorsCount == 10) {
      setCenter(patternNow, 0, 0);
    }
    setPattern(patternNow, 0);
    sequenceCounter = updateCountPatterns(m_robotState.shiftCycleNow, SERVO_FULL_CYCLE);
  }
}

// the loop function runs over and over again forever
void loop() {
  if (sequenceCounter.m == 0) {
    // check emergency task
    taskNext = getHighPriorityTaskByInputs(gyroState, inputState);
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
      if ((patternNow == P_DONE) || (((patternNow == P_STANDGO) || (patternNow == P_GOFORWARD)) && (sensorsEnabled))) {
        // get next task
        taskNow = getNormalTaskByInputs(inputState, defaultTask);
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
        if (m_init.motorsCount == 10) {
          m_robotState.stepSteeringNow = setCenter(patternNow, getDirectionGyro(), m_robotState.centerMotorValueNow);
        }
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
        sensorsEnabled = false;
      }
      break;
      case P_ENABLEINPUTS:
      {
        sensorsEnabled = true;
      }
      break;
      case P_DOFLIP:
      {
        doFlipServo(20);
        setServo(calibrationData, m_init.defaultHight);
      }
      case P_REPEAT:
      case P_DONE:
      // do nothing
      // immediatelly run loop again
      break;
      case P_DODOWN:
      {
        // disable motors
        detachPrepareServo(calibrationData);
        detachServo();
      }
      break;
      case P_CRAWLSTART:
      {
        m_robotState.centerMotorValueNow = 50;
        m_robotState.legLiftNow = 100;
        m_robotState.inputDistanceNow = m_init.normalInputDistance / 2;
        m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
        m_robotState.robotStateNow = ROBOT_CRAWL;
        m_robotState.legHightNow = m_init.defaultHight;
        m_robotState.shiftCycleNow = m_init.shiftCycle;
        m_robotState.speedMuliplierNow = 2;
        m_robotState.timeDelayNow = m_init.timeDelayLong;
      }
      break;
      case P_SWIMSTART:
      {
        m_robotState.centerMotorValueNow = 50;
        m_robotState.legLiftNow = 100;
        m_robotState.inputDistanceNow = m_init.normalInputDistance / 2;
        m_robotState.halfCycleNow = 0;
        m_robotState.robotStateNow = ROBOT_SWIM;
        m_robotState.legHightNow = 150;
        m_robotState.shiftCycleNow = m_init.shiftCycle;
        m_robotState.speedMuliplierNow = 2;
        m_robotState.timeDelayNow = m_init.timeDelayLong * 2;
      }
      break;
      case P_INOSTART:
      {
        m_robotState.centerMotorValueNow = 0;
        m_robotState.legLiftNow = 80;
        m_robotState.inputDistanceNow = m_init.normalInputDistance;
        m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
        m_robotState.robotStateNow = ROBOT_INO;
        m_robotState.legHightNow = 100;
        m_robotState.shiftCycleNow = 8;
        m_robotState.speedMuliplierNow = 4;
        m_robotState.timeDelayNow = m_init.timeDelayLong * 5;
      }
      break;
      case P_NORMALSTART:
      {
        m_robotState.centerMotorValueNow = 0;
        m_robotState.legLiftNow = 40;
        m_robotState.inputDistanceNow = m_init.normalInputDistance;
        m_robotState.halfCycleNow = SERVO_HALF_CYCLE;
        m_robotState.robotStateNow = ROBOT_NORM;
        m_robotState.legHightNow = m_init.defaultHight;
        m_robotState.shiftCycleNow = m_init.shiftCycle;
        m_robotState.speedMuliplierNow = 2;
        m_robotState.timeDelayNow = m_init.timeDelayShort;
      }
      break;
      case P_SHORTDELAY:
      {
        m_robotState.timeDelayNow = m_init.timeDelayShort;
        m_robotState.shiftCycleNow = m_init.shiftCycle;
      }
      break;
      case P_LONGDELAY:
      {
        if (m_robotState.robotStateNow == ROBOT_INO) {
          m_robotState.timeDelayNow = m_init.timeDelayLong * 5;
          m_robotState.shiftCycleNow = 8;
        } else {
          m_robotState.timeDelayNow = m_init.timeDelayLong;
          m_robotState.shiftCycleNow = 0;
        }
      }
      break;
      default:
      {
        if (m_init.motorsCount == 10) {
          setCenter(patternNow, 0, m_robotState.centerMotorValueNow);
        }
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
  if (m_init.motorsCount == 10) {
    updateCenterServo(calibrationData, getValueCenter(sequenceCounter.m));
  }
  updateLegsServo(calibrationData, getWalkPatterns());
  doPWMServo(m_robotState.timeDelayNow);
  // update motor pattern point
  sequenceCounter = updateCountPatterns(m_robotState.shiftCycleNow, SERVO_FULL_CYCLE);
  // read proximity sensors
  inputState = updateInputs(sequenceCounter.m, sensorsEnabled, getDirectionGyro());
  // update gyro readings and ballance
  gyroState = updateGyro(sequenceCounter.m);
  updateBallanceServo(getStaticBallance(gyroState, sequenceCounter, getTouchInputs(), getWalkingMode()));
}
