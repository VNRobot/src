/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

// patterns enumerator
enum m_patterns {
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
enum m_tasks {
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
enum m_mode {
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
// motors values for 10 motors
allMotors m_motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// gyro state
accRoll _gyroState;
// Array to store currently executed task. contains list of patterns
unsigned char m_currentTask[28] = {DOCALIBRATION, DOREPEAT};
// pattern counter points to m_currentTask
unsigned char m_patternCounter = 0;

// enable sensors flag
bool m_sensorsEnabled = true;
// default task
unsigned char m_defaultTask = WALKTASK;
// device mode
unsigned char m_deviceMode = EXPLORE;
// mode button press flag
bool m_modePressed = false;
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
    m_deviceMode = CALIBRATION_INFO;
    calM1 = -60;
    calM2 = 60;
  } else if (getSoftwareVersionEeprom() != readSoftwareVersionEeprom()) {
    // set calibration mode
    m_deviceMode = CALIBRATION_INFO;
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
  if (m_deviceMode == CALIBRATION_INFO) {
    // factory mode is used for legs calibration
    Serial.println("Entering factory mode");
  } else {
    // normal operation
    // load calibration if available
    if (getSoftwareVersionEeprom() == readSoftwareVersionEeprom()) {
      readCalibrationEeprom();
      // set motors values after calibration
      setServo();
    }
    // demo mode activated when hand is placed 5cm from sensors during the boot
    if (checkForDemoModeInputs()) {
      // demo mode
      Serial.println("Entering demo mode");
      applyTask(DEMOTASK);
      // disable sensors in demo mmode
      m_sensorsEnabled = false;
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
  // update servo motors values. 
  _sequenceCounter = updatePatterns(_gyroState);
  // move motors.
  updateServo();
  // walking speed depends of the delay
  delay(_timeDelay);
  // check sequence start
  if (_sequenceCounter == 0) {
    // check device mode
    if (m_deviceMode == EXPLORE) {
      // explore mode
      // process cycling commands
      switch (m_currentTask[m_patternCounter]) {
        case RESET:
        {
          m_patternCounter = 0;
        }
        break;
        case DOREPEAT:
        {
          if (m_patternCounter > 0) {
            m_patternCounter --;
          }
        }
        break;
        default:
        break;
      }
      // update pattern for the next sequence
      exploreModeCall(setNextPattern(m_currentTask[m_patternCounter]));
    } else {
      // factory mode
      factoryModeCall(setNextPattern(m_currentTask[m_patternCounter]));
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
      applyTask(getTaskByInputs(_gyroState.stateGyro, _inputState));
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
      _newTask = getTaskByInputs(_gyroState.stateGyro, _inputState);
      if (m_defaultTask == _newTask) {
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
    if (m_deviceMode < CALIBRATION_START) {
      m_deviceMode = CALIBRATION_START;
    } else {
      m_modePressed = true;
    }
    Serial.print("Factory stage set to ");
    Serial.println((int)m_deviceMode);
  }
  // factory mode stages
  switch (m_deviceMode) {
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
      Serial.print((int)getCenterCurrentInputs());
      Serial.print(" front ");
      Serial.print((int)getFrontCurrentInputs());
      Serial.print(" rear ");
      Serial.println((int)getRearCurrentInputs());
    break;
    case CALIBRATION_START: 
    {
      m_deviceMode = CALIBRATION_FRONT;
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
        m_patternCounter = legsCalibration(m_patternCounter);
      }
    } 
    break;
    case CALIBRATION_SAVE:
    {
      m_deviceMode = CALIBRATION_DONE;
      Serial.println("Saving calibration data");
      writeCalibrationEeprom();
      writeSoftwareVersionEeprom(getSoftwareVersionEeprom());
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
