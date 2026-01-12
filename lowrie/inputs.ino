/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Gets analog inputs
 - Reads power input voltage im mV
 - Reads power current from 3 independed channels in mA
    current1: power current on connectors D2, D3, D4, D5
    current2: power current on connectors D6, D7, D8, D9
    current3: power current on connectors D10, D11, D12, D13
 - Reads analog sensors raw values and converts to cm
    cm     value
    5      640
    10     420
    15     320
    20     260
    25     250
    60     100

*/

// sensor state
enum senState {
  SEN_EDGE = 0,
  SEN_OBSTACLE = 1,
  SEN_WALL = 2,
  SEN_BLOCK = 3,
  SEN_NORMAL = 4
};

//----------------configuration flags-----------------
// edge detection enabled
bool edgeEnabled = false;
// current sensor enabled
bool currentEnabled = false;
// low battery sensor enabled
bool voltageEnabled = false;
//----------------------------------------------------

unsigned char allStateInputs = IN_NORMAL;
unsigned char allStateInputsOld = IN_NORMAL;
// turn left or right decision
bool turnLeft = true;

// analog sensors structure
struct aSensors {
  unsigned short battery;   // A6
  unsigned short current1;  // A7
  unsigned short current2;  // A3
  unsigned short current3;  // A2
  unsigned short left;      // A0
  unsigned short right;     // A1
};

// analog input values array
aSensors analogRawInputs = {0, 0, 0, 0, 0, 0}; // raw values
aSensors analogValueInputs = {6000, 0, 0, 0, 0, 0}; // processed values
// touch data
touch legTouch = {true, true, true};

// init inputs
void initInputs(void) {
}

// read and remember analog sensors readings
unsigned char updateInputs(unsigned char sequenceCount, bool sensorsEnabled, char direction) {
  // read analog inputs
  analogRawInputs.battery = (unsigned short)analogRead(A6);
  analogRawInputs.current1 = (unsigned short)analogRead(A7);
  analogRawInputs.current2 = (unsigned short)analogRead(A3);
  analogRawInputs.current3 = (unsigned short)analogRead(A2);
  analogRawInputs.left = (unsigned short)analogRead(A0);
  analogRawInputs.right = (unsigned short)analogRead(A1);
  // calculate sensor current in mA
  // 1
  if (analogRawInputs.current1 > analogRawInputs.battery) {
    analogRawInputs.current1 = analogRawInputs.battery;
  }
  if (analogRawInputs.current1 > INPUT_GROUNDED) { // not grounded // devides 2.36  880/373 or * 19 instead of 8
    if (analogValueInputs.current1 > (analogRawInputs.battery - analogRawInputs.current1) * 8) {
      analogValueInputs.current1 -= 50;
    } else {
      analogValueInputs.current1 += 50;
    }
  }
  // 2
  if (analogRawInputs.current2 > analogRawInputs.battery) {
    analogRawInputs.current2 = analogRawInputs.battery;
  }
  if (analogRawInputs.current2 > INPUT_GROUNDED) { // not grounded
    if (analogValueInputs.current2 > (analogRawInputs.battery - analogRawInputs.current2) * 8) {
      analogValueInputs.current2 -= 50;
    } else {
      analogValueInputs.current2 += 50;
    }
  }
  // 3
  if (analogRawInputs.current3 > analogRawInputs.battery) {
    analogRawInputs.current3 = analogRawInputs.battery;
  }
  if (analogRawInputs.current3 > INPUT_GROUNDED) { // not grounded
    if (analogValueInputs.current3 > (analogRawInputs.battery - analogRawInputs.current3) * 8) {
      analogValueInputs.current3 -= 50;
    } else {
      analogValueInputs.current3 += 50;
    }
  }
  // battery in mV
  if (analogValueInputs.battery > analogRawInputs.battery * 25 / 3) {
    analogValueInputs.battery -= 10;
  } else {
    analogValueInputs.battery += 10;
  }
  // proximity sensors in cm
  // crossconnection left senor is facing right and right sensor is facing left
  analogValueInputs.right = (unsigned short)((1600000 / analogRawInputs.left) / analogRawInputs.left);
  analogValueInputs.left = (unsigned short)((1600000 / analogRawInputs.right) / analogRawInputs.right);
  //
  if (sequenceCount == 0) {
    allStateInputs = _statusInputs(getSensorState(analogValueInputs.left, sensorsEnabled), getSensorState(analogValueInputs.right, sensorsEnabled), direction);
    if (allStateInputsOld != allStateInputs) {
      allStateInputsOld = allStateInputs;
      // debug print
      //_printInputs(allStateInputs);
    }
    // debug print
    //if (sequenceCount == 0) {
    //  _printLineInputs();
    //}
  }
  return allStateInputs;
}

