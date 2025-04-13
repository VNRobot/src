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
  P_STANDGOSHIFTLEFT,
  P_STANDGOSHIFTRIGHT,
  P_GOSHIFTLEFT,
  P_GOSHIFTRIGHT,
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
  GOSHIFTRIGHT_TASK,
  GOSHIFTLEFT_TASK,
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
  motors st;
  motors fl;
  motors fr;
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

// motors calibration values for 10 motors
allMotors calibrationData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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
unsigned char m_fullCycle = 24;
// half cycle
unsigned char m_halfCycle = 12;
// main time delay in the loop in msec
unsigned char m_timeDelay = 21;
// roll ballance flag
bool m_rollBallanceEnabled = false;
// pitch ballance flag
bool m_pitchBallanceEnabled = true;
// forward ballance flag
bool m_forwardBallanceEnabled = true;
// auto calibration enabled
bool m_autoCalibrationEnabled = true;
// calibration current
unsigned short m_calibrationCurrent = 640; //ma
// software version hardcoded. should be changed manually
unsigned char m_versionEeprom = 51;
// maximal pair of legs current
unsigned short m_maxInputCurrent = 1500; //ma
// normal distance sensor beam to ground
unsigned char m_normalInputDistance = 50; //cm
// leg lift angle when walking
unsigned char m_legPatternLift = 16;
// reverse center motor direction
bool m_reverseCenterServo = false;
// center position in the pattern array. center point is 24
char m_forwardCenterServo = 24; // (range 16 to 32) bigger the number more weight on front
// enable fast walking
bool m_fastWalkingPatternEnabled = true;
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
  if (_readButtonPress() || (m_versionEeprom != readSoftwareVersionEeprom())) {
    // factory mode is used for legs calibration
    Serial.println(F("Entering factory mode"));
    // set motors values with clear calibration data
    setServo(calibrationData, 90, 90);
    // do calibration
    if (doCalibration(& calibrationData)) {
      writeCalibrationEeprom(calibrationData);
      writeSoftwareVersionEeprom(m_versionEeprom);
      delay(6000);
    }
    delay(500);
  }
  // normal operation
  // load calibration if available
  if (m_versionEeprom == readSoftwareVersionEeprom()) {
    // read values by using pointer to struct
    readCalibrationEeprom(& calibrationData);
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
  setServo(calibrationData, 30, 30);
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
  updateServo(calibrationData, getWalkPatterns(), getLiftPatterns());
  delay(m_timeDelay);
  // update motor pattern point
  sequenceCounter = updateCountPatterns();
  // read proximity sensors
  inputState = updateInputs(sequenceCounter, sensorsEnabled);
  // update gyro readings
  if (sequenceCounter == 0) {
    // update ballance
    gyroState = updateGyro(sequenceCounter);
    updateStaticBallanceServo(getStaticBallance(gyroState, sequenceCounter));
    setFowardBallanceServo(getForwardBallance(gyroState));
  } else {
    // update static ballance
    updateStaticBallanceServo(getStaticBallance(updateGyro(sequenceCounter), sequenceCounter));
  }
}
