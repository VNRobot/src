/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Gets analog inputs
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
  SEN_EDGE,
  SEN_OBSTACLE,
  SEN_WALL,
  SEN_BLOCK,
  SEN_NORMAL
};

// analog sensors structure
struct aSensors {
  unsigned short left;      // A0
  unsigned short right;     // A1
};

// sensors enabled flag
bool sensorsEnabled = true;
// bool edge enabled
bool edgeEnabled = false;
// turn left or right decision
bool turnLeft = true;
// sensors state
unsigned char senStateLeft = 0;
unsigned char senStateRight = 0;
// analog input values array
aSensors analogRawInputs = {0, 0}; // raw values
aSensors distanceMeasured = {0, 0}; // processed values
// normal min and max distance
unsigned char normalDistanceMin = 12; // cm
unsigned char normalDistanceMax = 48; // cm
unsigned char calculatedDistance = 48; // cm
// down angle sensor limits
unsigned char downAngleMin = OFFROAD_ANGLE; // deg
unsigned char downAngleMax = 60; // deg

// calculate sensors geometry
void calculateGeometry(void) {
  if ((SENSOR_ANGLE - m_gyroState.accPitchY) > downAngleMax) { // 60 deg max
    // too close to the ground
    calculatedDistance = SENSOR_HIGHT + m_robotState.legHightNow / 2; // 2 for small robot in mm
    edgeEnabled = false;
  } else if ((SENSOR_ANGLE - m_gyroState.accPitchY) > downAngleMin) { // 15 deg min
    // can be calculated
    calculatedDistance = (unsigned char)(((SENSOR_HIGHT + m_robotState.legHightNow / 2) / sin((SENSOR_ANGLE - m_gyroState.accPitchY) * (PI / 180.0))) / 10);
    edgeEnabled = false; //true;   //  *** todo
  } else {
    // too high
    calculatedDistance = normalDistanceMax;
    edgeEnabled = false;
  }
  if (calculatedDistance > normalDistanceMax) {
    calculatedDistance = normalDistanceMax;
    sensorsEnabled = true;
    edgeEnabled = false;
  } else if (calculatedDistance < normalDistanceMin) {
    calculatedDistance = normalDistanceMin;
    sensorsEnabled = false;
    edgeEnabled = false;
  } else {
    sensorsEnabled = true;
  }
}

// init inputs
void initInputs(void) {
  calculateGeometry();
}

// read and remember analog sensors readings
void updateInputs(void) {
  // read once in a pattern
  if (m_sequenceCounter.m == 0) {
    // update geometry
    calculateGeometry();
    // read analog inputs
    analogRawInputs.left = (unsigned short)analogRead(A0);
    analogRawInputs.right = (unsigned short)analogRead(A1);
    // find turn
    if (analogRawInputs.left > analogRawInputs.right) {
      turnLeft = false;
    } else {
      turnLeft = true;
    }
    // crossconnection left senor is facing right and right sensor is facing left
    distanceMeasured.right = (unsigned short)((1600000 / analogRawInputs.left) / analogRawInputs.left);
    distanceMeasured.left = (unsigned short)((1600000 / analogRawInputs.right) / analogRawInputs.right);
    // get sensor state
    senStateLeft  = _getSensorState(distanceMeasured.left);
    senStateRight = _getSensorState(distanceMeasured.right);
    //
    m_robotState.inputStateNow = _getInputState(senStateLeft, senStateRight);
    //
    //_printLineInputs();
    //_printSensorState(senStateLeft);
    //_printSensorState(senStateRight);
  }
}

// process distances
unsigned char _getSensorState(unsigned short input) {
  if (sensorsEnabled) {
    if (input < 120) {                            // no edge            120
      if (input > 5) {                            // not blocked        5
        if (input > (calculatedDistance / 3)) {   // no wall            
          if (input > (calculatedDistance / 2 + calculatedDistance / 4)) { // no obstacle        
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

// status of inputs
unsigned char _getInputState( unsigned short sLeft,  unsigned short sRight) {
  // both sensors normal
  if ((sLeft == SEN_NORMAL) && (sRight == SEN_NORMAL)) {
    return IN_NORMAL;
  }
  // both sensors blocked
  if ((sLeft == SEN_BLOCK) && (sRight == SEN_BLOCK)) {
    if (turnLeft) { // opposite turn
      return IN_WALL_FRONTLEFT;
    } else {
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
    if (turnLeft) {
      return IN_OBSTACLE_FRONTRIGHT;
    } else {
      return IN_OBSTACLE_FRONTLEFT;
    } 
  }
  //SEN_EDGE, SEN_OBSTACLE, SEN_WALL, SEN_BLOCK, SEN_NORMAL
  // right is normal
  if (sRight == SEN_NORMAL) {
    // only left side obstacle
    if (sLeft == SEN_BLOCK) {
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
    if (sRight == SEN_BLOCK) {
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
    return IN_WALL_FRONTRIGHT;
  }
  if (sRight == SEN_BLOCK) {
    return IN_WALL_FRONTLEFT;
  }
  //SEN_EDGE, SEN_OBSTACLE, SEN_WALL
  if ((sLeft == SEN_WALL) || (sLeft == SEN_EDGE)) {
    return IN_WALL_FRONTLEFT;
  }
  if ((sRight == SEN_WALL) || (sRight == SEN_EDGE)) {
    return IN_WALL_FRONTRIGHT;
  }
  return IN_NORMAL;
}
/*
// print raw data
void _printLineInputs(void) {
  Serial.print(F(" left "));
  Serial.print((int)distanceMeasured.left);
  Serial.print(F(" right "));
  Serial.print((int)distanceMeasured.right);
}

void _printSensorState(unsigned char sensorState) {
  switch (sensorState) {
    case SEN_EDGE:
      Serial.print(F(" SEN_EDGE "));
    break;
    case SEN_OBSTACLE:
      Serial.print(F(" SEN_OBSTACLE "));
    break;
    case SEN_WALL:
      Serial.print(F(" SEN_WALL "));
    break;
    case SEN_BLOCK:
      Serial.print(F(" SEN_BLOCK "));
    break;
    case SEN_NORMAL:
      Serial.print(F(" SEN_NORMAL "));
    break;
    default:
      Serial.print(F(" Wrong sensor state "));
  }
}
*/