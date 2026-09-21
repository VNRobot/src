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
// main half time delay in ms. bigger the number slower the robot
#define TIME_DELAY              10
// low hight in mm. upper arm is horizontal
#define HIGHT_LOW               80
// normal hight
#define HIGHT_DEFAULT           120
// maximal hight
#define HIGHT_MAX               160
// lift value
#define LIFT_DEFAULT            50
// calibration angle
#define CALIBRATION_ANGLE_MIN   -15
#define CALIBRATION_ANGLE_MAX   15
// robot size devider
#define ROBOT_SIZE_DEVIDER      1
// leg lift point
#define LIFT_POINT              7
// main counter
#define MAIN_COUNTER_END        44
// linear leg speed
#define LINEAR_SPEED            3
// step size mm
#define STEP_SIZE               90

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
  char liftPoint;
  char speed;
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
  bool masterDevice;           // master device flag
  char mainCounter;            // main counter
  char patternNow;             // current pattern
  char speedLeft;              // left side speed
  char speedRight;             // right side speed
  char currentState;           // current received from other device
  char shiftForward;           // shift forward ballance
} masterData;

//---------------global variables---------------------------
// gyro state
accRoll m_gyroState = {0, 0, 0, 0, 0, 0, 0, 0, 0};
// leg values for 4 legs
allLegs m_legsValue = {125, 0, LEG_LINEAR, 0, LIFT_POINT, 0,
                       125, 0, LEG_LINEAR, 0, LIFT_POINT, 0,
                       125, 0, LEG_LINEAR, 0, LIFT_POINT, 0,
                       125, 0, LEG_LINEAR, 0, LIFT_POINT, 0};
// master data
masterData m_mainData { false, 0, P_STANDGO, 0, 0, C_NORMAL, 0};
//----------------------------------------------------------
// variable for temporary use
unsigned char i;
// robot version
char robotVersion = ROBOT_VERSION;

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
void _doQuickAndOther(char patternNow) {
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
      setServoQuick(HIGHT_LOW, HIGHT_LOW, 500);
      if (m_gyroState.aRollAverage < 0) {
        setFlippedServo(1, -1);
        setServoQuick(HIGHT_MAX, HIGHT_MAX, 500);
        setFlippedServo(-1, 1);
        setServoQuick(HIGHT_MAX, HIGHT_MAX, 500);
      } else {
        setFlippedServo(-1, 1);
        setServoQuick(HIGHT_MAX, HIGHT_MAX, 500);
        setFlippedServo(1, -1);
        setServoQuick(HIGHT_MAX, HIGHT_MAX, 500);
      }
      setServoQuick(HIGHT_LOW, HIGHT_LOW, 500);
      setFlippedGyro(false);
      setFlippedServo(1, 1);
      setServoQuick(HIGHT_LOW, HIGHT_LOW, 500);
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
    break;
  }
}

  // send data
  void _sendData() {
    if (m_mainData.masterDevice) {
      // send robotVersion, m_mainData.patternNow, m_mainData.speedLeft, m_mainData.speedRight, m_mainData.shiftForward
      Serial.print(robotVersion);
      Serial.print(m_mainData.patternNow);
      Serial.print(m_mainData.speedLeft);
      Serial.print(m_mainData.speedRight);
      Serial.print(m_mainData.shiftForward);
    } else {
      // send m_mainData.currentState
      Serial.print(m_mainData.currentState);
    }
  }

  // receive data
  int _receiveData() {
    int counter = Serial.available();
    if (m_mainData.masterDevice) {
      // receive m_mainData.currentState
      if (counter > 0) {
          m_mainData.currentState = Serial.read();
      }
    } else {
      // receive robotVersion, m_mainData.patternNow, m_mainData.speedLeft, m_mainData.speedRight, m_mainData.shiftForward
      if (counter > 4) {
        char version = Serial.read();
        if (version == robotVersion) {
          m_mainData.patternNow = Serial.read();
          m_mainData.speedLeft = Serial.read();
          m_mainData.speedRight = Serial.read();
          m_mainData.shiftForward = Serial.read();
          // set counter
          setPatternOfTask(m_mainData.patternNow);
          setHalfCounter(getWalkingModeInTask());
        }
      }
    }
    return Serial.available();
  }