// update touch state
touch getTouchInputs(void) {
  analogRawInputs.current1 = (unsigned short)analogRead(A7);
  analogRawInputs.current2 = (unsigned short)analogRead(A3);
  analogRawInputs.current3 = (unsigned short)analogRead(A2);
  if (analogRawInputs.current1 > INPUT_GROUNDED) { // not grounded
    legTouch.set1 = false;
  } else { // grounded
    legTouch.set1 = true;
  }
  // 2
  if (analogRawInputs.current2 > INPUT_GROUNDED) { // not grounded
    legTouch.set2 = false;
  } else { // grounded
    legTouch.set2 = true;
  }
  // 3
  if (analogRawInputs.current3 > INPUT_GROUNDED) { // not grounded
    legTouch.set3 = false;
  } else { // grounded
    legTouch.set3 = true;
  }
  return legTouch;
}

// process distances
unsigned char getSensorState(unsigned short input, bool sensorsEnabled) {
  if (sensorsEnabled) {
    if (input < (m_robotState.inputDistanceNow * 4)) { // no edge
      if (input > (m_robotState.inputDistanceNow / 6)) { // not blocked
        if (input > (m_robotState.inputDistanceNow / 3)) { // no wall
          if (input > (m_robotState.inputDistanceNow / 2)) { // no obstacle
            return SEN_NORMAL;
          } else {
            // obstacle
            return SEN_OBSTACLE;
          }
        } else {
          // wall
          return SEN_WALL;
        }
      } else {
        // blocked
        return SEN_BLOCK;
      }
    } else {
      // edge
      if (edgeEnabled) {
        return SEN_EDGE;
      } else {
        return SEN_NORMAL;
      }
    }
  } else {
    return SEN_NORMAL;
  }
}

unsigned char getHighPriorityTaskByInputs(accRoll gyroState, unsigned char inputState) {
  if (inputState == IN_LOW_BATTERY) {
    return DOWN_TASK;
  }
  if (inputState == IN_HIGH_CURRENT_1) {
    return STAND_TASK;
  }
  if (inputState == IN_HIGH_CURRENT_2) {
    return STAND_TASK;
  }
  if (inputState == IN_HIGH_CURRENT_3) {
    return STAND_TASK;
  }
  switch (gyroState.stateGyro) {
    case GYRO_UPSIDEDOWN:
      return FLIP_TASK;
    break;
    case GYRO_FELL_LEFT:
      return RECOVER_LEFT_TASK;
    break;
    case GYRO_FELL_RIGHT:
      return RECOVER_RIGHT_TASK;
    break;
    case GYRO_FELL_FRONT:
      return FLIP_TASK;
    break;
    case GYRO_FELL_BACK:
      return FLIP_TASK;
    break;
    case GYRO_FOLLING_LEFT:
      return RECOVER_LEFT_TASK;
    break;
    case GYRO_FOLLING_RIGHT:
      return RECOVER_RIGHT_TASK;
    break;
    case GYRO_FOLLING_FRONT:
      return DEFAULT_TASK;
    break;
    case GYRO_FOLLING_BACK:
      return DEFAULT_TASK;
    break;
    default:
    break;
  }
  return DEFAULT_TASK;
}

