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
#define SENSOR_DISTANCE_NORM    200    // if > obstacle
// norm 120
#define SENSOR_DISTANCE_MAX     100    // if < edge
#define SENSOR_NO_DATA          80     // no data
// extra sensors hight in mm 35
#define EXTRA_DISTANCE_NORM     35

// sensor state
enum senState {
  SEN_NORMAL   = 0,
  SEN_OBSTACLE = 1,
  SEN_WALL     = 2,
  SEN_BLOCK    = 3
};

// robot state structure
typedef struct roboInputState {
  short legHightNow;
  bool obstacleEnabled;
} roboInputState;

// robot state
roboInputState riState = {
  HIGHT_DEFAULT,           // short legHightNow;
  true                     // bool obstacleEnabled;
};

// sensors enabled
bool sensorsEnabled = false;
// edge enabled
bool edgeEnabled = false;
// extra inputs enabled
bool extraInputsEnabled = false;
// assume wall angle
short wallAngle = 0;
// sensors state
unsigned char sensorStateLeft = SEN_NORMAL;
unsigned char sensorStateRight = SEN_NORMAL;
// inputs state
unsigned char inputStateNow = IN_NORMAL;
unsigned char extraStateNow = EX_NORMAL;
// sensors values
short leftSensorValue = SENSOR_DISTANCE_NORM;
short rightSensorValue = SENSOR_DISTANCE_NORM;
// sensor buffers
short leftSensorBuffer0 = SENSOR_DISTANCE_NORM;
short rightSensorBuffer0 = SENSOR_DISTANCE_NORM;
short leftSensorBuffer1 = SENSOR_DISTANCE_NORM;
short rightSensorBuffer1 = SENSOR_DISTANCE_NORM;
short leftSensorBuffer2 = SENSOR_DISTANCE_NORM;
short rightSensorBuffer2 = SENSOR_DISTANCE_NORM;
short leftSensorAverage = SENSOR_DISTANCE_NORM;
short rightSensorAverage = SENSOR_DISTANCE_NORM;
//
short leftExtraSensorValue = 0;
short rightExtraSensorValue = 0;
short leftExtraSensorBuffer0 = 0;
short rightExtraSensorBuffer0 = 0;
short leftExtraSensorBuffer1 = 0;
short rightExtraSensorBuffer1 = 0;
short leftExtraSensorBuffer2 = 0;
short rightExtraSensorBuffer2 = 0;
short leftExtraSensorAverage = 10;
short rightExtraSensorAverage = 10;
short leftExtraSensorBallanced = 0;
short rightExtraSensorBallanced = 0;

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
    if (edgeEnabled && riState.obstacleEnabled) {
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

// init inputs
void initInputs(bool calibrationMode) {
  Serial.println(F("initInputs"));
  for (i = 0; i < 16; i ++) {
    updateInputsCount(0);
    delay(10);
  }
  unsigned char counter = 0;
  while (calibrationMode) {
    counter ++;
    if (counter >= SERVO_FULL_CYCLE) {
      counter = 0;
    }
    delay(20);
    if (m_getButtonPressed()) {
      calibrationMode = false;
    }
    updateInputsCount(counter);
    if (counter == 0) {
      _printSensorState(sensorStateLeft);
      _printSensorState(sensorStateRight);
      _printInputState();
      if (extraInputsEnabled) {
        Serial.print(F(" Average left "));
        Serial.print((int)leftExtraSensorAverage);
        Serial.print(F(" Average Right "));
        Serial.print((int)rightExtraSensorAverage);
        Serial.print(F(" Extra left "));
        Serial.print((int)getExtraInputLeft());
        Serial.print(F(" Extra Right "));
        Serial.println((int)getExtraInputRight());
      }
    }
  }
}

// update sensor readings
void updateInputsCount(unsigned char counter) {
  //
  if (extraInputsEnabled) {
    //
    leftExtraSensorValue = (950 - analogRead(A3)) / 4 - EXTRA_DISTANCE_NORM - riState.legHightNow / ROBOT_SIZE_DEVIDER;
    leftExtraSensorBallanced = (leftExtraSensorValue + leftExtraSensorBuffer0 + leftExtraSensorBuffer1 + leftExtraSensorBuffer2) / 4;
    leftExtraSensorBuffer2 = leftExtraSensorBuffer1;
    leftExtraSensorBuffer1 = leftExtraSensorBuffer0;
    leftExtraSensorBuffer0 = leftExtraSensorValue;
    //
    rightExtraSensorValue = (950 - analogRead(A2)) / 4 - EXTRA_DISTANCE_NORM - riState.legHightNow / ROBOT_SIZE_DEVIDER;
    rightExtraSensorBallanced = (rightExtraSensorValue + rightExtraSensorBuffer0 + rightExtraSensorBuffer1 + rightExtraSensorBuffer2) / 4;
    rightExtraSensorBuffer2 = rightExtraSensorBuffer1;
    rightExtraSensorBuffer1 = rightExtraSensorBuffer0;
    rightExtraSensorBuffer0 = rightExtraSensorValue;
    //
    if (counter == 1) {
      // extra left
      if (leftExtraSensorAverage < leftExtraSensorValue) {
        leftExtraSensorAverage ++;
      } else if (leftExtraSensorAverage > leftExtraSensorValue) {
        leftExtraSensorAverage --;
      }
      // extra right
      if (rightExtraSensorAverage < rightExtraSensorValue) {
        rightExtraSensorAverage ++;
      } else if (rightExtraSensorAverage > rightExtraSensorValue) {
        rightExtraSensorAverage --;
      }
    }
  }
  //
  leftSensorBuffer2 = leftSensorBuffer1;
  leftSensorBuffer1 = leftSensorBuffer0;
  leftSensorBuffer0 = leftSensorValue;
  leftSensorValue = analogRead(A0);
  //
  rightSensorBuffer2 = rightSensorBuffer1;
  rightSensorBuffer1 = rightSensorBuffer0;
  rightSensorBuffer0 = rightSensorValue;
  rightSensorValue = analogRead(A1);
  //
  if (counter == 0) {
    // process analog sensors
    if (sensorsEnabled) {
      leftSensorAverage = (leftSensorValue + leftSensorBuffer0 + leftSensorBuffer1 + leftSensorBuffer2) / 4;
      rightSensorAverage = (rightSensorValue + rightSensorBuffer0 + rightSensorBuffer1 + rightSensorBuffer2) / 4;
      if ((rightSensorAverage > SENSOR_NO_DATA) || (leftSensorAverage > SENSOR_NO_DATA)) {
        // data received
        wallAngle = - rightSensorAverage + leftSensorAverage;
        if (wallAngle > 80) {
          wallAngle = 80;
        } else if (wallAngle < -80) {
          wallAngle = -80;
        }
        //
        sensorStateRight = _getStateFromRaw(rightSensorAverage); // right
        sensorStateLeft = _getStateFromRaw(leftSensorAverage); // left
        //if (extraInputsEnabled) {
        //  if (getExtraInputLeft() > 40) {
        //    sensorStateRight = SEN_BLOCK;
        //  }
        //  if (getExtraInputRight() > 40) {
        //    sensorStateLeft = SEN_BLOCK;
        //  }
        //}
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
    // process extras
    if (extraInputsEnabled) {
      if ((getExtraInputLeft() < -20) || (getExtraInputRight() < -20)) {
        extraStateNow = EX_STEP_UP_BIG;
      } else if ((getExtraInputLeft() < -10) || (getExtraInputRight() < -10)) {
        extraStateNow = EX_STEP_UP_SMALL;
      } else if ((getExtraInputLeft() > 20) || (getExtraInputRight() > 20)) {
        extraStateNow = EX_STEP_DOWN_BIG;
      } else if ((getExtraInputLeft() > 10) || (getExtraInputRight() > 10)) {
        extraStateNow = EX_STEP_DOWN_SMALL;
      } else {
        extraStateNow = EX_NORMAL;
      }
    }
    //_printSensorState(sensorStateLeft);
    //_printSensorState(sensorStateRight);
    //_printInputState();
  }
}

// get extra input left. negative - bump in mm
short getExtraInputLeft(void) {
  return (short)(leftExtraSensorBallanced - leftExtraSensorAverage); //
}

// get extra input right. negative - bump in mm
short getExtraInputRight(void) {
  return (short)(rightExtraSensorBallanced - rightExtraSensorAverage); //
}

// get wall angle
short getWallAngleInputs(void) {
  return wallAngle;
}

// get inputs state
unsigned char getInputState(void) {
  return inputStateNow;
}

// get extra inputs state
unsigned char getExtraInputState(void) {
  return extraStateNow;
}

// set robot state
void setStateInputs(unsigned char newState) {
  switch (newState) {
    case ROBOT_NORM:
    {
      riState.legHightNow = HIGHT_DEFAULT;
      riState.obstacleEnabled = true;
    }
    break;
    case ROBOT_INO:
    {
      riState.legHightNow = HIGHT_DEFAULT;
      riState.obstacleEnabled = false;
    }
    break;
    default:
    break;
  }
}

// enable sensors
void enableSensorInputs(bool inputs) {
  sensorsEnabled = inputs;
}

// enable extra sensors
void enableExtraInputs(bool inputs) {
  extraInputsEnabled = inputs;
}

// enable edges
void enableEdgeInputs(bool inputs) {
  edgeEnabled = inputs;
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
