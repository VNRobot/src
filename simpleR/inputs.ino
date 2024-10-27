/*
Simple Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Gets analog and digital inputs
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

// digital inputs pins
enum dPinsInputs {
  F_SWITCH = 10
};

// sensor state
enum senState {
  SEN_CLIFF = 0,
  SEN_OBSTACLE = 1,
  SEN_WALL = 2,
  SEN_BLOCK = 3,
  SEN_NORMAL = 4
};

// input state
enum inState {
  IN_LOW_BATTERY,
  IN_HIGH_CURRENT_1,
  IN_HIGH_CURRENT_2,
  IN_HIGH_CURRENT_3,
  IN_TOUCH_FRONTLEFT,
  IN_TOUCH_FRONTRIGHT,
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

unsigned char normalDistance = 20; //cm
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
aSensors analogInputs = {0, 0, 0, 0, 0, 0}; // raw values
aSensors analogValueInputs = {6500, 0, 0, 0, 0, 0}; // processed values

// digital sensors structure
struct dSensors {
  unsigned char f;
};

// digital input values array
dSensors digitalInputs = {0};

// init inputs
void initInputs(void) {
  pinMode(F_SWITCH, INPUT_PULLUP);
}

// read and remember analog sensors readings
unsigned char updateInputs(unsigned char sequenceCount) {
  // read analog inputs
  analogInputs.battery = (unsigned short)analogRead(A6);
  analogInputs.current1 = (unsigned short)analogRead(A7);
  analogInputs.current2 = (unsigned short)analogRead(A3);
  analogInputs.current3 = (unsigned short)analogRead(A2);
  analogInputs.left = (unsigned short)analogRead(A0);
  analogInputs.right = (unsigned short)analogRead(A1);
  // read digital inputs
  digitalInputs.f = (unsigned char)digitalRead(F_SWITCH);
  // calculate sensor current in mA
  // 1
  if (analogInputs.current1 > analogInputs.battery) {
    analogInputs.current1 = analogInputs.battery;
  }
  if (analogValueInputs.current1 > (analogInputs.battery - analogInputs.current1) * 8) {
    analogValueInputs.current1 -= 50;
  } else {
    analogValueInputs.current1 += 50;
  }
  // 2
  if (analogInputs.current2 > analogInputs.battery) {
    analogInputs.current2 = analogInputs.battery;
  }
  if (analogValueInputs.current2 > (analogInputs.battery - analogInputs.current2) * 8) {
    analogValueInputs.current2 -= 50;
  } else {
    analogValueInputs.current2 += 50;
  }
  // 3
  if (analogInputs.current3 > analogInputs.battery) {
    analogInputs.current3 = analogInputs.battery;
  }
  if (analogValueInputs.current3 > (analogInputs.battery - analogInputs.current3) * 8) {
    analogValueInputs.current3 -= 50;
  } else {
    analogValueInputs.current3 += 50;
  }
  // battery in mV
  if (analogValueInputs.battery > analogInputs.battery * 25 / 3) {
    analogValueInputs.battery -= 10;
  } else {
    analogValueInputs.battery += 10;
  }
  // proximity sensors in cm
  // crossconnection left senor is facing right and right sensor is facing left
  analogValueInputs.right = (unsigned short)((1600000 / analogInputs.left) / analogInputs.left);
  analogValueInputs.left = (unsigned short)((1600000 / analogInputs.right) / analogInputs.right);
  //
  allStateInputs = _statusInputs(getSensorState(analogValueInputs.left), getSensorState(analogValueInputs.right));
  //
  if (allStateInputsOld != allStateInputs) {
    allStateInputsOld = allStateInputs;
    // print state
    if (m_debugMode) {
      _printInputs(allStateInputs);
    }
  }
  if (sequenceCount == 0) {
    if (m_debugMode) {
      // print raw data
      _printLineInputs();
    }
  }
  return allStateInputs;
}

// check battery low state
bool checkBatteryLowInputs(void) {
  if (allStateInputs == IN_LOW_BATTERY) {
    return true;
  }
  return false;
}

// process distances
unsigned char getSensorState(unsigned short input) {
  if (input < (normalDistance * 2)) { // no cliff
    if (input > (normalDistance / 6)) { // not blocked
      if (input > (normalDistance / 3)) { // no wall
        if (input > (normalDistance / 2)) { // no obstacle
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
    // cliff
    return SEN_CLIFF;
  }
}

// process sensors return next task name
// could be more complex if remembers previos states
unsigned char getTaskByInputs(accRoll gyroState, unsigned char inputState, unsigned char defaultTask, bool sensorsEnabled) {
  if (inputState == IN_LOW_BATTERY) {
    return STAND_TASK;
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
  if ((gyroState.stateGyro == GYRO_UPSIDEDOWN) || (gyroState.stateGyro == GYRO_FELL_LEFT) || (gyroState.stateGyro == GYRO_FELL_RIGHT) || (gyroState.stateGyro == GYRO_FELL_FRONT) || (gyroState.stateGyro == GYRO_FELL_BACK)) {
    // stop moving
    return STAND_TASK;
  }
  // check sensors enabled
  if (! sensorsEnabled) {
    return defaultTask;
  }
  // obstacle state
  switch (inputState) {
    case IN_TOUCH_FRONTLEFT:
      return GOBACKRIGHT_TASK;
    break;
    case IN_TOUCH_FRONTRIGHT:
      return GOBACKLEFT_TASK;
    break;
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

// print raw data
void _printLineInputs(void) {
  Serial.print(" battery ");
  Serial.print((int)analogValueInputs.battery);
  Serial.print(" C ");
  Serial.print((int)analogValueInputs.current1);
  Serial.print(" F ");
  Serial.print((int)analogValueInputs.current2);
  Serial.print(" R ");
  Serial.print((int)analogValueInputs.current3);
  Serial.print(" left ");
  Serial.print((int)analogValueInputs.left);
  Serial.print(" right ");
  Serial.print((int)analogValueInputs.right);
  Serial.print(" touch ");
  Serial.println((int)digitalInputs.f);
}

// print inputs
void _printInputs(int state) {
  // print input state
  switch (state) {
    case IN_LOW_BATTERY:
      Serial.println(" IN_LOW_BATTERY ");
    break;
    case IN_HIGH_CURRENT_1:
      Serial.println(" IN_HIGH_CURRENT_1 ");
    break;
    case IN_HIGH_CURRENT_2:
      Serial.println(" IN_HIGH_CURRENT_2 ");
    break;
    case IN_HIGH_CURRENT_3:
      Serial.println(" IN_HIGH_CURRENT_3 ");
    break;
    case IN_TOUCH_FRONTLEFT:
      Serial.println(" IN_TOUCH_FRONTLEFT ");
    break;
    case IN_TOUCH_FRONTRIGHT:
      Serial.println(" IN_TOUCH_FRONTRIGHT ");
    break;
    case IN_WALL_FRONTLEFT:
      Serial.println(" IN_WALL_FRONTLEFT ");
    break;
    case IN_WALL_FRONTRIGHT:
      Serial.println(" IN_WALL_FRONTRIGHT ");
    break;
    case IN_WALL_LEFT:
      Serial.println(" IN_WALL_LEFT ");
    break;
    case IN_WALL_RIGHT:
      Serial.println(" IN_WALL_RIGHT ");
    break;
    case IN_OBSTACLE_FRONTLEFT:
      Serial.println(" IN_OBSTACLE_FRONTLEFT ");
    break;
    case IN_OBSTACLE_FRONTRIGHT:
      Serial.println(" IN_OBSTACLE_FRONTRIGHT ");
    break;
    case IN_OBSTACLE_LEFT:
      Serial.println(" IN_OBSTACLE_LEFT ");
    break;
    case IN_OBSTACLE_RIGHT:
      Serial.println(" IN_OBSTACLE_RIGHT ");
    break;
    case IN_NORMAL:
      Serial.println(" IN_NORMAL ");
    break;
    default:
      Serial.println(" Wrong inputs state ");
  }
}

// status of inputs
unsigned char _statusInputs( unsigned short sLeft,  unsigned short sRight) {
  // battery low 6500
  if (analogValueInputs.battery < 6500) {
    return IN_LOW_BATTERY;
  }
  // motor 1 current too high
  if (analogValueInputs.current1 > 800) {
    return IN_HIGH_CURRENT_1;
  }
  // motor 2 current too high
  if (analogValueInputs.current2 > 800) {
    return IN_HIGH_CURRENT_2;
  }
  // motor 3 current too high
  if (analogValueInputs.current3 > 800) {
    return IN_HIGH_CURRENT_3;
  }
  // touch
  if (digitalInputs.f == 0) {
    if (analogValueInputs.left > analogValueInputs.right) {
      turnLeft = true;
    } else {
      turnLeft = false;
    } 
    if (turnLeft) {
      return IN_TOUCH_FRONTRIGHT;
    } else {
      return IN_TOUCH_FRONTLEFT;
    }
  }
  // check sensors
  if ((sLeft == SEN_NORMAL) && (sRight == SEN_NORMAL)) {
    return IN_NORMAL;
  }
  if (sRight == SEN_NORMAL) {
    // only left side obstacle
    turnLeft = false;
    if (sLeft == SEN_BLOCK) {
      turnLeft = true;
      return IN_WALL_RIGHT;
    }
    if (sLeft == SEN_WALL) {
      return IN_WALL_LEFT;
    } else {
      return IN_OBSTACLE_LEFT;
    }
  }
  if (sLeft == SEN_NORMAL) {
    // only right side obstacle
    turnLeft = true;
    if (sRight == SEN_BLOCK) {
      turnLeft = false;
      return IN_WALL_LEFT;
    }
    if (sRight == SEN_WALL) {
      return IN_WALL_RIGHT;
    } else {
      //SEN_OBSTACLE
      return IN_OBSTACLE_RIGHT;
    }
  }
  // both sensors obstacle or cliff
  if ((sLeft == SEN_CLIFF) || (sRight == SEN_CLIFF)) {
    if (turnLeft) {
      return IN_WALL_FRONTRIGHT;
    } else {
      return IN_WALL_FRONTLEFT;
    }
  }
  // both sensors obstacle and blocked
  if ((sLeft == SEN_BLOCK) || (sRight == SEN_BLOCK)) {
    if (turnLeft) {
      return IN_WALL_FRONTRIGHT;
    } else {
      return IN_WALL_FRONTLEFT;
    }
  }
  // both sensors wall or obstacle
  if (analogValueInputs.left > analogValueInputs.right) {
    turnLeft = true;
  } else {
    turnLeft = false;
  } 
  if ((sLeft == SEN_WALL) || (sRight == SEN_WALL)) {
    if (turnLeft) {
      return IN_WALL_FRONTRIGHT;
    } else {
      return IN_WALL_FRONTLEFT;
    }
  } else {
    if (turnLeft) {
      return IN_OBSTACLE_FRONTRIGHT;
    } else {
      return IN_OBSTACLE_FRONTLEFT;
    }
  }
  // normal
  return IN_NORMAL;
}

// check for demo mode
bool checkForDemoModeInputs(void) {
    // sensors are blocked 500 ~ 5cm
    if (analogInputs.left > 400 || analogInputs.right > 400) {
        return true;
    }
    return false;
}

// get current of center motors
unsigned short getCurrent1Inputs(void) {
  return analogValueInputs.current1;
}

// get current of front motors
unsigned short getCurrent2Inputs(void) {
  return analogValueInputs.current2;
}

// get current of rear motors
unsigned short getCurrent3Inputs(void) {
  return analogValueInputs.current3;
}

// check mode button pressed
bool checkModeButtonPressedInputs(void) {
  // button is pressed if lower than 400
  if (analogRead(A6) < 400) {
    return true;
  }
  return false;
}
