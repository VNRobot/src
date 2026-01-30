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

// turn left or right decision
bool turnLeft = true;

// sensors state
unsigned char senStateLeft = 0;
unsigned char senStateRight = 0;

// analog sensors structure
struct aSensors {
  unsigned short left;      // A0
  unsigned short right;     // A1
};

// analog input values array
aSensors analogRawInputs = {0, 0}; // raw values
aSensors analogValueInputs = {0, 0}; // processed values

// init inputs
void initInputs(void) {
}

// read and remember analog sensors readings
void updateInputs(void) {
  // read once in a pattern
  if (m_sequenceCounter.m == 0) {
    // read analog inputs
    analogRawInputs.left = (unsigned short)analogRead(A0);
    analogRawInputs.right = (unsigned short)analogRead(A1);
    // crossconnection left senor is facing right and right sensor is facing left
    analogValueInputs.right = (unsigned short)((1600000 / analogRawInputs.left) / analogRawInputs.left);
    analogValueInputs.left = (unsigned short)((1600000 / analogRawInputs.right) / analogRawInputs.right);
    // get sensor state
    senStateLeft  = _getSensorState(analogValueInputs.left);
    senStateRight = _getSensorState(analogValueInputs.right);
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
  if (m_robotState.sensorsEnabledNow) {
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
      if (m_robotState.edgeEnabled) {
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
/*
// print raw data
void _printLineInputs(void) {
  Serial.print(F(" left "));
  Serial.print((int)analogValueInputs.left);
  Serial.print(F(" right "));
  Serial.print((int)analogValueInputs.right);
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