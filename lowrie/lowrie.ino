/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

#include <EEPROM.h>
#include <Servo.h>

// software version hardcoded. should be changed manually
#define ROBOT_VERSION           11
// input grounded 0 - 1023
#define INPUT_GROUNDED          400
// main time delay in ms. bigger the number slower the robot
#define TIME_DELAY              10
// low hight in mm. upper arm is horizontal
#define HIGHT_LOW               80
// normal hight
#define HIGHT_DEFAULT           130
// maximal hight
#define HIGHT_MAX               170
// normal leg lift in mm
#define LEG_LIFT                40
// main servo pattern counter end
#define MAIN_FULL_CYCLE         36
#define MAIN_HALF_CYCLE         18
#define SERVO_FULL_CYCLE        72
#define SERVO_HALF_CYCLE        36
#define SERVO_PAIR_SHIFT        18  // used for ino
// calibration angle
#define CALIBRATION_ANGLE_MIN   -10
#define CALIBRATION_ANGLE_MAX   10

// input state
enum inState {
  IN_OBSTACLE_FRONT,
  IN_OBSTACLE_FRONTLEFT,
  IN_OBSTACLE_FRONTRIGHT,
  IN_OBSTACLE_LEFT,
  IN_OBSTACLE_RIGHT,
  IN_FAR_OBSTACLE_LEFT,
  IN_FAR_OBSTACLE_RIGHT,
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
  Q_DOLOW,
  Q_DODOWN,
  Q_DOSTAND,
  Q_DORESET,
  Q_DORECOVER,
  Q_DOFLIP,
  Q_DONE,
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
  STANDGO_TASK,
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
  ROBOT_INO,
  ROBOT_CRAWL
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
  short aRollNow;              // relative roll  now    
  short aPitchNow;             // relative pitch now
  short aRollAverage;          // roll       right - positive   -90 0 90 upsidedown also 0
  short aPitchAverage;         // pitch      up - positive   -90 0 90 upsidedown also 0
  short aUpsideAverage;        // z          upside down - negative
  short aLiftFL;               // dynamic ballance when leg is lifted
  short aLiftFR;               // dynamic ballance when leg is lifted
  short aLiftRL;               // dynamic ballance when leg is lifted
  short aLiftRR;               // dynamic ballance when leg is lifted
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
  unsigned char robotStateNow;
  short legHightNow;
  short legLiftNow;
  char speedMuliplierNow;
  char speedLNow;
  char speedRNow;
  bool forwardNow;
  bool walkingModeNow;
} robotState;

//---------------global variables---------------------------
// sequence counters
phase m_sequenceCounter = {0, 0, 0, 0, 0};
// robot state
robotState m_robotState = {
  ROBOT_NORM,              // unsigned char robotStateNow;
  HIGHT_DEFAULT,           // short legHightNow;
  LEG_LIFT,                // short legLiftNow;
  2,                       // char speedMuliplierNow; 1 or 2
  0,                       // speedLNow
  0,                       // speedRNow
  true,                    // forwardNow
  true                     // walkingModeNow
};
// gyro state
accRoll m_gyroState = {0, 0, 0, 0, 0, 0, 0, 0, 0};
// leg values for 4 legs
allLegs m_legsValue = {125, 0, false, 125, 0, false, 125, 0, false, 125, 0, false};
// ballance correction
allLegs m_legCorrect = {0, 0, false, 0, 0, false, 0, 0, false, 0, 0, false};
//----------------------------------------------------------
// servo cycle is done flag
bool m_cycleDone = true;
// default task from rTasks
unsigned char m_defaultTask = STANDGO_TASK;
// next task
unsigned char m_taskPriority = PRIORITY_LOW;
// variable for temporary use
unsigned char i;

// check button pressed
bool m_getButtonPressed(void) {
    bool modeButtonPressed = false;
    if (analogRead(A6) < INPUT_GROUNDED) {
    modeButtonPressed = true;
    // block until button released
    while (analogRead(A6) < INPUT_GROUNDED) {
      delay(100);
    }
  }
  return modeButtonPressed;
}

// true if software version from eeprom is right
bool _rightSoftwareVersionEeprom() {
  unsigned char version = EEPROM.read(0);
  // software version address is 0
  if (version == ROBOT_VERSION) {
    return true;
  } else {
    return false;
  }
}

// write software version to eeprom
void _writeSoftwareVersionEeprom() {
#ifdef BOARD_ESP32
  EEPROM.write(0, ROBOT_VERSION);
  EEPROM.commit();
#else
  EEPROM.update(0, ROBOT_VERSION);
#endif
}

