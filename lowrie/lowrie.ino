/*
Walking Robot Lowrie
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
  P_ENABLEINPUTS,
  P_DISABLEINPUTS,
  P_STANDGOSHIFTLEFT,
  P_STANDGOSHIFTRIGHT,
  P_GOSHIFTLEFT,
  P_GOSHIFTRIGHT,
  P_DODOWN,
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
  DOWN_TASK
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
struct legMotors {
  motors fl;
  motors fr;
  motors rl;
  motors rr;
};
// all motors structure
struct allMotors {
  char front;
  char rear;
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
allMotors m_calibration = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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
// main time delay in the loop in msec
unsigned char _timeDelay = 25;
// new task
 unsigned char _newTask = BEGIN_TASK;
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
  Serial.println("Device started");
  // init proximity sensors
  initInputs();
  updateInputs(0);
  // check for Mode button press or not calibrated
  if (_readButtonPress() || (getSoftwareVersionEeprom() != readSoftwareVersionEeprom())) {
    // do calibration
    if (doCalibration(& m_calibration)) {
      writeCalibrationEeprom(m_calibration);
    }
    delay(500);
  }
  // normal operation
  // load calibration if available
  if (getSoftwareVersionEeprom() == readSoftwareVersionEeprom()) {
    // read values by using pointer to struct
    readCalibrationEeprom(& m_calibration);
  }
  // demo mode activated when hand is placed 5cm from sensors during the boot
  if (checkForDemoModeInputs()) {
    // demo mode
    Serial.println("Entering demo mode");
    applyTask(DEMO_TASK);
    // disable sensors in demo mmode
  } else {
    Serial.println("Entering explore mode");
    applyTask(BEGIN_TASK);
  }
  // init servo motors
  initServo(0, 0);
  // set motors values after calibration
  setServo(m_calibration, 0, 0);
  // init gyro MPU6050 using I2C
  delay(500);
  initGyro();
  delay(200);
  // reset gyro
  resetGyro();
  delay(200);
  // update gyro readings
  gyroState = updateGyro(sequenceCounter);
  // load task and pattern
  setPattern(patternNow);
  sequenceCounter = updateCountPatterns(patternNow);
}

// the loop function runs over and over again forever
void loop() {
  if (sequenceCounter == 0) {
    // check for task end
    if ((patternNow == P_DONE) || (((patternNow == P_STANDGO) || (patternNow == P_GOFORWARD)) && (sensorsEnabled))) {
      // get next task
      taskNow = getTaskByInputs(gyroState, inputState, defaultTask, sensorsEnabled);
      // apply new task
      applyTask(taskNow);
      // debug print
      // printTaskname(taskNow);
      // get new task pattern
      patternNow = getPatternInTask();
    } else {
      if (patternNow != P_END) {
        // get next pattern
        patternNow = getNextPatternInTask();
      }
    }
    // debug print
    // printPatternName(patternNow);
    switch (patternNow) {
      case P_STANDGO:
      {
        setPattern(patternNow);
        updateTurnPattern(getDirectionCorrectionGyro());
        doCycle();
      }        
      break;
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
      default:
        setPattern(patternNow);
        doCycle();
      break;
    }
  } else {
    // check for task interruption
    if (((patternNow == P_STANDGO) || (patternNow == P_GOFORWARD)) && (sensorsEnabled)) {
      // get next task
      taskNext = getTaskByInputs(gyroState, inputState, defaultTask, sensorsEnabled);
      if (taskNext != taskNow) {
        // should interrupt
        setInterruptedPattern();
      }
    }
    doCycle();
  }
}

// set motors and read sensors
void doCycle(void) {
  // update servo motors values, move motors
  updateServo(updateMotorsPatterns(m_calibration));
  // update motor pattern point
  sequenceCounter = updateCountPatterns(patternNow);
  // update robot ballance
  // updateBallancePattern();
  // walking speed depends of the delay
  delay(_timeDelay);
  // read proximity sensors
  inputState = updateInputs(sequenceCounter);
  // update gyro readings
  gyroState = updateGyro(sequenceCounter);
}
