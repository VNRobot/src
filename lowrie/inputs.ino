/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Process analog inputs
 - Reads analog sensors raw values and converts to cm
    cm     value
    5      640
    10     420
    15     320
    20     260
    25     250
    60     100
*/

#define SENSOR_DISTANCE_BLOCK   600    // if > blocked
#define SENSOR_DISTANCE_MIN     300    // if > wall
#define SENSOR_DISTANCE_NORM    220    // if > obstacle
// norm 120
#define SENSOR_DISTANCE_MAX     100    // if < edge
#define SENSOR_NO_DATA          80     // no data
// extra sensors hight in mm 35
#define EXTRA_DISTANCE_NORM     35
#define EXTRA_END_CYCLE         28
#define EXTRA_COUNT_CYCLE       8

// sensor state
enum senState {
  SEN_NORMAL   = 0,
  SEN_OBSTACLE = 1,
  SEN_WALL     = 2,
  SEN_BLOCK    = 3
};

// robot state structure
typedef struct roboInputState {
  bool sensorsEnabled;
  bool obstacleEnabled;
} roboInputState;

// robot state
roboInputState riState = {
  true,             // bool sensorsEnabled;
  true             // bool obstacleEnabled;
};

// edge enabled
bool edgeEnabled = false;
// extra inputs enabled
bool extraInputsEnabled = true;
// sensors values sum
int leftSensorValueSum = 0;
int rightSensorValueSum = 0;
int leftExtraSensorValueSum = 0;
int rightExtraSensorValueSum = 0;
// sensors values
unsigned short leftSensorValue = SENSOR_DISTANCE_NORM;
unsigned short rightSensorValue = SENSOR_DISTANCE_NORM;
short leftExtraSensorValue = 0;
short rightExtraSensorValue = 0;
short leftExtraSensorAverage = 0;
short rightExtraSensorAverage = 0;
// assume wall angle
short wallAngle = 0;
// sensors state
unsigned char sensorStateLeft = SEN_NORMAL;
unsigned char sensorStateRight = SEN_NORMAL;
// inputs state
unsigned char inputStateNow = IN_NORMAL;

/*
uses
m_getButtonPressed()
*/

// process data distance
unsigned char _getStateFromRaw(unsigned short inputValue) {
  if (inputValue > SENSOR_DISTANCE_BLOCK) {
    return SEN_BLOCK;
  }
  if (inputValue > SENSOR_DISTANCE_MIN) {
    return SEN_WALL;
  }
  if (inputValue > SENSOR_DISTANCE_NORM) {
    if (riState.obstacleEnabled) {
      return SEN_OBSTACLE;
    }
  }
  if (inputValue < SENSOR_DISTANCE_MAX) {
    if (edgeEnabled) {
      return SEN_OBSTACLE;
    }
  }
  return SEN_NORMAL;
}

// status of inputs
unsigned char _getInputState(unsigned char left, unsigned char right) {
  // bits  fl    fr
  //       8  4  2  1 
  char bits = left * 4 + right;
  switch (bits) {
    //          left          right
    case 11: // SEN_WALL      SEN_BLOCK
    case 7: // SEN_OBSTACLE   SEN_BLOCK
    case 9: // SEN_WALL       SEN_OBSTACLE
      return IN_OBSTACLE_FRONTLEFT;
    break;
    case 14: // SEN_BLOCK     SEN_WALL
    case 13: // SEN_BLOCK     SEN_OBSTACLE
    case 6: // SEN_OBSTACLE   SEN_WALL
      return IN_OBSTACLE_FRONTRIGHT;
    break;
    case 15: // SEN_BLOCK     SEN_BLOCK
    case 10: // SEN_WALL      SEN_WALL
      return IN_OBSTACLE_FRONT;
    break;
    case 3: // SEN_NORMAL     SEN_BLOCK
    case 8: // SEN_WALL       SEN_NORMAL
      return IN_OBSTACLE_LEFT;
    break;
    case 4: // SEN_OBSTACLE   SEN_NORMAL
      return IN_FAR_OBSTACLE_LEFT;
    case 5: // SEN_OBSTACLE   SEN_OBSTACLE
      return IN_FAR_OBSTACLE_FRONT;
    break;
    case 12: // SEN_BLOCK     SEN_NORMAL
    case 2: // SEN_NORMAL     SEN_WALL
      return IN_OBSTACLE_RIGHT;
    break;
    case 1: // SEN_NORMAL     SEN_OBSTACLE
      return IN_FAR_OBSTACLE_RIGHT;
    break;
    case 0: // SEN_NORMAL     SEN_NORMAL
      return IN_NORMAL;             
    break;
    default:
    break;
  }
  return IN_NORMAL;
}

