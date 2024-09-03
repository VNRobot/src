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

// digital inputs pins
enum dPinsInputs {
  F_SWITCH = 3
};

// sensor state
enum senState {
  SEN_CLIF = 0,
  SEN_OBSTACLE = 1,
  SEN_WALL = 2,
  SEN_BLOCK = 3,
  SEN_NORMAL = 4
};

// input state
enum inState {
  IN_LOW_BATTERY = 0,        // battery low
  IN_HIGH_CURRENT_1 = 1,     // A7 center motor current too high
  IN_HIGH_CURRENT_2 = 2,     // A3 front motor current too high
  IN_HIGH_CURRENT_3 = 3,     // A2 rear motor current too high
  IN_TOUCH_FRONT = 4,        // touch front
  IN_WALL_FRONT = 5,
  IN_WALL_LEFT = 6,
  IN_WALL_RIGHT = 7,
  IN_OBSTACLE_FRONT = 8,
  IN_OBSTACLE_LEFT = 9,
  IN_OBSTACLE_RIGHT = 10,
  IN_NORMAL = 11             // normal
};

unsigned char allStateInputs = IN_NORMAL;
unsigned char allStateInputsOld = IN_NORMAL;

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
  // calculate analog input current in values in mA
  // 1
  if (analogInputs.current1 > analogInputs.battery) {
    analogInputs.current1 = analogInputs.battery;
  }
  analogValueInputs.current1 = (analogValueInputs.current1 * 7 + (analogInputs.battery - analogInputs.current1) * 8) / 8;
  // 2
  if (analogInputs.current2 > analogInputs.battery) {
    analogInputs.current2 = analogInputs.battery;
  }
  analogValueInputs.current2 = (analogValueInputs.current2 * 7 + (analogInputs.battery - analogInputs.current2) * 8) / 8;
  // 3
  if (analogInputs.current3 > analogInputs.battery) {
    analogInputs.current3 = analogInputs.battery;
  }
  analogValueInputs.current3 = (analogValueInputs.current3 * 7 + (analogInputs.battery - analogInputs.current3) * 8) / 8;
  // battery in mV
  analogValueInputs.battery = (analogValueInputs.battery * 7 + analogInputs.battery * 25 / 3) / 8;
  // proximity sensors in cm
  analogValueInputs.left = (analogValueInputs.left * 3 + (unsigned short)((1600000 / analogInputs.left) / analogInputs.left)) / 4;
  analogValueInputs.right = (analogValueInputs.right * 3 + (unsigned short)((1600000 / analogInputs.right) / analogInputs.right)) / 4;
  //
  allStateInputs = _statusInputs(getSensorState(analogValueInputs.left), getSensorState(analogValueInputs.right));
  //
  if (allStateInputsOld != allStateInputs) {
    allStateInputsOld = allStateInputs;
    // print raw data
    // _printLineInputs();
    _printInputs(allStateInputs);
  }
  if (sequenceCount == 0) {
    _printLineInputs();
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
  if (input < 100) { // 60
    if (input > 5) {
      if (input > 15) {
        if (input > 30) { // 20
          // normal 30 - 70
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
      // block
      return SEN_BLOCK;
    }
  } else {
    // clif
    return SEN_CLIF;
  }
}

// process sensors return next task name
// could be more complex if remembers previos states
unsigned char getTaskByInputs(unsigned char gyroState, unsigned char inputState, unsigned char defaultTask, bool sensorsEnabled) {
  if (inputState == IN_LOW_BATTERY) {
    return DOWNTASK;
  }
  if (inputState == IN_HIGH_CURRENT_1) {
    return STANDTASK;
  }
  if (inputState == IN_HIGH_CURRENT_2) {
    return STANDTASK;
  }
  if (inputState == IN_HIGH_CURRENT_3) {
    return STANDTASK;
  }
  if ((gyroState == GYRO_UPSIDEDOWN) || (gyroState == GYRO_FELL_LEFT) || (gyroState == GYRO_FELL_RIGHT) || (gyroState == GYRO_FELL_FRONT) || (gyroState == GYRO_FELL_BACK)) {
    // stop moving
    return STANDTASK;
  }
  // check sensors enabled
  if (! sensorsEnabled) {
    return defaultTask;
  }
  // obstacle state
  switch (inputState) {
    case IN_WALL_FRONT:
      return WALKBACKTASK; //TURNAROUNDTASK;
    break;
    case IN_WALL_LEFT:
      return STANDTURNRIGHT2TASK;
    break;
    case IN_WALL_RIGHT:
      return STANDTURNLEFT2TASK;
    break;
    case IN_OBSTACLE_FRONT:
      {
        if (analogValueInputs.right > analogValueInputs.left) {
          return STANDTURNLEFTTASK;
        } else {
          return STANDTURNRIGHTTASK;
        }
      }
    break;
    case IN_OBSTACLE_LEFT:
      return WALKTURNRIGHTTASK;
    break;
    case IN_OBSTACLE_RIGHT:
      return WALKTURNLEFTTASK;
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
    case IN_TOUCH_FRONT:
      Serial.println(" IN_TOUCH_FRONT ");
    break;
    case IN_WALL_FRONT:
      Serial.println(" IN_WALL_FRONT ");
    break;
    case IN_WALL_LEFT:
      Serial.println(" IN_WALL_LEFT ");
    break;
    case IN_WALL_RIGHT:
      Serial.println(" IN_WALL_RIGHT ");
    break;
    case IN_OBSTACLE_FRONT:
      Serial.println(" IN_OBSTACLE_FRONT ");
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
    return IN_TOUCH_FRONT;
  }
  // check sensors
  if ((sLeft == SEN_NORMAL) && (sRight == SEN_NORMAL)) {
    return IN_NORMAL;
  }
  if (sRight == SEN_NORMAL) {
    // only right sensor obstacle
    if ((sLeft == SEN_BLOCK) || (sLeft == SEN_WALL)) {
      return IN_WALL_RIGHT;
    } else {
      return IN_OBSTACLE_RIGHT;
    }
  }
  if (sLeft == SEN_NORMAL) {
    // only left sensor obstacle
    if ((sRight == SEN_BLOCK) || (sRight == SEN_WALL)) {
      return IN_WALL_LEFT;
    } else {
      return IN_OBSTACLE_LEFT;
    }
  }
  // both sensors obstacle
  if ((sLeft == SEN_BLOCK) || (sLeft == SEN_WALL) || (sRight == SEN_BLOCK) || (sRight == SEN_WALL)) {
    return IN_WALL_FRONT;
  } else {
    return IN_OBSTACLE_FRONT;
  }
  // normal
  return IN_NORMAL;
}

// check for calibration mode
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

