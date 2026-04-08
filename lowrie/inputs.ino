/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Process analog inputs
read analog sensors. norm: 36 - 42 
*/

// robot size
#define ROBOT_SIZE_DIVIDER      2
// sensors geometry
#define SENSOR_ANGLE            30     // down angle
#define SENSOR_HIGHT            20     // mm hight relative to legs
#define SENSOR_DISTANCE_BLOCK   5      // cm
#define SENSOR_DISTANCE_MIN     20     // cm
#define SENSOR_DISTANCE_NORM    38     // cm
#define SENSOR_DISTANCE_MAX     55     // cm

// sensor state
enum senState {
  SEN_BLOCK    = 0,
  SEN_WALL     = 1,
  SEN_OBSTACLE = 2,
  SEN_EDGE     = 3,
  SEN_NORMAL   = 4
};

// sensors enabled flag disable for testing
bool sensorsEnabled = true;
// bool edge enabled flag
bool edgeEnabled = false;
// sensors active
bool sensorsActive = false;
// distance ratio * 10
//unsigned char distanceRatio = 2;
// state array
unsigned short inputStateF[6] = {0, 0, 0, 0, 0, 0};
unsigned short inputStateR[6] = {0, 0, 0, 0, 0, 0};
// sensor state left, center, right
unsigned short stateFront[3] = {0, 0, 0}; // processed values
unsigned short stateRear[3] = {0, 0, 0};  // processed values

// check surface
void _checkSurface(void) {
  // check robot roll
  if ((m_gyroState.aRollAverage < -SLOP_ANGLE) || (m_gyroState.aRollAverage > SLOP_ANGLE)) {
    sensorsActive = false;
    return;
  }
  // check robot pitch
  if ((m_gyroState.aPitchAverage < -SLOP_ANGLE) || (m_gyroState.aPitchAverage > SLOP_ANGLE)) {
    sensorsActive = false;
    return;
  }
  sensorsActive = true;
}

// get hight in mm
unsigned char _getSensorHight(void) {
  return (unsigned char)(SENSOR_HIGHT + m_robotState.legHightNow / ROBOT_SIZE_DIVIDER);
}

// init inputs
void initInputs(void) {
  // 3800 / (20 + 130 / 2) = 44
  //distanceRatio = (unsigned char)((SENSOR_DISTANCE_NORM * 100) / (SENSOR_HIGHT + HIGHT_DEFAULT / ROBOT_SIZE_DIVIDER));
}

// process data pair
unsigned char _getstateFromPair(unsigned char upper1, unsigned char lower1) {
  if (sensorsEnabled && sensorsActive) {
    if ((upper1 < SENSOR_DISTANCE_BLOCK) || (lower1 < SENSOR_DISTANCE_BLOCK)) {
      return SEN_BLOCK;
    }
    if ((upper1 < SENSOR_DISTANCE_MIN) && (lower1 < SENSOR_DISTANCE_MIN)) {
      return SEN_WALL;
    }
    if ((upper1 < SENSOR_DISTANCE_MIN) || (lower1 < SENSOR_DISTANCE_MIN)) {
      return SEN_OBSTACLE;
    }
    if ((upper1 > SENSOR_DISTANCE_MAX) && (lower1 > SENSOR_DISTANCE_MAX)) {
      if (edgeEnabled) {
        return SEN_EDGE;
      } else {
        return SEN_NORMAL;
      }
    }
  }
  return SEN_NORMAL;
}

// get minimal
unsigned char _getMin(unsigned char value1, unsigned char value2) {
  if (value1 < value2) {
    return value1;
  } else {
    return value2;
  }
}

// process analog sensors readings
void updateInputsZero(void) {
  // check roll and pitch and make sensors active
  _checkSurface();
  // pair status
  for (i = 0; i < 6; i ++) {
    inputStateF[i] = _getstateFromPair(m_inputDistanceFU[i], m_inputDistanceFL[i]);
    inputStateR[i] = _getstateFromPair(m_inputDistanceRU[i], m_inputDistanceRL[i]);
  }
  // reduce number of points
  if (m_gyroState.aUpsideAverage < 0) {
    // upside down
    for (i = 0; i < 3; i ++) {
      stateFront[2 - i] = _getMin(inputStateF[i * 2], inputStateF[i * 2 + 1]);
      stateRear[2 - i] = _getMin(inputStateR[i * 2], inputStateR[i * 2 + 1]);
    }
  } else {
    // normal
    for (i = 0; i < 3; i ++) {
      stateFront[i] = _getMin(inputStateF[i * 2], inputStateF[i * 2 + 1]);
      stateRear[i] = _getMin(inputStateR[i * 2], inputStateR[i * 2 + 1]);
    }
  }
  //
  m_robotState.inputStateNow = _getInputState();
  //
  _printInputState();
}

// status of inputs
unsigned char _getInputState(void) {
  // bits fl fc fr rl rc rr
  //      4  2  1  4  2  1
  char bits = 0;
  if (m_robotState.forwardNow) {
    if (stateFront[0] == SEN_NORMAL) {
      bits = 4;
    }
    if (stateFront[1] == SEN_NORMAL) {
      bits += 2;
    }
    if (stateFront[2] == SEN_NORMAL) {
      bits += 1;
    }
  } else {
    if (stateRear[0] == SEN_NORMAL) {
      bits = 4;
    }
    if (stateRear[1] == SEN_NORMAL) {
      bits += 2;
    }
    if (stateRear[2] == SEN_NORMAL) {
      bits += 1;
    }
  }
  switch (bits) {
    case 7:
    // go forward
    return IN_NORMAL;             
    break;
    case 6:
    // go left
    return IN_OBSTACLE_RIGHT;
    break;
    case 4:
    // turn left
    return IN_OBSTACLE_FRONTRIGHT;
    break;
    case 3:
    // go right
    return IN_OBSTACLE_LEFT;
    break;
    case 1:
    // turn right
    return IN_OBSTACLE_FRONTLEFT;
    break;
    case 0:
    case 2:
    case 5:
    // go back
    return IN_OBSTACLE_FRONT;
    break;
    default:
    break;
  }
  return IN_NORMAL;
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
    case IN_NORMAL:
      Serial.print(F(" IN_NORMAL "));
    break;
    default:
      Serial.print(F(" Wrong input state "));
  }
}
