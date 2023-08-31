/*
Wlking Robot Lowrie
Licended GNU GPLv3 2023
Arduino nano
Main file
*/

#include <Servo.h>

Servo servo_frnt;
Servo servo_rear;
Servo servo_fl_1;
Servo servo_fl_2;
Servo servo_fr_1;
Servo servo_fr_2;
Servo servo_rl_1;
Servo servo_rl_2;
Servo servo_rr_1;
Servo servo_rr_2;

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
// buttons
enum m_dPins {
  MODE_BTN = 2,
  FRONT_MOTOR = 3,
  REAR_MOTOR = 4,
  FL1_MOTOR = 5,
  FL2_MOTOR = 6,
  FR1_MOTOR = 7,
  FR2_MOTOR = 8,
  RL1_MOTOR = 9,
  RL2_MOTOR = 10,
  RR1_MOTOR = 11,
  RR2_MOTOR = 12,
  BATTERY_LOW = 13
};
// device mode enumerator
enum m_mode {
  EXPLORE,
  CALIBRATION_INFO,
  CALIBRATION_FRONT,
  CALIBRATION_REAR,
  CALIBRATION_START,
  CALIBRATION_AUTO_1,
  CALIBRATION_AUTO_2,
  CALIBRATION_SAVE,
  CALIBRATION_DONE
};
// structure for motors
struct motors {
  char motor1;
  char motor2;
};
// front motor, rear motor, motor1, motor2
struct sequence {
  char front;
  char rear;
  motors m;
};
// structure for four legs
struct legs {
  unsigned char fl;
  unsigned char fr;
  unsigned char rl;
  unsigned char rr;
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
// Array to store currently executed task
// contains list of patterns
unsigned char m_currentTask[28];
// 16 positions per sequence
// all legs using the same sequence with different pozition shift
sequence m_currentSequence[16] = {
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,

  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,

  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,

  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30
};
// dynamic body balance for 8 motors
legMotors m_bodyBalance = {0, 0, 0, 0, 0, 0, 0, 0};
// motors calibration values for 10 motors
allMotors m_calibration = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// pattern counter points to m_currentTask
unsigned char m_patternCounter = 0;
// sequence shift between right and left legs
unsigned char m_leftRightShift = 0;
// enable sensors flag
bool m_sensorsEnabled = true;
// enable gyro flag
bool m_gyroEnabled = true;
// default task
unsigned char m_defaultTask = WALKTASK; // STANDWALKTASK;
// center position in the pattern array + - 6
int m_center = 10; // bigger the number more weight on front
// device mode
unsigned char _deviceMode = EXPLORE;
// points to m_currentSequence for every leg
legs _sequenceCount = { 0, 0, 0, 0};
// mode button press flag
bool _modePressed = false;
// calibration counter
unsigned char _calibrationCounter = 0;
// calibration stage
unsigned char _calibrationStage = 0;
// main time delay in the loop in msec
unsigned char _timeDelay = 35;
// new task
 unsigned char _newTask;
//----------------------------------------------------------
// variables for temporary use
unsigned char i;
//----------------------------------------------------------

// read button press in blocking mode
// return true when pressed and released
bool readButtonPress(int pin) {
  bool buffer = false;
  while (LOW == digitalRead(pin)) {
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
  // init digital inputs
  // mode button is active on LOW
  pinMode(MODE_BTN, INPUT_PULLUP); // button "mode"
  // battery voltage sensor is active on HIGH
  pinMode(BATTERY_LOW, INPUT);
  // check for Mode button press and for stored calibration
  if (readButtonPress(MODE_BTN) || (getSoftwareVersionEeprom() != readSoftwareVersionEeprom())) {
    // set calibration mode
    _deviceMode = CALIBRATION_INFO;
  }
  // init gyro MPU6050 using I2C
  delay(500);
  initGyro();
  delay(100);
  // init motor pins
  // motor connection pins
  //   f,  r, fl1, fl2, fr1, fr2, rl1, rl2, rr1, rr2
  //   3,  4,   5,   6,   7,   8,   9,  10,  11,  12
  // 
  // init motors one by one
  servo_frnt.attach(FRONT_MOTOR, 500, 2500);
  servo_frnt.write(90);
  delay(100);
  servo_rear.attach(REAR_MOTOR, 500, 2500);
  servo_rear.write(90);
  delay(100);
  servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  servo_fl_1.write(90);
  delay(100);
  servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  servo_fl_2.write(90);
  delay(100);
  servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  servo_fr_1.write(90);
  delay(100);
  servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  servo_fr_2.write(90);
  delay(100);
  servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  servo_rl_1.write(90);
  delay(100);
  servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  servo_rl_2.write(90);
  delay(100);
  servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  servo_rr_1.write(90);
  delay(100);
  servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  servo_rr_2.write(90);
  delay(100);
  // clear proximity sensors
  updateInputs(0);
  // check for factory mode
  // factory mode is activated when "mode" button is pressed during the boot
  if (_deviceMode == CALIBRATION_INFO) {
    // factory mode is used for legs calibration
    Serial.println("Entering factory mode");
    applyTask(DOWNTASK);
  } else {
    // normal operation
    // load calibration if available
    if (getSoftwareVersionEeprom() == readSoftwareVersionEeprom()) {
      readCalibrationEeprom();
      // set motors values after calibration
      servo_frnt.write(90 - m_calibration.front);
      delay(100);
      servo_rear.write(90 - m_calibration.rear);
      delay(100);
      servo_fl_1.write(90 + m_calibration.m.fl.motor1);
      delay(100);
      servo_fl_2.write(90 - m_calibration.m.fl.motor2);
      delay(100);
      servo_fr_1.write(90 - m_calibration.m.fr.motor1);
      delay(100);
      servo_fr_2.write(90 + m_calibration.m.fr.motor2);
      delay(100);
      servo_rl_1.write(90 + m_calibration.m.rl.motor1);
      delay(100);
      servo_rl_2.write(90 - m_calibration.m.rl.motor2);
      delay(100);
      servo_rr_1.write(90 - m_calibration.m.rr.motor1);
      delay(100);
      servo_rr_2.write(90 + m_calibration.m.rr.motor2);
      delay(500);
    }
    // demo mode activated when hand is placed 5cm from sensors during the boot
    if (checkForDemoModeInputs()) {
      // demo mode
      Serial.println("Entering demo mode");
      applyTask(DEMOTASK);
    } else {
      Serial.println("Entering explore mode");
      applyTask(BEGINTASK);
    }
  }
  // reset gyro
  resetGyro();
}

// the loop function runs over and over again forever
void loop() {
  // set motors angle values
  servo_frnt.write(90 - (m_currentSequence[_sequenceCount.fl].front          + m_calibration.front));
  servo_rear.write(90 - (m_currentSequence[_sequenceCount.fl].rear           + m_calibration.rear));
  servo_fl_1.write(90 + (m_currentSequence[_sequenceCount.fl].m.motor1  - 30 + m_calibration.m.fl.motor1 + m_bodyBalance.fl.motor1));
  servo_fl_2.write(90 - (m_currentSequence[_sequenceCount.fl].m.motor2  - 30 + m_calibration.m.fl.motor2 + m_bodyBalance.fl.motor2));
  servo_fr_1.write(90 - (m_currentSequence[_sequenceCount.fr].m.motor1  - 30 + m_calibration.m.fr.motor1 + m_bodyBalance.fr.motor1));
  servo_fr_2.write(90 + (m_currentSequence[_sequenceCount.fr].m.motor2  - 30 + m_calibration.m.fr.motor2 + m_bodyBalance.fr.motor2));
  servo_rl_1.write(90 + (m_currentSequence[_sequenceCount.rl].m.motor1  - 30 + m_calibration.m.rl.motor1 + m_bodyBalance.rl.motor1));
  servo_rl_2.write(90 - (m_currentSequence[_sequenceCount.rl].m.motor2  - 30 + m_calibration.m.rl.motor2 + m_bodyBalance.rl.motor2));
  servo_rr_1.write(90 - (m_currentSequence[_sequenceCount.rr].m.motor1  - 30 + m_calibration.m.rr.motor1 + m_bodyBalance.rr.motor1));
  servo_rr_2.write(90 + (m_currentSequence[_sequenceCount.rr].m.motor2  - 30 + m_calibration.m.rr.motor2 + m_bodyBalance.rr.motor2));
  // walking speed depends of the delay
  delay(_timeDelay); // best value 35
  // read proximity sensors
  updateInputs(_sequenceCount.fl);
  // update gyro readings
  updateGyro(_sequenceCount.fl);
  // update sequence shift 
  if (_sequenceCount.fl < 15) {
    _sequenceCount.fl ++;
    _sequenceCount.rr = _sequenceCount.fl;
    if (m_leftRightShift == 16) {
      // second pair of legs not moving
      // used for stand to walk and walk to stand transition
      _sequenceCount.fr = 0;
      _sequenceCount.rl = 0;
    } else if (_sequenceCount.fl >= m_leftRightShift) {
      _sequenceCount.fr = _sequenceCount.fl - m_leftRightShift;
      _sequenceCount.rl = _sequenceCount.fl - m_leftRightShift;
    } else {
      _sequenceCount.fr = _sequenceCount.fl + m_leftRightShift;
      _sequenceCount.rl = _sequenceCount.fl + m_leftRightShift;
    }
  } else {
    // last sequence in the pattern. Get next pattern
    _sequenceCount.fl = 0;
    _sequenceCount.rr = 0;
    if (m_leftRightShift == 16) {
      _sequenceCount.fr = 0;
      _sequenceCount.rl = 0;
    } else {
      _sequenceCount.fr = m_leftRightShift;
      _sequenceCount.rl = m_leftRightShift;
    }
    // check device mode
    if (_deviceMode == EXPLORE) {
      // explore mode
      // gyro based balance fix
      if (m_gyroEnabled) {
        m_center = fixBalanceGyro(m_center);
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
      applyTask(getTaskByInputs(checkVerticalPositionGyro(), checkBatteryLowInputs()));
    }
    break;
    case STANDWALK:
    {
      if (m_gyroEnabled) {
        updateTurnPattern(getDirectionCorrectionGyro());
      } else {
        updateTurnPattern(0);
      }
    }        
    break;
    case WALKFORWARD:
    {
      // assume WALKFORWARD is deault task
      _newTask = getTaskByInputs(checkVerticalPositionGyro(), checkBatteryLowInputs());
      if (m_defaultTask == _newTask) {
        if (m_gyroEnabled) {
          updateTurnPattern(getDirectionCorrectionGyro());
        } else {
          updateTurnPattern(0);
        }
      } else {
        applyTask(_newTask);
      }
    }
    break;
    case RESETDIRECTION:
    {
      updateWalkingDirectionGyro(-getWalkingDirectionGyro());
    }
    break;
    default:
    break;
  }
}

// factory mode call
void factoryModeCall(unsigned char patternStatus) {
  // update factory stage
  if (readButtonPress(MODE_BTN)) {
    if (_deviceMode < CALIBRATION_START) {
      _deviceMode ++;
    } else {
      _modePressed = true;
    }
    Serial.print("Factory stage set to ");
    Serial.println((int)_deviceMode);
  }
  // factory mode stages
  switch (_deviceMode) {
    case CALIBRATION_INFO: 
      // print proximity sensors
      Serial.print("Left eye ");
      Serial.print((int)analogRead(A0));
      Serial.print(" Right eye ");
      Serial.println((int)analogRead(A1));
      // print gyro data
      Serial.print("Direction ");
      Serial.print((int)getDirectionCorrectionGyro());
      Serial.print(" Nose ");
      Serial.print((int)getNoseDiveGyro());
      Serial.print(" Roll left ");
      Serial.print((int)getRollLeftGyro());
      Serial.print(" Roll right ");
      Serial.println((int)getRollRightGyro());
    break;
    case CALIBRATION_FRONT: 
    {
      Serial.println("Press mode button when front is aligned");
      // update front center motor
      m_calibration.front -= 1;
      if (m_calibration.front < -15) {
        m_calibration.front = 10;
      }
    }
    break;
    case CALIBRATION_REAR:
    {
      Serial.println("Press mode button when rear is aligned");
      // update rear center motor
      m_calibration.rear -= 1;
      if (m_calibration.rear < -15) {
        m_calibration.rear = 10;
      }
    }
    break;
    case CALIBRATION_START: 
    {
      _deviceMode = CALIBRATION_AUTO_1;
      Serial.println("Starting legs calibration");
      applyTask(CALIBRATIONTASK);
    } 
    break;
    case CALIBRATION_AUTO_1: 
    case CALIBRATION_AUTO_2: 
    {
      if (patternStatus == DOCALIBRATION) {
        m_patternCounter = calibrateLegs(m_patternCounter);
      }
    } 
    break;
    case CALIBRATION_SAVE:
    {
      _deviceMode = CALIBRATION_DONE;
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

// detect button pressed
bool _getButtonPressed(void) {
  if (_modePressed) {
    _modePressed = false;
    return true;
  }
  return false;
}

// do legs calibration
unsigned char calibrateLegs(unsigned char patternCounter) {
  //patternCounter = 0;
  if (_deviceMode == CALIBRATION_AUTO_1) {
    // tune motor 1
    switch (_calibrationStage) {
      case 0:
      {
        // leg FL
        if (_calibrationCounter == 0) {
          // set initial leg calibration
          m_calibration.m.fl.motor1 = -20;
          m_calibration.m.fl.motor2 = 0;
          m_calibration.m.fr.motor1 = -20;
          m_calibration.m.fr.motor2 = 0;
          m_calibration.m.rl.motor1 = -20;
          m_calibration.m.rl.motor2 = 0;
          m_calibration.m.rr.motor1 = -20;
          m_calibration.m.rr.motor2 = 0;
          patternCounter ++;
          _calibrationCounter ++;
        } else {
          if (! _getButtonPressed()) {
            m_calibration.m.fl.motor1 += 1;
            if (m_calibration.m.fl.motor1 > 20) {
              m_calibration.m.fl.motor1 = -20;
            }
          } else {
            _calibrationCounter = 0;
            _calibrationStage ++;
          }
        }
      }
      break;
      case 2:
      {
        // leg FR
        if (_calibrationCounter == 0) {
          _calibrationCounter ++;
        } else {
          if (! _getButtonPressed()) {
            m_calibration.m.fr.motor1 += 1;
            if (m_calibration.m.fr.motor1 > 20) {
              m_calibration.m.fr.motor1 = -20;
            }
          } else {
            _calibrationCounter = 0;
            _calibrationStage ++;
          }
        }
      }
      break;
      case 1:
      {
        // leg RL
        if (_calibrationCounter == 0) {
          _calibrationCounter ++;
        } else {
          if (! _getButtonPressed()) {
            m_calibration.m.rl.motor1 += 1;
            if (m_calibration.m.rl.motor1 > 20) {
              m_calibration.m.rl.motor1 = -20;
            }
          } else {
            _calibrationCounter = 0;
            _calibrationStage ++;
          }
        }
      }
      break;
      case 3:
      {
        // leg RR
        if (_calibrationCounter == 0) {
          _calibrationCounter ++;
        } else {
          if (! _getButtonPressed()) {
            m_calibration.m.rr.motor1 += 1;
            if (m_calibration.m.rr.motor1 > 20) {
              m_calibration.m.rr.motor1 = -20;
            }
          } else {
            _calibrationCounter = 0;
            _calibrationStage = 0;
            _deviceMode ++;
          }
        }
      }
      break;
      default:
      break;
    }
  } else if (_deviceMode == CALIBRATION_AUTO_2) {
    // tune motor 2
    switch (_calibrationStage) {
      case 2:
      {
        // leg FL
        if (_calibrationCounter == 0) {
          _calibrationCounter ++;
        } else {
          if (! _getButtonPressed()) {
            m_calibration.m.fl.motor2 += 1;
            if (m_calibration.m.fl.motor2 > 20) {
              m_calibration.m.fl.motor2 = -20;
            }
          } else {
            _calibrationCounter = 0;
            _calibrationStage ++;
          }
        }
      }
      break;
      case 0:
      {
        // leg FR
        if (_calibrationCounter == 0) {
          // set legs balance
          m_calibration.m.fl.motor2 = -20;
          m_calibration.m.fr.motor2 = -20;
          m_calibration.m.rl.motor2 = -20;
          m_calibration.m.rr.motor2 = -20;
          patternCounter ++;
          _calibrationCounter ++;
        } else {
          if (! _getButtonPressed()) {
            m_calibration.m.fr.motor2 += 1;
            if (m_calibration.m.fr.motor2 > 20) {
              m_calibration.m.fr.motor2 = -20;
            }
          } else {
            _calibrationCounter = 0;
            _calibrationStage ++;
          }
        }
      }
      break;
      case 3:
      {
        // leg RL
        if (_calibrationCounter == 0) {
          _calibrationCounter ++;
        } else {
          if (! _getButtonPressed()) {
            m_calibration.m.rl.motor2 += 1;
            if (m_calibration.m.rl.motor2 > 20) {
              m_calibration.m.rl.motor2 = -20;
            }
          } else {
            _calibrationCounter = 0;
            _calibrationStage = 0;
            // end of calibration
            patternCounter ++;
            _deviceMode ++;
          }
        }
      }
      break;
      case 1:
      {
        // leg RR
        if (_calibrationCounter == 0) {
          _calibrationCounter ++;
        } else {
          if (! _getButtonPressed()) {
            m_calibration.m.rr.motor2 += 1;
            if (m_calibration.m.rr.motor2 > 20) {
              m_calibration.m.rr.motor2 = -20;
            }
          } else {
            _calibrationCounter = 0;
            _calibrationStage ++;
          }
        }
      }
      break;
      default:
      break;
    }
  }
  return patternCounter;
}
