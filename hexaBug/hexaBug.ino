/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

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
  DEMO_TASK,
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
// structure for one leg motors
struct motors {
  char motor1;
  char motor2;
};
// legs motors structure
struct allMotors {
  motors sw;
  motors st;
  motors fl;
  motors fr;
  motors rl;
  motors rr;
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
// touch with 3k resistor
struct touch {
  bool set1;
  bool set2;
  bool set3;
};

// motors calibration values for 12 motors
allMotors calibrationData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// inputs state
unsigned char inputState = 0;
// gyro state
accRoll gyroState;
// sequence counter
unsigned char sequenceCounter = 0;
// current pattern
unsigned char patternNow = P_DOSTAND;
// enable sensors flag
bool sensorsEnabled = false;
// default task
unsigned char defaultTask = GO_TASK;
// current task
unsigned char taskNow = STAND_TASK;
// next task
unsigned char taskNext = STAND_TASK;
// new task
unsigned char _newTask = BEGIN_TASK;
// variable for temporary use
unsigned char i;
//-------------global variables---------------------------
// full cycle
unsigned char m_fullCycle = 36;
// half cycle
unsigned char m_halfCycle = 18;
// main time delay in the loop in msec
unsigned char m_timeDelay = 20;
// roll ballance flag
bool m_rollBallanceEnabled = false;
// pitch ballance flag
bool m_pitchBallanceEnabled = false;
// forward ballance flag
bool m_forwardBallanceEnabled = false;
// auto calibration enabled
bool m_autoCalibrationEnabled = false;
// calibration current
unsigned short m_calibrationCurrent = 960; //ma
// software version hardcoded. should be changed manually
unsigned char m_versionEeprom = 91;
// maximal pair of legs current
unsigned short m_maxInputCurrent = 2000; //ma
// normal distance sensor beam to ground
unsigned char m_normalInputDistance = 50; //cm
// center position in the pattern array. center point is 22
char m_forwardCenterServo = 22; // bigger the number more weight on front
// sensors enabled
unsigned char m_sensorsInputsEnabled = false;
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

// init hardware
void initHardware(short initAngle, short setAngle) {
  // init servo motors into 0 - horizontal, 90 - vertical. increase angle lifting robot
  initServo(calibrationData, initAngle, initAngle);
  delay(200);
  setServo(calibrationData, setAngle, setAngle);
  delay(200);
  // init digital sensors
  initInputs();
  updateInputs(0, sensorsEnabled);
  // init gyro MPU6050 using I2C
  initGyro();
  delay(200);
  resetGyro();
  delay(20);
}

// runs once on boot or reset
void setup() {
  // Start serial for debugging
  Serial.begin(9600);
  Serial.println(F("Device started"));
  delay(200);
  // check for Mode button press or if not calibrated
  if (_readButtonPress() || (m_versionEeprom != readSoftwareVersionEeprom())) {
    // factory mode is used for legs calibration
    Serial.println(F("Entering factory mode"));
    // init hardware
    initHardware(45, 0);
    // do calibration
    if (doCalibration(& calibrationData)) {
      writeCalibrationEeprom(calibrationData);
      writeSoftwareVersionEeprom(m_versionEeprom);
      delay(6000);
    }
    // disable motors
    detachServo(calibrationData);
  } else {
    // read values by using pointer to struct
    readCalibrationEeprom(& calibrationData);
    // init hardware
    initHardware(20, 45);
    // demo mode activated when hand is placed 5cm from sensors during the boot
    if (checkForDemoModeInputs()) {
      // demo mode
      Serial.println(F("Entering demo mode"));
      applyTask(DEMO_TASK);
    } else {
      // explore mode
      Serial.println(F("Entering explore mode"));
      applyTask(BEGIN_TASK);
    }
    // update gyro readings
    gyroState = updateGyro(sequenceCounter);
    // load task and pattern
    setSteps(patternNow, 0);
    setPattern(patternNow);
    sequenceCounter = updateCountPatterns();
    updatePhaseSteps(sequenceCounter);
  }
}

// the loop function runs over and over again forever
void loop() {
  if (sequenceCounter == 0) {
    // check emergency task
    taskNext = getHighPriorityTaskByInputs(gyroState, inputState);
    if ((taskNext != DEFAULT_TASK) && (taskNow != taskNext)) {
      taskNow = taskNext;
      // apply new task
      applyTask(taskNow);
      // debug print
      printTaskname(taskNow);
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
        printTaskname(taskNow);
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
        setSteps(patternNow, getDirectionCorrectionGyro());
        setPattern(patternNow);
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
        m_timeDelay = 30;
      }
      break;
      case P_LONGDELAY:
      {
        m_timeDelay = 60;
      }
      break;
      default:
      {
        setSteps(patternNow, 0);
        setPattern(patternNow);
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
  updateServo(calibrationData, getWalkPatterns(), getLiftPatterns());
  delay(m_timeDelay);
  bool stepsClear = updateSurfaceSteps(updateTouchInputs());
  // update motor pattern point
  if (stepsClear) {
    sequenceCounter = updateCountPatterns();
  }
  updatePhaseSteps(sequenceCounter);
  // read proximity sensors
  inputState = updateInputs(sequenceCounter, sensorsEnabled);
  // update gyro readings
  if (sequenceCounter == 0) {
    // update ballance
    gyroState = updateGyro(sequenceCounter);
    updateStaticBallanceServo(getStaticBallance(gyroState, sequenceCounter));
    setFowardBallanceSteps(getForwardBallance(gyroState));
  } else {
    // update static ballance
    updateStaticBallanceServo(getStaticBallance(updateGyro(sequenceCounter), sequenceCounter));
  }
}