// process sensors return next task name
// could be more complex if remembers previos states
unsigned char getNormalTaskByInputs(unsigned char inputState, unsigned char defaultTask) {
  // obstacle state
  switch (inputState) {
    case IN_WALL_FRONTLEFT:
      return GOBACKRIGHT_TASK;
    break;
    case IN_WALL_FRONTRIGHT:
      return GOBACKLEFT_TASK;
    break;
    case IN_WALL_LEFT:
      return STANDTURNRIGHT2_TASK;
    break;
    case IN_WALL_RIGHT:
      return STANDTURNLEFT2_TASK;
    break;
    case IN_OBSTACLE_FRONTLEFT:
      return STANDTURNRIGHT_TASK;
    break;
    case IN_OBSTACLE_FRONTRIGHT:
      return STANDTURNLEFT_TASK;
    break;
    case IN_OBSTACLE_LEFT:
      return GOTURNRIGHT_TASK;
    break;
    case IN_OBSTACLE_RIGHT:
      return GOTURNLEFT_TASK;
    break;
    case IN_NORMAL:
      return defaultTask;
    break;
    default:
      return defaultTask;
    break;
  }
  return defaultTask;
}

// status of inputs
unsigned char _statusInputs( unsigned short sLeft,  unsigned short sRight, char direction) {
  if (voltageEnabled) {
    // battery low
    if (analogValueInputs.battery < LOW_BATTERY) {
      return IN_LOW_BATTERY;
    }
  }
  if (currentEnabled) {
    // motor 1 current too high
    if (analogValueInputs.current1 > m_init.maxInputCurrent) {
      return IN_HIGH_CURRENT_1;
    }
    // motor 2 current too high
    if (analogValueInputs.current2 > m_init.maxInputCurrent) {
      return IN_HIGH_CURRENT_2;
    }
    // motor 3 current too high
    if (analogValueInputs.current3 > m_init.maxInputCurrent) {
      return IN_HIGH_CURRENT_3;
    }
  }
  // both sensors normal
  if ((sLeft == SEN_NORMAL) && (sRight == SEN_NORMAL)) {
    if (direction > 50) {
      turnLeft = false;
      return IN_WALL_FRONTLEFT;
    }
    if (direction < -50) {
      turnLeft = true;
      return IN_WALL_FRONTRIGHT;
    }
    return IN_NORMAL;
  }
  // both sensors blocked
  if ((sLeft == SEN_BLOCK) && (sRight == SEN_BLOCK)) {
    if (analogValueInputs.left > analogValueInputs.right) {
      turnLeft = false;
      return IN_WALL_FRONTRIGHT;
    } else {
      turnLeft = true;
      return IN_WALL_FRONTRIGHT;
    } 
  }
  // both sensors wall or both sensors edge
  if (((sLeft == SEN_WALL) && (sRight == SEN_WALL)) || ((sLeft == SEN_EDGE) && (sRight == SEN_EDGE))) {
    if (turnLeft) {
      return IN_WALL_FRONTRIGHT;
    } else {
      return IN_WALL_FRONTLEFT;
    }
  }
  // both sensors obstacle
  if ((sLeft == SEN_OBSTACLE) && (sRight == SEN_OBSTACLE)) {
    if (analogValueInputs.left > analogValueInputs.right) {
      turnLeft = true;
      return IN_OBSTACLE_FRONTRIGHT;
    } else {
      turnLeft = false;
      return IN_OBSTACLE_FRONTLEFT;
    } 
  }
  //SEN_EDGE, SEN_OBSTACLE, SEN_WALL, SEN_BLOCK, SEN_NORMAL
  // right is normal
  if (sRight == SEN_NORMAL) {
    // only left side obstacle
    turnLeft = false;
    if (sLeft == SEN_BLOCK) {
      turnLeft = true;
      return IN_WALL_RIGHT;
    }
    if ((sLeft == SEN_WALL) || (sLeft == SEN_EDGE)) {
      return IN_WALL_LEFT;
    } else {
      //SEN_OBSTACLE
      return IN_OBSTACLE_LEFT;
    }
  }
  // left is normal
  if (sLeft == SEN_NORMAL) {
    // only right side obstacle
    turnLeft = true;
    if (sRight == SEN_BLOCK) {
      turnLeft = false;
      return IN_WALL_LEFT;
    }
    if ((sRight == SEN_WALL) || (sRight == SEN_EDGE)) {
      return IN_WALL_RIGHT;
    } else {
      //SEN_OBSTACLE
      return IN_OBSTACLE_RIGHT;
    }
  }
  //SEN_EDGE, SEN_OBSTACLE, SEN_WALL, SEN_BLOCK
  if (sLeft == SEN_BLOCK) {
    turnLeft = true;
    return IN_WALL_FRONTRIGHT;
  }
  if (sRight == SEN_BLOCK) {
    turnLeft = false;
    return IN_WALL_FRONTLEFT;
  }
  //SEN_EDGE, SEN_OBSTACLE, SEN_WALL
  if ((sLeft == SEN_WALL) || (sLeft == SEN_EDGE)) {
    turnLeft = false;
    return IN_WALL_FRONTLEFT;
  }
  if ((sRight == SEN_WALL) || (sRight == SEN_EDGE)) {
    turnLeft = true;
    return IN_WALL_FRONTRIGHT;
  }
  // should not go here
  if (analogValueInputs.left > analogValueInputs.right) {
    turnLeft = true;
  } else {
    turnLeft = false;
  } 
  return IN_NORMAL;
}

