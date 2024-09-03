/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

// patterns enumerator
enum rPatterns {
  DOSTAND,
  STANDTOWALK,
  WALKTOSTAND,
  STANDWALK,
  STANDWALKLEFT,
  STANDWALKRIGHT,
  STANDWALKSHIFTLEFT,
  STANDWALKSHIFTRIGHT,
  WALKFORWARD,
  WALKLEFT,
  WALKRIGHT,
  WALKSHIFTLEFT,
  WALKSHIFTRIGHT,
  WALKBACK,
  DODOWN,
  RESET,
  DOREPEAT,
  DONE,
  DOCALIBRATION,
  DOCALIBRATION_1,
  DOCALIBRATION_2,
  RESETDIRECTION,
  INPROGRESS
};
// tasks enumerator
enum rTasks {
  CALIBRATIONTASK,
  DEMOTASK,
  BEGINTASK,
  DOWNTASK,
  WALKBACKTASK,
  TURNAROUNDTASK,
  WALKSHIFTRIGHTTASK,
  WALKSHIFTLEFTTASK,
  WALKTURNRIGHTTASK,
  WALKTURNLEFTTASK,
  STANDTURNRIGHTTASK,
  STANDTURNLEFTTASK,
  STANDTURNRIGHT2TASK,
  STANDTURNLEFT2TASK,
  WALKTASK,
  STANDWALKTASK,
  STANDTASK
};
// device mode enumerator
enum rMode {
  EXPLORE,
  CALIBRATION_INFO,
  CALIBRATION_START,
  CALIBRATION_FRONT,
  CALIBRATION_REAR,
  CALIBRATION_AUTO_1,
  CALIBRATION_AUTO_2,
  CALIBRATION_SAVE,
  CALIBRATION_DONE
};

enum gState {
  GYRO_NORM,          // moving
  GYRO_SHAKEN,        // shaken
  GYRO_UPSIDEDOWN,    // upside down
  GYRO_HIT_SIDE,      // hit left or right
  GYRO_HIT_FRONT,     // hit front or back
  GYRO_FELL_LEFT,    // fell left
  GYRO_FELL_RIGHT,   // fell right
  GYRO_FELL_FRONT,   // fell front
  GYRO_FELL_BACK,     // fell back
  GYRO_DOWN_HILL,    // moving down hill
  GYRO_UP_HILL,      // moving up hill
  GYRO_FOLLING_LEFT,  // folling left
  GYRO_FOLLING_RIGHT, // folling right
  GYRO_FOLLING_FRONT, // folling front
  GYRO_FOLLING_BACK  // folling back
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
// gyro state
accRoll _gyroState;

// enable sensors flag
bool _sensorsEnabled = true;
// default task
unsigned char _defaultTask = WALKTASK;
// main time delay in the loop in msec
unsigned char _timeDelay = 25;
// new task
 unsigned char _newTask;
// inputs state
unsigned char _inputState = 0;
// sequence counter
unsigned char _sequenceCounter = 0;
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
  int calM1 = 0;
  int calM2 = 0;
  // Start serial for debugging
  Serial.begin(9600);
  Serial.println("Device started");
  // check for Mode button press and for stored calibration
  if (_readButtonPress()) {
    // set calibration mode
    setModeCalibration(CALIBRATION_INFO);
    calM1 = -60;
    calM2 = 60;
  } else if (getSoftwareVersionEeprom() != readSoftwareVersionEeprom()) {
    // set calibration mode
    setModeCalibration(CALIBRATION_INFO);
    calM1 = -60;
    calM2 = 60;
  }
  // init servo motors
  initServo(calM1, calM2);
  // clear proximity sensors
  initInputs();
  updateInputs(0);
  // check for factory mode
  // factory mode is activated when "mode" button is pressed during the boot
  if (getModeCalibration() == CALIBRATION_INFO) {
    // factory mode is used for legs calibration
    Serial.println("Entering factory mode");
  } else {
    // normal operation
    // load calibration if available
    if (getSoftwareVersionEeprom() == readSoftwareVersionEeprom()) {
      // read values by using pointer to struct
      readCalibrationEeprom(& m_calibration);
      // set motors values after calibration
      setServo(m_calibration);
    }
    // demo mode activated when hand is placed 5cm from sensors during the boot
    if (checkForDemoModeInputs()) {
      // demo mode
      Serial.println("Entering demo mode");
      applyTask(DEMOTASK);
      // disable sensors in demo mmode
      _sensorsEnabled = false;
    } else {
      Serial.println("Entering explore mode");
      applyTask(BEGINTASK);
    }
  }
  // init gyro MPU6050 using I2C
  delay(500);
  initGyro();
  delay(200);
  // reset gyro
  resetGyro();
}

