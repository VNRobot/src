/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

#include <Servo.h>

// input state
enum inState {
  IN_LOW_BATTERY,
  IN_HIGH_CURRENT_1,
  IN_HIGH_CURRENT_2,
  IN_HIGH_CURRENT_3,
  IN_WALL_FRONTLEFT,
  IN_WALL_FRONTRIGHT,
  IN_WALL_LEFT,
  IN_WALL_RIGHT,
  IN_OBSTACLE_FRONTLEFT,
  IN_OBSTACLE_FRONTRIGHT,
  IN_OBSTACLE_LEFT,
  IN_OBSTACLE_RIGHT,
  IN_NORMAL             
};

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
// pin numbers for servo motors
enum dPinsServo {
  SW1_MOTOR = 2,
  SW2_MOTOR = 3,
  ST1_MOTOR = 4,
  ST2_MOTOR = 5,
  FL1_MOTOR = 6,
  FL2_MOTOR = 7,
  FR1_MOTOR = 8,
  FR2_MOTOR = 9,
  RL1_MOTOR = 10,
  RL2_MOTOR = 11,
  RR1_MOTOR = 12,
  RR2_MOTOR = 13
};
// init servo library
//Servo m_servo_sw_1;
//Servo m_servo_sw_2;
Servo m_servo_st_1;
Servo m_servo_st_2;
Servo m_servo_fl_1;
Servo m_servo_fl_2;
Servo m_servo_fr_1;
Servo m_servo_fr_2;
Servo m_servo_rl_1;
Servo m_servo_rl_2;
Servo m_servo_rr_1;
Servo m_servo_rr_2;
// structure for one leg motors
struct motors {
  char motor1;
  char motor2;
};
// structure for one leg data
struct leg {
  short hight;
  short shift;
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
// legs motors structure
struct allLegs {
  leg fl;
  leg fr;
  leg rl;
  leg rr;
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

// motors calibration values for optional 12 motors
allMotors calibrationData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// inputs state defined in inState
unsigned char inputState = IN_NORMAL;
// gyro state
accRoll gyroState;
// sequence counter 0 to m_fullCycle - 1
unsigned char sequenceCounter = 0;
// current pattern defined in rPatterns
unsigned char patternNow = P_DOSTAND;
// enable sensors flag toggled by P_ENABLEINPUTS and P_DISABLEINPUTS
bool sensorsEnabled = false;
// default task from rTasks
unsigned char defaultTask = GO_TASK;
// current task
unsigned char taskNow = STAND_TASK;
// next task
unsigned char taskNext = STAND_TASK;
// variable for temporary use
unsigned char i;
//-------------global variables---------------------------
// full cycle
unsigned char m_fullCycle = 36;
// half cycle
unsigned char m_halfCycle = 18;
// main time delay in the loop in msec
unsigned char m_timeDelay = 8;
// roll ballance flag
bool m_rollBallanceEnabled = false;
// pitch ballance flag
bool m_pitchBallanceEnabled = false;
// forward ballance flag
bool m_forwardBallanceEnabled = false;
// software version hardcoded. should be changed manually
unsigned char m_versionEeprom = 53;
// maximal pair of legs current
unsigned short m_maxInputCurrent = 1500; //ma
// normal distance sensor beam to ground
unsigned char m_normalInputDistance = 50; //cm
// center position in the leg forward shift
char m_forwardCenterBallance = 0; // bigger the number more weight on front
// default height in mm
short m_defaultHight = 125;
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
  // check for Mode button press or if not calibrated
  if (_readButtonPress() || (m_versionEeprom != readSoftwareVersionEeprom())) {
    // factory mode is used for legs calibration
    Serial.println(F("Entering factory mode"));
    // do calibration
    if (doCalibration(& calibrationData)) {
      writeCalibrationEeprom(calibrationData);
      writeSoftwareVersionEeprom(m_versionEeprom);
      delay(6000);
    }
  } else {
    // read values by using pointer to struct
    readCalibrationEeprom(& calibrationData);
    // init servo motors hight in mm
    initServo(calibrationData, 80);
    delay(200);
    setServo(calibrationData, m_defaultHight);
    delay(200);
    // init digital sensors
    initInputs();
    // update inputs direction is 0
    updateInputs(sequenceCounter, sensorsEnabled, 0);
    // init gyro MPU6050 using I2C
    initGyro();
    delay(200);
    resetGyro();
    delay(20);
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
    // load task and pattern. direction is 0
    setCenter(patternNow, 0);
    setPattern(patternNow, 0);
    sequenceCounter = updateCountPatterns();
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
      //printTaskname(taskNow);
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
        setCenter(patternNow, getDirectionCorrectionGyro());
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
      case P_DODOWN:
      {
        // disable motors
        detachServo(calibrationData);
      }
      break;
      default:
      {
        setCenter(patternNow, 0);
        setPattern(patternNow, 0);
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
  updateCenterServo(calibrationData, getValueCenter(sequenceCounter));
  updateLegsServo(calibrationData, getWalkPatterns());
  delay(m_timeDelay);
  // update motor pattern point
  sequenceCounter = updateCountPatterns();
  // read proximity sensors
  inputState = updateInputs(sequenceCounter, sensorsEnabled, getDirectionGyro());
  // update gyro readings
  if (sequenceCounter == 0) {
    // update ballance
    gyroState = updateGyro(sequenceCounter);
    updateStaticBallanceServo(getStaticBallance(gyroState, sequenceCounter));
  } else {
    // update static ballance
    updateStaticBallanceServo(getStaticBallance(updateGyro(sequenceCounter), sequenceCounter));
  }
}