// runs once on boot or reset
void setup() {
  // Start serial for debugging
  Serial.begin(9600);
  Serial.println(F("Device started"));
  delay(200);
  // check button press
  bool calibrationMode = m_getButtonPressed();
  if (!_rightSoftwareVersionEeprom()) {
    calibrationMode = true;
  }
  // init digital sensors
  initInputs(calibrationMode);
  // init current readings
  initCurrent(calibrationMode);
  // init legs servo motors
  initServo(calibrationMode);
  // lift legs for gyro calibration
  if (calibrationMode) {
    delay(1000);
    setFlippedGyro(true);
    setFlippedServo(-1, -1);
    setServo(HIGHT_MAX, HIGHT_MAX, 20);
  }
  // init gyro
  initGyro(calibrationMode);
  delay(200);
  updateGyroCount(0);
  delay(20);
  resetGyro();
  delay(20);
  updateGyroCount(0);
  // disable motors
  if (calibrationMode) {
    // write software version
    _writeSoftwareVersionEeprom();
    detachServo();
    delay(20000);
  }
  delay(200);
  setServo(HIGHT_DEFAULT, HIGHT_DEFAULT, 20);
  // update current readings
  updateCurrentCount(0);
  // read proximity sensors
  updateInputState(true, 0, false);
  // explore mode
  Serial.println(F("Entering explore mode"));
  applyTask(BEGIN_TASK);
  // load task and pattern. direction is 0
  updatePath(0);
  updatePatternsCount();
  // set distance to target
  setDistancePath(100);
}

// set motors and read sensors
void _doCycle(void) {
  // update servo motors values, move motors
  setWalkPatternsCount(getWalkingModeInTask());
  updateLegsServoCount();
  delay(TIME_DELAY);
  m_cycleDone = true;
}

// the loop function runs over and over again forever
void loop() {
  if (m_cycleDone) {
    // runs only after delay
    m_cycleDone = false;
    // update motor pattern point
    updatePatternsCount();
    // update current readings
    updateCurrentCount(m_sequenceCounter.m);
    // update gyro readings
    updateGyroCount(m_sequenceCounter.m);
    // update sensor readings
    updateInputsCount(m_sequenceCounter.m);
    // once in a pattern after delay
    if (m_sequenceCounter.m == 0) {
      // process proximity sensors
      updateInputState(getSurfaceFlatGyro(), getDirectionGyro(), false);
    }
    // update ballance
    updateBallanceCount();
    updateBallanceServoCount();
  }
  // once in a pattern
  if (m_sequenceCounter.m == 0) {
    // set new task and next pattern
    setPatternAndTask(m_defaultTask, getCurrentState(), getGyroState(), getDirectionGyro(), m_taskPriority);
    switch (getPatternOfTask()) {
      case Q_RESETGIRO:
      {
        resetGyro();
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
        if (m_gyroState.aRollAverage < 0) {
          setFlippedServo(1, -1);
          setServo(HIGHT_MAX, HIGHT_MAX, 0);
          setFlippedServo(-1, 1);
          setServo(HIGHT_MAX, HIGHT_MAX, 0);
        } else {
          setFlippedServo(-1, 1);
          setServo(HIGHT_MAX, HIGHT_MAX, 0);
          setFlippedServo(1, -1);
          setServo(HIGHT_MAX, HIGHT_MAX, 0);
        }
        setServo(HIGHT_LOW, HIGHT_LOW, 0);
        setFlippedGyro(false);
        setFlippedServo(1, 1);
        setServo(HIGHT_LOW, HIGHT_LOW, 0);
      }
      break;
      case Q_DOFLIP:
        // do nothing for now
      case Q_DORESET:
      {
        if (m_gyroState.aUpsideAverage < 0) {
          setFlippedGyro(true);
          setFlippedServo(-1, -1);
        } else {
          setFlippedGyro(false);
          setFlippedServo(1, 1);
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
        m_taskPriority = PRIORITY_HIGH;
      }
      break;
      case Q_SETPRIORITY_NORM:
      {
        m_taskPriority = PRIORITY_NORM;
      }
      break;
      case Q_SETPRIORITY_LOW:
      {
        m_taskPriority = PRIORITY_LOW;
      }
      break;
      default:
      {
        // P_STANDGO
        // getWallAngleInputs() getInputState() getCurrentState() getDirectionGyro() getGyroState()
        //
        // set speed and direction
        updatePath(getDirectionGyro());
        _doCycle();
      }
      break;
    }
  } else {
    // cycle in the middle of pattern
    _doCycle();
  }
}
