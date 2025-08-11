/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

#include <Servo.h>

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
  P_DONE,
  P_RESETDIRECTION,
  P_RESTOREDIRECTION,
  P_RESETGIRO,
  P_ENABLEINPUTS,
  P_DISABLEINPUTS,
  P_STANDGOSHIFTLEFT,
  P_STANDGOSHIFTRIGHT,
  P_GOSHIFTLEFT,
  P_GOSHIFTRIGHT,
  P_DODOWN,
  P_RECOVERLEFT,
  P_RECOVERRIGHT,
  P_SHORTDELAY,
  P_LONGDELAY,
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
// pin numbers for servo motors
enum dPinsServo {
  HL1_MOTOR = 2,
  HL2_MOTOR = 3,
  HR1_MOTOR = 4,
  HR2_MOTOR = 5,
  FL1_MOTOR = 6,
  FL2_MOTOR = 7,
  FR1_MOTOR = 8,
  FR2_MOTOR = 9,
  RL1_MOTOR = 10,
  RL2_MOTOR = 11,
  RR1_MOTOR = 12,
  RR2_MOTOR = 13
};
// init servo library
Servo m_servo_hl_1;
Servo m_servo_hl_2;
Servo m_servo_hr_1;
Servo m_servo_hr_2;
Servo m_servo_fl_1;
Servo m_servo_fl_2;
Servo m_servo_fr_1;
Servo m_servo_fr_2;
Servo m_servo_rl_1;
Servo m_servo_rl_2;
Servo m_servo_rr_1;
Servo m_servo_rr_2;
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
// leg timing phase. main RR
struct phase {
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
  unsigned char versionEeprom; // software version hardcoded. should be changed manually
  unsigned char motorsCount; // motors count
  unsigned char fullCycle; // main pattern counter
  unsigned char halfCycle;
  unsigned char shiftCycle;
  unsigned char timeDelayShort;
  unsigned char timeDelayLong;
  unsigned short maxInputCurrent; // maximal pair of legs current
  unsigned char normalInputDistance; // normal distance sensor beam to ground
  short defaultHight; // default height in mm
  char forwardCenterBallance; // center position in the leg forward shift // bigger the number more weight on front
  char speedMuliplier; // walking speed multipier max = 3
  bool rollBallanceEnabled; // roll ballance enable flag
  bool pitchBallanceEnabled; // pitch ballance enable flag
  bool forwardBallanceEnabled; // forward ballance enable flag
  bool touchBallanceEnabled; // touch enable flag
  bool sensorsInputsEnabled; // sensors enabled flag
  bool centerMotorsEnabled; // center mototor enabled
  bool stepSteeringEnabled; // step steering enabled
} robotSetup;

