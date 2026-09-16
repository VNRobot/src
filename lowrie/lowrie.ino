/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

#include <EEPROM.h>
#include <Servo.h>

// software version hardcoded. should be changed manually
#define ROBOT_VERSION           25
// input grounded 0 - 1023
#define INPUT_GROUNDED          400
// main time delay in ms. bigger the number slower the robot
#define TIME_DELAY              24
// low hight in mm. upper arm is horizontal
#define HIGHT_LOW               80
// normal hight mm
#define HIGHT_DEFAULT           110
// maximal hight mm
#define HIGHT_MAX               160
// lift value
#define LIFT_DEFAULT            40
// calibration angle
#define CALIBRATION_ANGLE_MIN   -15
#define CALIBRATION_ANGLE_MAX   15
// robot size devider
#define ROBOT_SIZE_DEVIDER      1
// leg lift point
#define LIFT_POINT              5
// step size mm
#define STEP_SIZE               50
// step margine
#define STEP_MARGINE            10
// maximal speed
#define SPEED_MAX               2
// lifted leg speed
#define LIFTED_LEG_SPEED        10

// input state
enum inState {
  IN_OBSTACLE_FRONT,
  IN_OBSTACLE_FRONTLEFT,
  IN_OBSTACLE_FRONTRIGHT,
  IN_OBSTACLE_LEFT,
  IN_OBSTACLE_RIGHT,
  IN_FAR_OBSTACLE_FRONT,
  IN_FAR_OBSTACLE_LEFT,
  IN_FAR_OBSTACLE_RIGHT,
  IN_NORMAL             
};
// input extra state
enum exState {
  EX_STEP_UP_SMALL,
  EX_STEP_UP_BIG,
  EX_STEP_DOWN_SMALL,
  EX_STEP_DOWN_BIG,
  EX_NORMAL
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
  Q_PROCESSED,
  Q_END
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
// robot state
enum rState {
  ROBOT_NORM,
  ROBOT_INO,
  ROBOT_CRAWL
};
// leg state
enum lState {
  LEG_LINEAR,
  LEG_BEFORE_LIFTING,
  LEG_LIFTING,
  LEG_LIFTED,
  LEG_LOWERING,
  LEG_AFTER_LOWERING
};
// leg location
enum legLoc {
  FRONT_LEG,
  REAR_LEG,
  WRONG_LEG
};

// structure for one leg data
typedef struct leg {
  short hight;
  short shift;
  unsigned char state;
  char speed;
  short targeth;
  short targets;
  unsigned char location;
} leg;
// legs motors structure
typedef struct allLegs {
  leg fl;
  leg fr;
  leg rl;
  leg rr;
} allLegs;
// structure for four legs
typedef struct quad {
  short fl;
  short fr;
  short rl;
  short rr;
  bool enabledF;
  bool enabledR;
} quad;
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

//---------------global variables---------------------------
// gyro state
accRoll m_gyroState = {0, 0, 0, 0, 0, 0, 0, 0, 0};
// leg values for 4 legs
allLegs m_legsValue = {HIGHT_DEFAULT, -STEP_SIZE, LEG_LINEAR, 0, HIGHT_DEFAULT, -STEP_SIZE - STEP_MARGINE, WRONG_LEG,
                       HIGHT_DEFAULT, -STEP_SIZE, LEG_LINEAR, 0, HIGHT_DEFAULT, -STEP_SIZE - STEP_MARGINE, WRONG_LEG,
                       HIGHT_DEFAULT,  STEP_SIZE, LEG_LINEAR, 0, HIGHT_DEFAULT, STEP_SIZE + STEP_MARGINE, WRONG_LEG,
                       HIGHT_DEFAULT,  STEP_SIZE, LEG_LINEAR, 0, HIGHT_DEFAULT, STEP_SIZE + STEP_MARGINE, WRONG_LEG};
//----------------------------------------------------------
// main counter
unsigned char mCounter = 0;
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

// quick and other patterns
void _doQuickAndOther(unsigned char patternNow) {
  switch (patternNow) {
    case Q_RESETGIRO:
    {
      resetGyro();
    }
    break;
    case Q_DOLOW:
    {
      setServo(HIGHT_LOW, HIGHT_LOW, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 20);
    }
    break;
    case Q_DOSTAND:
    {
      setServo(HIGHT_DEFAULT, HIGHT_DEFAULT, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 20);
    }
    break;
    case Q_DORECOVER:
    {
      setServoQuick(HIGHT_LOW, HIGHT_LOW, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 500);
      if (m_gyroState.aRollAverage < 0) {
        setServoQuick(HIGHT_LOW, HIGHT_LOW, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 500);
      } else {
        setServoQuick(HIGHT_LOW, HIGHT_LOW, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 500);
      }
      setServoQuick(HIGHT_LOW, HIGHT_LOW, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 500);
    }
    break;
    case Q_DOFLIP:
      // do nothing for now
    case Q_DORESET:
    {
    }
    break;
    case Q_REPEAT:
    case Q_DONE:
    case Q_PROCESSED:
    case Q_END:
    // do nothing
    // immediatelly run loop again
    break;
    case Q_DODOWN:
    {
      // disable motors
      setServo(HIGHT_LOW, HIGHT_LOW, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 20);
      detachServo();
      //detachCenter();
    }
    break;
    default:
      Serial.println(F("Wrong pattern"));
    break;
  }
}

// set motors and read sensors
void _doCycle(void) {
  // process legs movement
  if (getWalkingModeInTask()) {
    // set legs state
    updateLegsCounter();
    // set legs shift
    setWalkPatternsShiftCount();
    // set legs lift
    setLegsLiftCount(readSwitchesCount());
  }
  updateLegsServoCount();
  delay(TIME_DELAY);
  // runs only after delay
  // update motor pattern point
  mCounter = updateCounter();
  // update current readings
  updateCurrentCount(mCounter);
  // update gyro readings
  updateGyroCount(mCounter);
  // update sensor readings
  updateInputsCount(mCounter);
}

// runs once on boot or reset
void setup() {
  // Start serial for debugging
  Serial.begin(9600);
  Serial.println(F("Device started"));
  delay(200);
  // -------init shift------- 
  // short shiftForward, bool walk, bool ballance, bool rock
  initShift(0, true, false, false);
  // -------init leg lift------- 
  // short legHight, short legLift, bool sideBallance, bool compensation
  initLifts(HIGHT_DEFAULT, LIFT_DEFAULT, false, false);
  // check button press
  bool calibrationMode = m_getButtonPressed();
  unsigned char version = EEPROM.read(0);
  if (version != ROBOT_VERSION) {
    calibrationMode = true;
  }
  // -------init switches------- 
  // bool calibrationMode, bool swFrontEnable, bool swRearEnable
  initSwitches(calibrationMode, false, false);
  // -------init sensors inputs-------
  // bool calibrationMode, short legHight, bool sensorsEnabled, bool extraInputsEnabled
  initInputs(calibrationMode, HIGHT_DEFAULT, false, false);
  enableObstacleInputs(false);
  enableEdgeInputs(false);
  // -------attach legs servo-------
  attachServo();
  // -------init current readings-------
  // bool calibrationMode, bool extraEnabled
  initCurrent(calibrationMode, true);
  // init legs servo motors
  initServo(calibrationMode);
  if (calibrationMode) {
    delay(1000);
    // lift legs for gyro calibration
    setServo(HIGHT_LOW, HIGHT_LOW, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 20);
  }
  // -------init gyro-------
  initGyro(calibrationMode);
  delay(200);
  updateGyroCount(0);
  delay(20);
  resetGyro();
  delay(20);
  updateGyroCount(0);
  if (calibrationMode) {
    // write software version
    #ifdef BOARD_ESP32
      EEPROM.write(0, ROBOT_VERSION);
      EEPROM.commit();
    #else
      EEPROM.update(0, ROBOT_VERSION);
    #endif
    // disable motors
    detachServo();
    Serial.println(F(" Calibration complete. Please restart now"));
    delay(20000);
  }
  delay(200);
  setServo(HIGHT_DEFAULT, HIGHT_DEFAULT, - STEP_SIZE - STEP_MARGINE, STEP_SIZE + STEP_MARGINE, 20);
  // update current readings
  updateCurrentCount(0);
  // read proximity sensors
  updateInputsCount(0);
  // explore mode
  Serial.println(F("Entering explore mode"));
  // -------init tasks-------
  initTasks();
  // load task and pattern. direction is 0
  // -------init path-------
  // short stepSize, short speed, bool turning, bool counting
  initPath(STEP_SIZE, SPEED_MAX, false, false);
  setDistancePath(100); // cm
  updatePath(0);
  // -------init counter-------
  // short mainCycle, char timeShift
  initCounter(32);
  // start counter
  mCounter = updateCounter();
  delay(200);
}

// the loop function runs over and over again forever
void loop() {
  if (mCounter == 0) {
    // set new pattern and task
    setPatternAndTask(getCurrentState(), getGyroState());
    // get pattern
    unsigned char patternNow = getPatternOfTask();
    if (patternNow == P_STANDGO) {
      // normal walking to avoid obstacles
      // get and set new direction
      setDirectionGyro(calculateNewDirectionPath(getInputState(), getWallAngleInputs(), getDirectionGyro()));
      // update path
      updatePath(getDirectionGyro());
      _doCycle();
    } else {
      // quick and non walking patterns
      _doQuickAndOther(patternNow);
    }
  } else {
    // cycle in the middle of pattern
    _doCycle();
  }
}
