/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

#include <EEPROM.h>
#include <Servo.h>

// software version hardcoded. should be changed manually
#define ROBOT_VERSION           23
// input grounded 0 - 1023
#define INPUT_GROUNDED          400
// main time delay in ms. bigger the number slower the robot
#define TIME_DELAY              24
// low hight in mm. upper arm is horizontal
#define HIGHT_LOW               80
// normal hight
#define HIGHT_DEFAULT           130
// maximal hight
#define HIGHT_MAX               160
// servo counters
#define SERVO_FULL_CYCLE        18
#define SERVO_HALF_CYCLE        9
// calibration angle
#define CALIBRATION_ANGLE_MIN   -15
#define CALIBRATION_ANGLE_MAX   15
// robot size devider
#define ROBOT_SIZE_DEVIDER      2
// state counter
#define STATE_COUNTER           4

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
  ROBOT_INO
};
// structure for one leg data
struct leg {
  short hight;
  short shift;
  bool lifted;
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
// leg switch 
struct switches {
  unsigned char fl;
  unsigned char fr;
  unsigned char rl;
  unsigned char rr;
};

//---------------global variables---------------------------
// gyro state
accRoll m_gyroState = {0, 0, 0, 0, 0, 0, 0, 0, 0};
// leg values for 4 legs
allLegs m_legsValue = {125, 0, false, 125, 0, false, 125, 0, false, 125, 0, false};
// switches data
switches m_swState = {1, 1, 1, 1};
switches m_swCount = {0, 0, 0, 0};
//----------------------------------------------------------
// main counter
unsigned char mCounter = 0;
// state counter
unsigned char stateCounter = 0;
// variable for temporary use
unsigned char i;
// time delay
unsigned char timeDelay = TIME_DELAY;

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
      setServo(HIGHT_LOW, HIGHT_LOW, 20);
    }
    break;
    case Q_DOSTAND:
    {
      setServo(HIGHT_DEFAULT, HIGHT_DEFAULT, 20);
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
    case Q_PROCESSED:
    case Q_END:
    // do nothing
    // immediatelly run loop again
    break;
    case Q_DODOWN:
    {
      // disable motors
      setServo(HIGHT_LOW, HIGHT_LOW, 20);
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
  // update servo motors values, move motors
  readSwitchesCount(mCounter);
  timeDelay = setWalkPatternsCount(getWalkingModeInTask(), getspeedLPath(), getspeedRPath());
  updateLegsServoCount();
  delay(timeDelay);
  // runs only after delay
  // update motor pattern point
  mCounter = updatePatternsCount();
  // update current readings
  updateCurrentCount(mCounter);
  // update gyro readings
  updateGyroCount(mCounter);
  // update sensor readings
  updateInputsCount(mCounter);
  // update center motors
  //updateCenterCount(getspeedLPath(), getspeedRPath());
}

// set robot state
void _setState(unsigned char newState) {
  setStatePattern(newState);
  setStatePath(newState);
  setStateInputs(newState);
  //Serial.print(F(" State "));
  switch (newState) {
    case ROBOT_NORM:
    {
      //Serial.println("ROBOT_NORM");
    }
    break;
    case ROBOT_INO:
    {
      //Serial.println("ROBOT_INO");
    }
    break;
    default:
    break;
  }
}

// runs once on boot or reset
void setup() {
  // Start serial for debugging
  Serial.begin(9600);
  Serial.println(F("Device started"));
  delay(200);
  // set features
  enableBallancePatterns(false);
  enableExtraCurrent(true);
  enableSensorInputs(true);
  enableExtraInputs(false);
  enableEdgeInputs(false);
  enableTurningPath(true);
  enableCountingPath(false);
  // check button press
  bool calibrationMode = m_getButtonPressed();
  unsigned char version = EEPROM.read(0);
  if (version != ROBOT_VERSION) {
    calibrationMode = true;
  }
  // init switches
  initSwitches(calibrationMode);
  // init sensors
  initInputs(calibrationMode);
  // attach center servo
  //attachCenter();
  // attach servo
  attachServo();
  // init current readings
  initCurrent(calibrationMode);
  // init center servo motors
  //initCenter(calibrationMode);
  // init legs servo motors
  initServo(calibrationMode);
  if (calibrationMode) {
    delay(1000);
    // lift legs for gyro calibration
    setFlippedGyro(true);
    setFlippedServo(-1, -1);
    //setCenter(20);
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
    //detachCenter();
    Serial.println(F(" Calibration complete. Please restart now"));
    delay(20000);
  }
  delay(200);
  //setCenter(0);
  setServo(HIGHT_DEFAULT, HIGHT_DEFAULT, 20);
  // update current readings
  updateCurrentCount(0);
  // read proximity sensors
  updateInputsCount(0);
  // explore mode
  Serial.println(F("Entering explore mode"));
  initTasks();
  // load task and pattern. direction is 0
  updatePath(0);
  mCounter = updatePatternsCount();
  // set distance to target cm
  setDistancePath(100);
  // set state
  _setState(ROBOT_NORM);
}

// the loop function runs over and over again forever
void loop() {
  if (mCounter == 0) {
    // once in a pattern
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
      // check for robot state
      if (!getSurfaceFlatGyro() || (getExtraInputState() == EX_STEP_UP_BIG) || (getExtraInputState() == EX_STEP_DOWN_BIG)) {
        stateCounter = 0; //STATE_COUNTER * 2;
      } else if (getSurfaceBumpyGyro() || (getExtraInputState() == EX_STEP_UP_SMALL) || (getExtraInputState() == EX_STEP_DOWN_SMALL)) {
        if (stateCounter < STATE_COUNTER) {
          stateCounter = 0; //STATE_COUNTER;
        }
      }
      // set state
      if (stateCounter == 0) {
        _setState(ROBOT_NORM);
        //setDirectionCenter(getDirectionGyro());
      } else {
        _setState(ROBOT_INO);
        stateCounter --;
      }
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