// get shifted counter
unsigned char getShiftedCounterInputs(unsigned char counter) {
  unsigned char shiftedCount = counter + MAIN_HALF_CYCLE;
  if (shiftedCount >= MAIN_FULL_CYCLE) {
    shiftedCount -= MAIN_FULL_CYCLE;
  }
  return shiftedCount;
}


// init inputs
void initInputs(bool calibrationMode) {
  Serial.println(F("initInputs"));
  unsigned char counter = 0;
  while (calibrationMode) {
    counter ++;
    if (counter >= MAIN_FULL_CYCLE) {
      counter = 0;
    }
    delay(10);
    if (m_getButtonPressed()) {
      calibrationMode = false;
    }
    updateInputsCount(counter, getShiftedCounterInputs(counter), counter);
    if (counter == 0) {
      _printSensorState(sensorStateLeft);
      _printSensorState(sensorStateRight);
      _printInputState();
      if (extraInputsEnabled) {
        Serial.print(F(" Extra left "));
        Serial.print((int)getExtraInputLeft());
        Serial.print(F(" Extra Right "));
        Serial.println((int)getExtraInputRight());
      }
    }
  }
}

// update sensor readings
void updateInputsCount(unsigned char counter, unsigned char counterFL, unsigned char counterFR) {
  if (extraInputsEnabled) {
    // extra left average
    if ((counterFL > EXTRA_END_CYCLE) || (counterFL == 0)) {
      leftExtraSensorValueSum += analogRead(A3);
    }
    if (counterFL == 0) {
      if (leftExtraSensorAverage < leftExtraSensorValue) {
        leftExtraSensorAverage ++;
      } else if (leftExtraSensorAverage > leftExtraSensorValue) {
        leftExtraSensorAverage --;
      }
      leftExtraSensorValue = (950 - leftExtraSensorValueSum / EXTRA_COUNT_CYCLE) / 4 - EXTRA_DISTANCE_NORM - HIGHT_DEFAULT / ROBOT_SIZE_DEVIDER;
      leftExtraSensorValueSum = 0;
    }
    // extra right average
    if ((counterFR > EXTRA_END_CYCLE) || (counterFR == 0)) {
      rightExtraSensorValueSum += analogRead(A2);
    }
    if (counterFR == 0) {
      if (rightExtraSensorAverage < rightExtraSensorValue) {
        rightExtraSensorAverage ++;
      } else if (rightExtraSensorAverage > rightExtraSensorValue) {
        rightExtraSensorAverage --;
      }
      rightExtraSensorValue = (950 - rightExtraSensorValueSum / EXTRA_COUNT_CYCLE) / 4 - EXTRA_DISTANCE_NORM - HIGHT_DEFAULT / ROBOT_SIZE_DEVIDER;
      rightExtraSensorValueSum = 0;
    }
  }
  // input sensors average
  if ((counter > MAIN_HALF_CYCLE) || (counter == 0)) {
    leftSensorValueSum += analogRead(A0);
    rightSensorValueSum += analogRead(A1);
  }
  if (counter == 0) {
    leftSensorValue = leftSensorValueSum / MAIN_HALF_CYCLE;
    rightSensorValue = rightSensorValueSum / MAIN_HALF_CYCLE;
    leftSensorValueSum = 0;
    rightSensorValueSum = 0;
    // process analog sensors
    if (riState.sensorsEnabled) {
      if ((rightSensorValue > SENSOR_NO_DATA) || (leftSensorValue > SENSOR_NO_DATA)) {
        // data received
        wallAngle = - rightSensorValue + leftSensorValue;
        if (wallAngle > 80) {
          wallAngle = 80;
        } else if (wallAngle < -80) {
          wallAngle = -80;
        }
        //
        sensorStateRight = _getStateFromRaw(rightSensorValue); // right
        sensorStateLeft = _getStateFromRaw(leftSensorValue); // left
        // get input state
        inputStateNow = _getInputState(sensorStateLeft, sensorStateRight);
      } else {
        // no data
        wallAngle = 0;
        if (edgeEnabled) {
          inputStateNow = IN_OBSTACLE_FRONT;
        } else {
          inputStateNow = IN_NORMAL;
        }
      }
    } else {
      inputStateNow = IN_NORMAL;
    }
    //_printSensorState(sensorStateLeft);
    //_printSensorState(sensorStateRight);
    //_printInputState();
  }
}

