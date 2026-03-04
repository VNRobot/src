/*
Walking Robot Lowrie
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

// robot size
#define ROBOT_SIZE_DIVIDER      2
// sensors geometry
#define SENSOR_ANGLE            20     // down angle
#define SENSOR_HIGHT            10     // mm hight relative to legs
#define SENSOR_DISTANCE_MIN     12     // cm
#define SENSOR_DISTANCE_MAX     48     // cm

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
  unsigned short leftUpper;      // A0 left upper sensor facing down
  unsigned short rightUpper;     // A1 right upper sensor facing down
  unsigned short leftLower;      // A3 left lower sensor uses when flipped
  unsigned short rightLower;     // A2 right lower sensor uses when flipped
};
// distance sensors structure
struct dSensors {
  unsigned short left;
  unsigned short right;
};

// sensors enabled flag
bool sensorsEnabled = false;
bool sensorsActive = false;
// bool edge enabled
bool edgeEnabled = false;
// turn left or right decision
bool turnLeft = true;
// sensors state
unsigned char senStateLeft = 0;
unsigned char senStateRight = 0;
// analog input values array
aSensors analogRawInputs = {0, 0, 0, 0}; // raw values
dSensors distanceMeasured = {0, 0}; // processed values
// normal min and max distance
unsigned char calculatedDistance = SENSOR_DISTANCE_MAX; // cm

// check surface
void checkSurface(void) {
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

// init inputs
void initInputs(void) {
  // calculate geometry
  if (SENSOR_ANGLE > 60) { // 60 deg max
    // sensors facing down
    calculatedDistance = SENSOR_HIGHT + m_robotState.legHightNow / ROBOT_SIZE_DIVIDER; // 2 for small robot
    edgeEnabled = false;
  } else if (SENSOR_ANGLE > 15) { // 15 deg min
    // can be calculated
    calculatedDistance = (unsigned char)(((SENSOR_HIGHT + m_robotState.legHightNow / ROBOT_SIZE_DIVIDER) / sin(SENSOR_ANGLE * (PI / 180.0))) / 10);
    edgeEnabled = true;
  } else {
    // too horizontal
    calculatedDistance = SENSOR_DISTANCE_MAX;
    edgeEnabled = false;
  }
  // analyse sensor angles
  if (calculatedDistance > SENSOR_DISTANCE_MAX) {
    // too far
    calculatedDistance = SENSOR_DISTANCE_MAX;
    edgeEnabled = false;
  } else if (calculatedDistance < SENSOR_DISTANCE_MIN) {
    // too close
    calculatedDistance = SENSOR_DISTANCE_MIN;
    edgeEnabled = false;
  } else {
    // all good
    edgeEnabled = true;
    sensorsEnabled = true;
  }
}

// read and remember analog sensors readings
void updateInputs(void) {
  // read once in a pattern
  if (m_sequenceCounter.m == 0) {
    // check roll and pitch
    checkSurface();
    // read analog inputs
    analogRawInputs.leftUpper = (unsigned short)analogRead(A0);
    analogRawInputs.rightUpper = (unsigned short)analogRead(A1);
    analogRawInputs.leftLower = (unsigned short)analogRead(A3);
    analogRawInputs.rightLower = (unsigned short)analogRead(A2);
    // crossconnection left sensor is facing right and right sensor is facing left upper facing down lower facing up
    if (m_gyroState.aRollAverage < 0) {
      // upside down
      if (analogRawInputs.leftUpper > analogRawInputs.leftLower) {
        // top blocked
        distanceMeasured.left = (unsigned short)((1600000 / analogRawInputs.leftUpper) / analogRawInputs.leftUpper);
      } else {
        // ground found
        distanceMeasured.left = (unsigned short)((1600000 / analogRawInputs.leftLower) / analogRawInputs.leftLower);
      }
      if (analogRawInputs.rightUpper > analogRawInputs.rightLower) {
        // top blocked
        distanceMeasured.right = (unsigned short)((1600000 / analogRawInputs.rightUpper) / analogRawInputs.rightUpper);
      } else {
        // ground found
        distanceMeasured.right = (unsigned short)((1600000 / analogRawInputs.rightLower) / analogRawInputs.rightLower);
      }
    } else {
      // normal
      if (analogRawInputs.leftUpper > analogRawInputs.leftLower) {
        // ground found
        distanceMeasured.right = (unsigned short)((1600000 / analogRawInputs.leftUpper) / analogRawInputs.leftUpper);
      } else {
        // top blocked
        distanceMeasured.right = (unsigned short)((1600000 / analogRawInputs.leftLower) / analogRawInputs.leftLower);
      }
      if (analogRawInputs.rightUpper > analogRawInputs.rightLower) {
        // ground found
        distanceMeasured.left = (unsigned short)((1600000 / analogRawInputs.rightUpper) / analogRawInputs.rightUpper);
      } else {
        // top blocked
        distanceMeasured.left = (unsigned short)((1600000 / analogRawInputs.rightLower) / analogRawInputs.rightLower);
      }
    }
    // find turn
    if (distanceMeasured.left < distanceMeasured.right) {
      turnLeft = false;
    } else {
      turnLeft = true;
    }
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
  if (sensorsEnabled && sensorsActive) {
    if (input < 120) {                            // no edge            120
      if (input > 5) {                            // not blocked        5
        if (input > (calculatedDistance / 3)) {   // no wall            
          if (input > (calculatedDistance / 2)) { // no obstacle        
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