// set motors and read sensors
void _doCycle(void) {
  // forward ballance
  m_mainData.shiftForward = setBallanceShiftCount(m_mainData.masterDevice, m_mainData.shiftForward);
  // set legs shift
  setWalkShiftCount(getWalkingModeInTask());
  // === sets m_legsValue.xx.shift
  // set legs lift
  setWalkLiftsCount(getWalkingModeInTask());
  // === sets m_legsValue.xx.hight
  updateLegsServoCount();
  // === motors set ===
  // do communication
  if (m_mainData.mainCounter == 0) {
    // send once at the beginning of the pattern
    _sendData();
  }
  delay(TIME_DELAY);
  // try to receive in every count
  _receiveData();
  delay(TIME_DELAY);
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
  //updateCenterCount();
}

// runs once on boot or reset
void setup() {
  // Start serial for debugging
  delay(200);
  Serial.begin(9600);
  delay(500);
  // -------init shift------- 
  // short shiftForward, bool walk, bool ballance
  initShift(0, true, false);
  // -------init lifts------- 
  // short legHight, short legLift, bool sideBallance
  initLifts(HIGHT_DEFAULT, LIFT_DEFAULT, false);
  // check button press
  bool calibrationMode = m_getButtonPressed();
  char version = EEPROM.read(0);
  if (version != robotVersion) {
    calibrationMode = true;
  }
  // -------init sensors inputs-------
  // bool calibrationMode, short legHight, bool sensorsEnabled, bool extraInputsEnabled
  initInputs(calibrationMode, HIGHT_DEFAULT, false, false);
  enableObstacleInputs(false);
  enableEdgeInputs(false);
  // -------attach center servo-------
  //attachCenter();
  // -------attach legs servo-------
  attachServo();
  // -------init current readings-------
  // bool calibrationMode, bool extraEnabled
  initCurrent(calibrationMode, true);
  // init center servo motors
  //initCenter(calibrationMode);
  // init legs servo motors
  initServo(calibrationMode);
  if (calibrationMode) {
    delay(1000);
    // lift legs for gyro calibration
    setFlippedGyro(true);
    setFlippedServo(-1, -1);
    //setCenter(0);
    setServo(HIGHT_MAX, HIGHT_MAX, 20);
  }
  // -------init gyro-------
  initGyro(calibrationMode);
  delay(200);
  updateGyroCount(0);
  // === sets m_gyroState
  delay(20);
  resetGyro();
  delay(20);
  updateGyroCount(0);
  // === sets m_gyroState
  if (calibrationMode) {
    // write software version
    #ifdef BOARD_ESP32
      EEPROM.write(0, robotVersion);
      EEPROM.commit();
    #else
      EEPROM.update(0, robotVersion);
    #endif
    // disable motors
    detachServo();
    //detachCenter();
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
  // -------init tasks-------
  initTasks();
  // load task and pattern. direction is 0
  // -------init path-------
  // short stepSize, short speed, bool turning, bool counting
  initPath(STEP_SIZE, true, false);
  setDistancePath(100); // cm
  updatePath(0);
  // === sets m_legsValue.xx.speed
  // -------init counter-------
  // short mainCycle
  initCounter(MAIN_COUNTER_END);
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
        // update path
        updatePath(getDirectionGyro());
        // === sets m_legsValue.xx.speed
        m_mainData.speedLeft = m_legsValue.fl.speed;
        m_mainData.speedRight = m_legsValue.fr.speed;
        //setDirectionCenter(getDirectionGyro());
      } else {
        // set speed
        setSideSpeed(m_mainData.speedLeft, m_mainData.speedRight);
      }
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