// get extra input left. negative - bump in mm
short getExtraInputLeft(void) {
  return (short)(leftExtraSensorValue - leftExtraSensorAverage); //
}

// get extra input right. negative - bump in mm
short getExtraInputRight(void) {
  return (short)(rightExtraSensorValue - rightExtraSensorAverage); //
}

// get wall angle
short getWallAngleInputs(void) {
  return wallAngle;
}

// get inputs state
unsigned char getInputState(void) {
  return inputStateNow;
}

// set robot state
void setStateInputs(unsigned char newState) {
  switch (newState) {
    case ROBOT_NORM:
    {
      riState.sensorsEnabled = true;
      riState.obstacleEnabled = true;
    }
    break;
    case ROBOT_INO:
    {
      riState.sensorsEnabled = true;
      riState.obstacleEnabled = false;
    }
    break;
    case ROBOT_CRAWL:
    {
      riState.sensorsEnabled = false;
      riState.obstacleEnabled = false;
    }
    break;
    default:
    break;
  }
}

// print input state
void _printInputState(void) {
  switch (inputStateNow) {
    case IN_OBSTACLE_FRONT:
      Serial.print(F(" IN_OBSTACLE_FRONT "));
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
    case IN_FAR_OBSTACLE_FRONT:
      Serial.print(F(" IN_FAR_OBSTACLE_FRONT "));
    break;
    case IN_FAR_OBSTACLE_LEFT:
      Serial.print(F(" IN_FAR_OBSTACLE_LEFT "));
    break;
    case IN_FAR_OBSTACLE_RIGHT:
      Serial.print(F(" IN_FAR_OBSTACLE_RIGHT "));
    break;
    case IN_NORMAL:
      Serial.print(F(" IN_NORMAL "));
    break;
    default:
      Serial.print(F(" Wrong input state "));
  }
  Serial.print(F(" left "));
  Serial.print((int)analogRead(A0));
  Serial.print(F(" Right "));
  Serial.print((int)analogRead(A1));
  Serial.print(F(" Angle "));
  Serial.println((int)wallAngle);
}

// print input state
void _printSensorState(unsigned char senState) {
  switch (senState) {
    case SEN_BLOCK:
      Serial.print(F(" SEN_BLOCK "));
    break;
    case SEN_WALL:
      Serial.print(F(" SEN_WALL "));
    break;
    case SEN_OBSTACLE:
      Serial.print(F(" SEN_OBSTACLE "));
    break;
    case SEN_NORMAL:
      Serial.print(F(" SEN_NORMAL "));
    break;
    default:
      Serial.print(F(" Wrong sensor state "));
  }
}