// the loop function runs over and over again forever
void loop() {
  // read proximity sensors
  _inputState = updateInputs(_sequenceCounter);
  // update gyro readings
  _gyroState = updateGyro(_sequenceCounter);
  // update servo motors values.move motors
  updateServo(updateMotorsPatterns(m_calibration));
  // update motor pattern point
  _sequenceCounter = updatePointPatterns(_gyroState, getModeCalibration());
  // walking speed depends of the delay
  delay(_timeDelay);
  // check sequence start
  if (_sequenceCounter == 0) {
    // check device mode
    if (getModeCalibration() == EXPLORE) {
      // explore mode
      exploreModeCall(setPattern(getPatternInTask()));
    } else {
      // factory mode
      factoryModeCall(setPattern(getPatternInTask()));
    }
  }
}

// explore mode call
void exploreModeCall(unsigned char patternStatus) {
  // if pattern execution is done, set new task
  switch (patternStatus) {
    case DONE:
    {     
      // get next task
      // proximity sensors input set next task. 
      applyTask(getTaskByInputs(_gyroState.stateGyro, _inputState, _defaultTask, _sensorsEnabled));
    }
    break;
    case STANDWALK:
    {
      updateTurnPattern(getDirectionCorrectionGyro());
    }        
    break;
    case WALKFORWARD:
    {
      // assume WALKFORWARD is deault task
      _newTask = getTaskByInputs(_gyroState.stateGyro, _inputState, _defaultTask, _sensorsEnabled);
      if (_defaultTask == _newTask) {
        updateTurnPattern(getDirectionCorrectionGyro());
      } else {
        applyTask(_newTask);
      }
    }
    break;
    case RESETDIRECTION:
    {
      resetDirectionGyro();
    }
    break;
    default:
    break;
  }
}

// factory mode call
void factoryModeCall(unsigned char patternStatus) {
  // update factory stage
  if (_readButtonPress()) {
    if (getModeCalibration() < CALIBRATION_START) {
      setModeCalibration(CALIBRATION_START);
    } else {
      setButtonCalibration();
    }
    Serial.print("Factory stage set to ");
    Serial.println((int)getModeCalibration());
  }
  // factory mode stages
  switch (getModeCalibration()) {
    case CALIBRATION_INFO: 
      // print proximity sensors
      Serial.print("Left eye ");
      Serial.print((int)analogRead(A0));
      Serial.print(" Right eye ");
      Serial.println((int)analogRead(A1));
      // print gyro data
      Serial.print("Direction ");
      Serial.println((int)getDirectionCorrectionGyro());
      // motors current
      Serial.print("Battery  ");
      Serial.print((int)analogRead(A6));
      Serial.print(" Current center ");
      Serial.print((int)getCurrent1Inputs());
      Serial.print(" front ");
      Serial.print((int)getCurrent2Inputs());
      Serial.print(" rear ");
      Serial.println((int)getCurrent3Inputs());
    break;
    case CALIBRATION_START: 
    {
      setModeCalibration(CALIBRATION_FRONT);
      Serial.println("Starting legs calibration");
      applyTask(CALIBRATIONTASK);
    } 
    break;
    case CALIBRATION_FRONT: 
    case CALIBRATION_REAR:
    case CALIBRATION_AUTO_1: 
    case CALIBRATION_AUTO_2: 
    {
      if (patternStatus == DOCALIBRATION) {
        setPointerInTask(legsCalibration(getPointerInTask(), & m_calibration));
      }
    } 
    break;
    case CALIBRATION_SAVE:
    {
      setModeCalibration(CALIBRATION_DONE);
      Serial.println("Saving calibration data");
      writeCalibrationEeprom(m_calibration);
      writeSoftwareVersionEeprom();
      Serial.print("Motors FL1: ");
      Serial.print((int)m_calibration.m.fl.motor1);
      Serial.print(" FL2: ");
      Serial.print((int)m_calibration.m.fl.motor2);
      Serial.print(" FR1: ");
      Serial.print((int)m_calibration.m.fr.motor1);
      Serial.print(" FR2: ");
      Serial.print((int)m_calibration.m.fr.motor2);
      Serial.print(" RL1: ");
      Serial.print((int)m_calibration.m.rl.motor1);
      Serial.print(" RL2: ");
      Serial.print((int)m_calibration.m.rl.motor2);
      Serial.print(" RR1: ");
      Serial.print((int)m_calibration.m.rr.motor1);
      Serial.print(" RR2: ");
      Serial.println((int)m_calibration.m.rr.motor2);
    } 
    break;
    case CALIBRATION_DONE: 
      Serial.println("Done. Please reset or restart.");
    break;
    default:
      Serial.println("Unknown factory stage");
    break;
  }
}
