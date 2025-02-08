/*
Ladybug Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

// patterns
enum rPatterns {
  P_DOSTAND,
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
  P_ENABLEINPUTLEFT,
  P_ENABLEINPUTRIGHT,
  P_DISABLEINPUTLEFT,
  P_DISABLEINPUTRIGHT,
  P_RECOVERLEFT,
  P_RECOVERRIGHT,
  P_RECOVER,
  P_ATTACH,
  P_DETACH,
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
  STAND_TASK,
  DOWN_TASK,
  BEND_LEFT_TASK,
  BEND_RIGHT_TASK,
  BEND_FRONT_TASK,
  BEND_REAR_TASK,
  RECOVER_LEFT_TASK,
  RECOVER_RIGHT_TASK,
  RECOVER_TASK,
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
  char motorL;
  char motorR;
};
// legs motors structure
struct legMotors {
  motors wheel;
};
// all motors structure
struct allMotors {
  char left;
  char right;
  legMotors m;
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

// motors calibration values for 10 motors
allMotors m_calibration = {0, 0, 0, 0};
// inputs logical state
unsigned char inputState = 0; // IN_NORMAL
// gyro state
accRoll gyroState;
// sequence counter
unsigned char sequenceCounter = 0;
// current pattern
unsigned char patternNow = P_DOSTAND;
// enable sensors flag
bool sensorLeftEnabled = true;
bool sensorRightEnabled = true;
// default task
unsigned char defaultTask = GO_TASK;
// current task
unsigned char taskNow = STAND_TASK;
// next task
unsigned char taskNext = STAND_TASK;
// main time delay in the loop in msec
unsigned char _timeDelay = 20;
//----------------------------------------------------------
// variables for temporary use
unsigned char i;
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
  // init proximity sensors
  initInputs();
  updateInputs(0, sensorLeftEnabled, sensorRightEnabled);
  // init gyro MPU6050 using I2C
  delay(500);
  initGyro();
  // check for Mode button press or not calibrated
  if (_readButtonPress() || (getSoftwareVersionEeprom() != readSoftwareVersionEeprom())) {
    // factory mode is used for legs calibration
    Serial.println(F("Entering factory mode"));
    // init servo motors for calibration
    initServo(40, 40); // center arm
    // set motors values positive closes wings. 50 closed, -30 open
    setServo(& m_calibration, 50, 50);
    // do calibration
    if (doCalibration(& m_calibration)) {
      writeCalibrationEeprom(m_calibration);
      writeSoftwareVersionEeprom();
    }
    delay(500);
  } else {
    // init servo motors for normal operation
    initServo(50, 50);
  }
  // normal operation
  // load calibration if available
  if (getSoftwareVersionEeprom() == readSoftwareVersionEeprom()) {
    // read values by using pointer to struct
    readCalibrationEeprom(& m_calibration);
  }
  Serial.println(F("Entering explore mode"));
  applyTask(BEGIN_TASK);
  // set motors values after calibration
  setServo(& m_calibration, 50, 50);
  delay(200);
  // reset gyro
  resetGyro();
  delay(200);
  // update gyro readings
  gyroState = updateGyro(sequenceCounter);
  // load task and pattern
  setPattern(patternNow);
  sequenceCounter = updateCountPatterns();
  detachWingServo();
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
      if ((patternNow == P_DONE) || ((patternNow == P_GOFORWARD) && (sensorLeftEnabled && sensorRightEnabled))) {
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
      case P_GOFORWARD:
      {
        setPattern(patternNow);
        updateTurnPattern(getDirectionCorrectionGyro());
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
      case P_DISABLEINPUTLEFT:
      {
        sensorLeftEnabled = false;
      }
      break;
      case P_DISABLEINPUTRIGHT:
      {
        sensorRightEnabled = false;
      }
      break;
      case P_ENABLEINPUTLEFT:
      {
        sensorLeftEnabled = true;
      }
      break;
      case P_ENABLEINPUTRIGHT:
      {
        sensorRightEnabled = true;
      }
      break;
      case P_ATTACH:
      {
        attachWingServo();
      }
      break;
      case P_DETACH:
      {
        detachWingServo();
      }
      break;
      case P_DONE:
      // do nothing
      // immediatelly run loop again
      break;
      default:
        setPattern(patternNow);
        doCycle();
      break;
    }
  } else if (sequenceCounter == 10) {
    // check for task interruption
    if (checkInterruptionInputs(taskNow, patternNow)) {
      setPattern(P_DOSTAND);
    }
    doCycle();
  } else {
    doCycle();
  }
}

// set motors and read sensors
void doCycle(void) {
  // update servo motors values, move motors
  updateServo(updateMotorsPatterns(m_calibration));
  // update motor pattern point
  sequenceCounter = updateCountPatterns();
  // walking speed depends of the delay
  delay(_timeDelay);
  // read proximity sensors
  inputState = updateInputs(sequenceCounter, sensorLeftEnabled, sensorRightEnabled);
  // update gyro readings
  gyroState = updateGyro(sequenceCounter);
}
