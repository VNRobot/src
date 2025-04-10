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
  P_GOFORWARDSLOW,
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
  P_DODOWN,
  P_DODOWNLEFT,
  P_DODOWNRIGHT,
  P_DODOWNFRONT,
  P_DODOWNREAR,
  P_RECOVERLEFT,
  P_RECOVERRIGHT,
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
  DEMO_TASK,
  DOWN_TASK,
  BEND_LEFT_TASK,
  BEND_RIGHT_TASK,
  BEND_FRONT_TASK,
  BEND_REAR_TASK,
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
struct legMotors {
  motors fl;
  motors fr;
  motors sl;
  motors sr;
  motors rl;
  motors rr;
};
// all motors structure
struct allMotors {
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

// motors calibration values for 12 motors
allMotors m_calibration = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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
// full cycle
unsigned char m_fullCycle = 20;
unsigned char m_halfCycle = 10;

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
  // init digital sensors
  initInputs();
  // read all sensors
  updateInputs(0, sensorsEnabled);
  // init gyro MPU6050 using I2C
  // init servo motors into 0 - horizontal, 90 - vertical. increase angle lifting robot
  initServo(30, 30);
  delay(400);
  // init gyro require horizontal position
  initGyro();
  // check for Mode button press or not calibrated
  if (_readButtonPress() || (getSoftwareVersionEeprom() != readSoftwareVersionEeprom())) {
    // factory mode is used for legs calibration
    Serial.println(F("Entering factory mode"));
    // set motors values with clear calibration data
    setServo(m_calibration, 90, 90);
    // do calibration
    if (doCalibration(& m_calibration)) {
      writeCalibrationEeprom(m_calibration);
      writeSoftwareVersionEeprom();
      delay(6000);
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
    Serial.println(F("Entering demo mode"));
    applyTask(DEMO_TASK);
    // disable sensors in demo mmode
  } else {
    Serial.println(F("Entering explore mode"));
    applyTask(BEGIN_TASK);
  }
  // set motors values after calibration
  setServo(m_calibration, 30, 30);
  delay(200);
  // reset gyro
  resetGyro();
  delay(200);
  // update gyro readings
  gyroState = updateGyro(sequenceCounter);
  // load task and pattern
  setPattern(patternNow, 0);
  sequenceCounter = updateCountPatterns();
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
      {
        setPattern(patternNow, getDirectionCorrectionGyro());
        doCycle();
      }        
      break;
      case P_GOFORWARD:
      {
        setPattern(patternNow, getDirectionCorrectionGyro());
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
      default:
        setPattern(patternNow, 0);
        doCycle();
      break;
    }
  } else {
    doCycle();
  }
}

// set motors and read sensors
void doCycle(void) {
  // update servo motors values, move motors
  updateServo(m_calibration, updateWalkPatterns(), updateLiftPatterns());
  delay(_timeDelay);
  // update motor pattern point
  sequenceCounter = updateCountPatterns();
  // read proximity sensors
  inputState = updateInputs(sequenceCounter, sensorsEnabled);
  // update gyro readings
  if (sequenceCounter == 0) {
    // update ballance
    gyroState = updateGyro(sequenceCounter);
    updateDynamicBallanceServo(getDynamicBallance(gyroState));
    updateStaticBallanceServo(getStaticBallance(gyroState, sequenceCounter));
    setFowardBallanceServo(getCenterStaticBallance());
  } else {
    // update static ballance
    updateStaticBallanceServo(getStaticBallance(updateGyro(sequenceCounter), sequenceCounter));
  }
}