// motors calibration values for optional 12 motors
allMotors calibrationData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// inputs state defined in inState
unsigned char inputState = IN_NORMAL;
// gyro state
accRoll gyroState;
// sequence counter 0 to m_fullCycle - 1
phase sequenceCounter = {0, 0, 0, 0, 0, 0};
// current pattern defined in rPatterns
unsigned char patternNow = P_DOSTAND;
// enable sensors flag toggled by P_ENABLEINPUTS and P_DISABLEINPUTS
bool sensorsEnabled = false;
// default task from rTasks
unsigned char defaultTask = GO_TASK;
// current task
unsigned char taskNow = STAND_TASK;
// next task
unsigned char taskNext = STAND_TASK;
// variable for temporary use
unsigned char i;
//-------------global variables---------------------------
// init data structure
// 10 motors small robot
//        m_init.    versionEeprom motorsCount fullCycle halfCycle shiftCycle timeDelayShort timeDelayLong maxInputCurrent normalInputDistance defaultHight forwardCenterBallance speedMuliplier rollBallanceEnabled pitchBallanceEnabled forwardBallanceEnabled touchBallanceEnabled sensorsInputsEnabled centerMotorsEnabled stepSteeringEnabled;
//robotSetup m_init = {54,           10,         36,       18,       0,         8,             8,            1500,           50,                 125,         0,                    2,             false,              false,               false,                 false,               true,                true,               false};
// 12 motors small robot
//        m_init.    versionEeprom motorsCount fullCycle halfCycle shiftCycle timeDelayShort timeDelayLong maxInputCurrent normalInputDistance defaultHight forwardCenterBallance speedMuliplier rollBallanceEnabled pitchBallanceEnabled forwardBallanceEnabled touchBallanceEnabled sensorsInputsEnabled centerMotorsEnabled stepSteeringEnabled;
robotSetup m_init = {54,           12,         36,       18,       4,         14,             16,           1800,           42,                 125,         0,                    2,             false,              false,               false,                 false,               true,                false,              true};
// 8 motors big robot
//        m_init.    versionEeprom motorsCount fullCycle halfCycle shiftCycle timeDelayShort timeDelayLong maxInputCurrent normalInputDistance defaultHight forwardCenterBallance speedMuliplier rollBallanceEnabled pitchBallanceEnabled forwardBallanceEnabled touchBallanceEnabled sensorsInputsEnabled centerMotorsEnabled stepSteeringEnabled;
//robotSetup m_init = {54,            8,         36,       18,       0,         20,            20,           2500,           50,                 125,         0,                    2,             false,              false,               false,                 false,               false,               false,              true};
// 10 motors big robot
//        m_init.    versionEeprom motorsCount fullCycle halfCycle shiftCycle timeDelayShort timeDelayLong maxInputCurrent normalInputDistance defaultHight forwardCenterBallance speedMuliplier rollBallanceEnabled pitchBallanceEnabled forwardBallanceEnabled touchBallanceEnabled sensorsInputsEnabled centerMotorsEnabled stepSteeringEnabled;
//robotSetup m_init = {54,           10,         36,       18,       0,         20,            20,           2500,           50,                 125,         5,                    2,             false,              false,               false,                 false,               true,                true,               false};
// 12 motors big robot
//        m_init.    versionEeprom motorsCount fullCycle halfCycle shiftCycle timeDelayShort timeDelayLong maxInputCurrent normalInputDistance defaultHight forwardCenterBallance speedMuliplier rollBallanceEnabled pitchBallanceEnabled forwardBallanceEnabled touchBallanceEnabled sensorsInputsEnabled centerMotorsEnabled stepSteeringEnabled;
//robotSetup m_init = {54,           12,         36,       18,       4,         30,            60,           2500,           50,                 125,         10,                   2,             false,              false,               false,                 false,               true,                false,              true};
//----------------------------------------------------------
// main time delay in the loop in msec
unsigned char timeDelay = m_init.timeDelayShort;
unsigned char legShift = m_init.shiftCycle;
unsigned char legShiftTemp = legShift;

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
  // check for Mode button press or if not calibrated
  if (_readButtonPress() || (m_init.versionEeprom != readSoftwareVersionEeprom())) {
    // factory mode is used for legs calibration
    Serial.println(F("Entering factory mode"));
    // do calibration
    if (doCalibration(& calibrationData)) {
      writeCalibrationEeprom(calibrationData);
      writeSoftwareVersionEeprom(m_init.versionEeprom);
      delay(6000);
    }
  } else {
    // read values by using pointer to struct
    readCalibrationEeprom(& calibrationData);
    // init servo motors hight in mm
    initServo(calibrationData, 70, 0);
    delay(200);
    setServo(calibrationData, m_init.defaultHight, m_init.forwardCenterBallance);
    delay(200);
    // init digital sensors
    initInputs();
    // update inputs direction is 0
    updateInputs(sequenceCounter.rr, sensorsEnabled, 0);
    // init gyro MPU6050 using I2C
    initGyro();
    delay(200);
    resetGyro();
    delay(20);
    // explore mode
    Serial.println(F("Entering explore mode"));
    applyTask(BEGIN_TASK);
    // update gyro readings
    gyroState = updateGyro(sequenceCounter.rr);
    // load task and pattern. direction is 0
    if (m_init.centerMotorsEnabled) {
      setCenter(patternNow, 0);
    }
    setPattern(patternNow, 0);
    sequenceCounter = updateCountPatterns(legShiftTemp);
  }
}

// the loop function runs over and over again forever
void loop() {
  if (sequenceCounter.rr == 0) {
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
        if (m_init.centerMotorsEnabled) {
          setCenter(patternNow, getDirectionCorrectionGyro());
        }
        setPattern(patternNow, getDirectionCorrectionGyro());
        updateLegShift();
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
      case P_DONE:
      // do nothing
      // immediatelly run loop again
      break;
      case P_DODOWN:
      {
        // disable motors
        detachServo(calibrationData);
      }
      break;
      case P_SHORTDELAY:
      {
        timeDelay = m_init.timeDelayShort;
        legShift = m_init.shiftCycle;
      }
      break;
      case P_LONGDELAY:
      {
        timeDelay = m_init.timeDelayLong;
        legShift = 0;
      }
      break;
      default:
      {
        if (m_init.centerMotorsEnabled) {
          setCenter(patternNow, 0);
        }
        setPattern(patternNow, 0);
        updateLegShift();
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
  if (m_init.centerMotorsEnabled) {
    updateCenterServo(calibrationData, getValueCenter(sequenceCounter.rr));
  }
  updateLegsServo(calibrationData, getWalkPatterns());
  delay(timeDelay);
  // update motor pattern point
  sequenceCounter = updateCountPatterns(legShiftTemp);
  // read proximity sensors
  inputState = updateInputs(sequenceCounter.rr, sensorsEnabled, getDirectionGyro());
  // update gyro readings and ballance
  updateBallanceServo(getStaticBallance(updateGyro(sequenceCounter.rr), sequenceCounter, getTouchInputs(), getWalkingMode()), m_init.forwardCenterBallance);
}

// update leg shift
void updateLegShift(void) {
  if (legShiftTemp < legShift) {
    legShiftTemp ++;
  } else if (legShiftTemp > legShift) {
    legShiftTemp --;
  }
}