// check mode button pressed
bool checkModeButtonPressedInputs(void) {
  // button is pressed if lower than 400
  if (analogRead(A6) < INPUT_GROUNDED) {
    return true;
  }
  return false;
}
/*
// print raw data
void _printLineInputs(void) {
  Serial.print(F(" battery "));
  Serial.print((int)analogValueInputs.battery);
  Serial.print(F(" C "));
  Serial.print((int)analogValueInputs.current1);
  Serial.print(F(" F "));
  Serial.print((int)analogValueInputs.current2);
  Serial.print(F(" R "));
  Serial.print((int)analogValueInputs.current3);
  Serial.print(F(" left "));
  Serial.print((int)analogValueInputs.left);
  Serial.print(F(" right "));
  Serial.println((int)analogValueInputs.right);
}

// print inputs
void _printInputs(int state) {
  // print input state
  switch (state) {
    case IN_LOW_BATTERY:
      Serial.print(F(" IN_LOW_BATTERY "));
    break;
    case IN_HIGH_CURRENT_1:
      Serial.print(F(" IN_HIGH_CURRENT_1 "));
    break;
    case IN_HIGH_CURRENT_2:
      Serial.print(F(" IN_HIGH_CURRENT_2 "));
    break;
    case IN_HIGH_CURRENT_3:
      Serial.print(F(" IN_HIGH_CURRENT_3 "));
    break;
    case IN_WALL_FRONTLEFT:
      Serial.print(F(" IN_WALL_FRONTLEFT "));
    break;
    case IN_WALL_FRONTRIGHT:
      Serial.print(F(" IN_WALL_FRONTRIGHT "));
    break;
    case IN_WALL_LEFT:
      Serial.print(F(" IN_WALL_LEFT "));
    break;
    case IN_WALL_RIGHT:
      Serial.print(F(" IN_WALL_RIGHT "));
    break;
    case IN_OBSTACLE_FRONTLEFT:
      Serial.print(F(" IN_OBSTACLE_FRONTLEFT "));
    break;
    case IN_OBSTACLE_FRONTRIGHT:
      Serial.print(F(" IN_OBSTACLE_FRONTRIGHT "));
    break;
    case IN_OBSTACLE_LEFT:
      Serial.print(F(" IN_OBSTACLE_LEFT "));
    break;
    case IN_OBSTACLE_RIGHT:
      Serial.print(F(" IN_OBSTACLE_RIGHT "));
    break;
    case IN_NORMAL:
      Serial.print(F(" IN_NORMAL "));
    break;
    default:
      Serial.println(F(" Wrong inputs state "));
  }
}
*/