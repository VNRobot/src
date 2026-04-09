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

#define SENSOR_DISTANCE_BLOCK   640    // 5  cm
#define SENSOR_DISTANCE_MIN     260    // 20 cm
#define SENSOR_DISTANCE_NORM    170    // 40 cm
#define SENSOR_DISTANCE_MAX     120    // 60 cm

// sensor state
enum senState {
  SEN_NORMAL   = 0,
  SEN_OBSTACLE = 1,
  SEN_WALL     = 2,
  SEN_BLOCK    = 3
};

// sensors enabled flag disable for testing
bool sensorsEnabled = true;
// bool edge enabled flag
bool edgeEnabled = false;

/*
uses
m_robotState
m_getButtonPressed()
*/

// init inputs
void initInputs(bool calibrationMode) {
  while (calibrationMode) {
    delay(1000);
    if (m_getButtonPressed()) {
      calibrationMode = false;
    }
    updateInputsZero(true);
    _printInputState();
  }
}

// process data distance
unsigned char _getStateFromRaw(unsigned short inputValue) {
  if (inputValue > SENSOR_DISTANCE_BLOCK) {
    return SEN_BLOCK;
  }
  if (inputValue > SENSOR_DISTANCE_MIN) {
    return SEN_WALL;
  }
  if (inputValue > SENSOR_DISTANCE_NORM) {
    return SEN_OBSTACLE;
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
    case 5: // SEN_OBSTACLE   SEN_OBSTACLE
      return IN_OBSTACLE_FRONT;
    break;
    case 3: // SEN_NORMAL     SEN_BLOCK
    case 8: // SEN_WALL       SEN_NORMAL
      return IN_OBSTACLE_LEFT;
    break;
    case 4: // SEN_OBSTACLE   SEN_NORMAL
      return IN_FAR_OBSTACLE_LEFT;
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

// process analog sensors readings
void updateInputsZero(bool surfaceFlat) {
  // read analog sensors
  // crossconnection left senor is facing right and right sensor is facing left
  unsigned char sensorStateLeft = SEN_NORMAL;
  unsigned char sensorStateRight = SEN_NORMAL;
  // check edge Enabled
  // todo ***
  // check roll and pitch and check sensors active
  if (surfaceFlat && sensorsEnabled) {
    // surface is ok
    sensorStateLeft = _getStateFromRaw((unsigned short)analogRead(A1)); // right
    sensorStateRight = _getStateFromRaw((unsigned short)analogRead(A0)); // left
  }
  //
  m_robotState.inputStateNow = _getInputState(sensorStateLeft, sensorStateRight);
  //
}

// print input state
void _printInputState(void) {
  switch (m_robotState.inputStateNow) {
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
  Serial.println((int)analogRead(A1));
}
