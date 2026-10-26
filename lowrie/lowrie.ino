/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

#include <EEPROM.h>
#include <Servo.h>
#include <Wire.h>

// low hight in mm. upper arm is horizontal
#define SERVO_HIGHT_LOW               80
// normal hight
#define SERVO_HIGHT_DEFAULT           120
// maximal hight
#define SERVO_HIGHT_MAX               160
// robot size devider
#define MAIN_ROBOT_SIZE_DEVIDER       2

// pin numbers for servo motors
enum dPinsServo {
  EX1_MOTOR = 2,
  EX2_MOTOR = 3,
  EX3_MOTOR = 4,
  EX4_MOTOR = 5,
  FL1_MOTOR = 6,
  FL2_MOTOR = 7,
  FR1_MOTOR = 8,
  FR2_MOTOR = 9,
  RL1_MOTOR = 10,
  RL2_MOTOR = 11,
  RR1_MOTOR = 12,
  RR2_MOTOR = 13
};
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
  LEG_LIFTED_BEFORE,
  LEG_LIFTED_AFTER,
  LEG_LOWERING,
  LEG_AFTER_LOWERING
};
// structure for one leg data
typedef struct leg {
  short hight;
  short shift;
  char state;
  char count;
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
// structure for center motor
typedef struct centers {
  short front;
  short rear;
} centers;
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
// main data structure
typedef struct masterData {
  char robotVersion;           // software version
  bool masterDevice;           // master device flag
  char mainCounter;            // main counter
  char patternNow;             // current pattern
  char speedLeft;              // left side speed
  char speedRight;             // right side speed
  char currentState;           // current received from other device
  char shiftForward;           // shift forward ballance
  char directionCorrection;        // direction correction
} masterData;

// software components
typedef struct softwareComponents {
  bool center;
  bool current;
  bool gyro;
  bool inputs;
  bool serial;
  bool servo;
  bool switches;
  bool wheels;
  bool generic;
} softwareComponents;

//---------------servo motors-------------------------------

// init servo library
Servo servo_ex_1;
Servo servo_ex_2;
Servo servo_ex_3;
Servo servo_ex_4;
Servo servo_fl_1;
Servo servo_fl_2;
Servo servo_fr_1;
Servo servo_fr_2;
Servo servo_rl_1;
Servo servo_rl_2;
Servo servo_rr_1;
Servo servo_rr_2;

//---------------global variables---------------------------
// center motor angle
centers m_centerAngle = {0, 0};
// gyro state
accRoll m_gyroState = {0, 0, 0, 0, 0, 0, 0, 0, 0};
// leg values for 4 legs
allLegs m_legsValue = {SERVO_HIGHT_LOW, 0, LEG_LINEAR, 0,
                       SERVO_HIGHT_LOW, 0, LEG_LINEAR, 0,
                       SERVO_HIGHT_LOW, 0, LEG_LINEAR, 0,
                       SERVO_HIGHT_LOW, 0, LEG_LINEAR, 0};
// master data
masterData m_mainData {
  25,                 //char robotVersion;           // software version
  true,               //bool masterDevice;           // master device flag
  0,                  //char mainCounter;            // main counter
  P_STANDGO,          //char patternNow;             // current pattern
  0,                  //char speedLeft;              // left side speed
  0,                  //char speedRight;             // right side speed
  C_NORMAL,           //char currentState;           // current received from other device
  0,                  //char shiftForward;           // shift forward ballance
  0                   //char directionCorrection;    // direction correction
};
//----------------------------------------------------------
// components
softwareComponents component {
  false,              //bool center;
  true,               //bool current;
  true,               //bool gyro;
  true,               //bool inputs;
  false,              //bool serial;
  true,               //bool servo;
  false,              //bool switches;
  false,              //bool wheels;
  true                //bool generic;
};
// variable for temporary use
int i;

// check button pressed
bool m_getButtonPressed(void) {
    bool modeButtonPressed = false;
    if (analogRead(A6) < 400) {
    modeButtonPressed = true;
    // block until button released
    while (analogRead(A6) < 400) {
      delay(100);
    }
  }
  return modeButtonPressed;
}

// quick and other patterns
void _doQuickAndOther(char patternNow) {
  switch (patternNow) {
    case Q_RESETGIRO:
    {
      resetGyro();
    }
    break;
    case Q_DOLOW:
    {
      setServo(SERVO_HIGHT_LOW, SERVO_HIGHT_LOW, 20);
    }
    break;
    case Q_DOSTAND:
    {
      setServo(SERVO_HIGHT_DEFAULT, SERVO_HIGHT_DEFAULT, 20);
    }
    break;
    case Q_DORECOVER:
    {
      //setServoQuick(SERVO_HIGHT_LOW, SERVO_HIGHT_LOW, 500);
      if (m_gyroState.aRollAverage < 0) {
        //setFlippedServo(1, -1);
        //setServoQuick(SERVO_HIGHT_MAX, SERVO_HIGHT_MAX, 500);
        //setFlippedServo(-1, 1);
        //setServoQuick(SERVO_HIGHT_MAX, SERVO_HIGHT_MAX, 500);
      } else {
        //setFlippedServo(-1, 1);
        //setServoQuick(SERVO_HIGHT_MAX, SERVO_HIGHT_MAX, 500);
        //setFlippedServo(1, -1);
        //setServoQuick(SERVO_HIGHT_MAX, SERVO_HIGHT_MAX, 500);
      }
      //setServoQuick(SERVO_HIGHT_LOW, SERVO_HIGHT_LOW, 500);
      setFlippedGyro(false);
      //setFlippedServo(1, 1);
      setServo(SERVO_HIGHT_DEFAULT, SERVO_HIGHT_DEFAULT, 20);
    }
    break;
    case Q_DOFLIP:
      // do nothing for now
    case Q_DORESET:
    {
      if (m_gyroState.aUpsideAverage < 0) {
        setFlippedGyro(true);
        //setFlippedServo(-1, -1);
      } else {
        setFlippedGyro(false);
        //setFlippedServo(1, 1);
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
      setServo(SERVO_HIGHT_LOW, SERVO_HIGHT_LOW, 20);
      detachServo();
      detachCenter();
      detachWheels();
    }
    break;
    default:
    break;
  }
}

// set motors and read sensors
void _doCycle(void) {
  // forward ballance
  m_mainData.shiftForward = setBallanceShiftCount(m_mainData.masterDevice, m_mainData.shiftForward);
  // set legs shift
  setWalkShiftCount(getWalkingModeInTask());
  // === sets m_legsValue.xx.shift
  // set legs lift
  setWalkLiftsCount(getWalkingModeInTask(), m_centerAngle.front);
  // === sets m_legsValue.xx.hight
  updateLegsServoCount();
  // === motors set ===
  // do communication
  if (m_mainData.mainCounter == 0) {
    // send once at the beginning of the pattern
    sendSerial();
  }
  doSleepHalfTime();
  // try to receive in every count
  receiveSerial();
  doSleepHalfTime();
  // runs only after delay
  // update motor pattern point
  m_mainData.mainCounter = updateCounter(getWalkingModeInTask(), m_mainData.masterDevice);
  // === sets m_legsValue.xx.count
  // === sets m_legsValue.xx.state
  // update current readings
  updateCurrentCount(m_mainData.mainCounter);
  // update gyro readings
  updateGyroCount(m_mainData.mainCounter);
  // === sets m_gyroState
  // update sensor readings
  updateInputsCount(m_mainData.mainCounter);
  // update center motors
  updateCenterCount();
}

// attach components
void _attachComponents(void) {
  // short liftPoint
  attachCounter();
  if (component.generic) {
    attachTasks();
    // short sizeDevider, bool turning, bool counting
    attachPath(MAIN_ROBOT_SIZE_DEVIDER, true, false);
  }
  if (component.center) {
    // bool frontE, bool rearE
    attachCenter(true, true);
  }
  if (component.current) {
    // bool extraEnabled
    attachCurrent(true);
  }
  if (component.gyro) {
    attachGyro();
  }
  if (component.inputs) {
    // short legHight, short sizeDevider, bool sensorsEnabled, bool extraInputsEnabled
    attachInputs(SERVO_HIGHT_DEFAULT, MAIN_ROBOT_SIZE_DEVIDER, true, false);
  }
  if (component.serial) {
    attachSerial();
  }
  if (component.servo) {
    // short legHight, bool sideBallance
    attachLifts(SERVO_HIGHT_DEFAULT, false);
    // short quickShiftMultiplier, bool walk, bool ballance
    attachShift(getQuickShiftMuliplierCounter(), true, false);
    // bool frontE, bool rearE, bool extraE
    attachServo(true, true, true);
  }
  if (component.switches) {
    // bool swFrontEnable, bool swRearEnable
    attachSwitches(true, true);
  }
  if (component.wheels) {
    //char wSpeed
    attachWheels(6);
  }
}

// init components
void _initComponents(bool calibrationMode) {
  // init counter
  initCounter();
  // init shift 
  initShift();
  // init lifts
  initLifts();
  // init sensors inputs
  initInputs(calibrationMode);
  enableObstacleInputs(true);
  enableEdgeInputs(false);
  // init switches inputs
  initSwitches(calibrationMode);
  // bool calibrationMode, bool extraEnabled
  initCurrent(calibrationMode);
  // init center servo motors
  initCenter(calibrationMode);
  // init Wheels servo motors
  initWheels(calibrationMode);
  // init legs servo motors
  initServo(calibrationMode);
  if (calibrationMode) {
    delay(1000);
    // lift legs for gyro calibration
    //setFlippedGyro(true);
    //setFlippedServo(-1, -1);
    setCenter(0);
    setServo(SERVO_HIGHT_LOW, SERVO_HIGHT_LOW, 20);
    delay(1000);
  }
  // init gyro
  initGyro(calibrationMode);
  // === sets m_gyroState
  if (calibrationMode) {
    // write software version
    EEPROM.update(0, m_mainData.robotVersion);
    // disable motors
    detachServo();
    detachCenter();
    detachWheels();
    delay(20000);
  } else {
    // explore mode
    // init tasks
    initTasks();
    // init path
    initPath();
    setDistancePath(100); // cm
    updatePath(0);
    // === sets m_legsValue.xx.speed
    initSerial();
  }
}

// set robot state
void _setState(char newState) {
  setServoState(newState);
  switch (newState) {
    case ROBOT_NORM:
    {
    }
    break;
    case ROBOT_INO:
    {
    }
    break;
    case ROBOT_CRAWL:
    {
    }
    break;
    default:
    break;
  }
}

// runs once on boot or reset
void setup() {
  // Start serial for debugging
  delay(200);
  Serial.begin(9600);
  delay(500);
  // Start wire
  Wire.begin();                      
  delay(100);
  // attach software components
  _attachComponents();
  // check button press
  bool calibrationMode = m_getButtonPressed();
  char version = EEPROM.read(0);
  if (version != m_mainData.robotVersion) {
    calibrationMode = true;
  }
  _initComponents(calibrationMode);
  delay(200);
  updateGyroCount(0);
  // === sets m_gyroState
  delay(20);
  resetGyro();
  delay(20);
  updateGyroCount(0);
  delay(200);
  setCenter(0);
  setServo(SERVO_HIGHT_LOW, SERVO_HIGHT_LOW, 20);
  // update current readings
  updateCurrentCount(0);
  // read proximity sensors
  updateInputsCount(0);
  // bool walkingModeNow
  m_mainData.mainCounter = updateCounter(true, m_mainData.masterDevice);
  // === sets m_legsValue.xx.count
  // === sets m_legsValue.xx.state
}

// the loop function runs over and over again forever
void loop() {
  if (m_mainData.mainCounter == 0) {
    if (m_mainData.masterDevice) {
      // set new pattern and task
      setPatternAndTask(getCurrentState(), getGyroState());
      // get pattern
      m_mainData.patternNow = getPatternOfTask();
    }
    if (m_mainData.patternNow == P_STANDGO) {
      if (m_mainData.masterDevice) {
        // normal walking to avoid obstacles
        // get and set new direction
        setDirectionGyro(calculateNewDirectionPath(getInputState(), getWallAngleInputs(), getDirectionGyro()));
        m_mainData.directionCorrection = getDirectionCharGyro();
        // update path
        updatePath(getDirectionGyro());
        setDirectionCenter(-m_mainData.directionCorrection);
        setDirectionWheels(m_mainData.speedLeft, m_mainData.speedRight);
      } else {
        // set speed
        setSideSpeed(m_mainData.speedLeft, m_mainData.speedRight);
        setDirectionCenter(m_mainData.directionCorrection);
        setDirectionWheels(m_mainData.speedLeft, m_mainData.speedRight);
      }
      // check for robot state
      _setState(stateUpdate(getSurfaceBumpyGyro(), getSurfaceFlatGyro(), getExtraInputState()));
      _doCycle();
    } else {
      // quick and non walking patterns
      _doQuickAndOther(m_mainData.patternNow);
    }
  } else {
    // cycle in the middle of pattern
    _doCycle();
  }
}
