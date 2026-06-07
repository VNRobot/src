/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot walking path
*/

// direction state
enum diState {
  DI_FORWARD = 5,
  DI_FORWARD_FAR_TURN = 10,
  DI_FORWARD_TURN = 20,
  DI_FORWARD_STAND = 30,
  DI_BACKWARD_STAND = 40,
  DI_BACKWARD_TURN = 50
};

// robot state structure
typedef struct roboPathState {
  char speedMuliplierNow;
} roboPathState;

// robot state
roboPathState rpState = {
  2                       // char speedMuliplierNow; 1 or 2
};

// enable step turning
bool stepTurningEnabled = false;
// enable steps distance count
bool stepsDistanceCountEnabled = false;
// speed relative value from 0 to 2
char speedAbsolute = 0;
// walking direction
bool walkFrward = true;
// distance to the target mm
short distanceToTarget = 0;
// absolute speed
char  speedLNow = 0;
char  speedRNow = 0;
// on the path flag
bool onThePath = false;
// state 
unsigned char pathState = ROBOT_NORM;

// step rotation
char _turnStep(short direction, char speed) {
  if (walkFrward) {
    // forward
    if (direction >= DI_FORWARD) {
      if (speed < 4) {
        speed ++;
      }
      if (speed < 4) {
        speed ++;
      }
    }
    if (direction <= - DI_FORWARD) {
      if (speed > 0) {
        speed --;
      }
      if (speed > 0) {
        speed --;
      }
    }
  } else {
    // backward
    if (direction <= - DI_FORWARD) {
      if (speed < 4) {
        speed ++;
      }
      if (speed < 4) {
        speed ++;
      }
    }
    if (direction >= DI_FORWARD) {
      if (speed > 0) {
        speed --;
      }
      if (speed > 0) {
        speed --;
      }
    }
  }
  return speed;
}

// get speed
void updatePath(short direction) {
  // walking mode
  if (distanceToTarget > 0) {
    // plan to go
    if ((direction < DI_FORWARD_TURN) && (direction > - DI_FORWARD_TURN)) {
      // go forward
      if (speedAbsolute < 4) {
        speedAbsolute ++;
      }
      walkFrward = true;
    } else if ((direction < DI_FORWARD_STAND) && (direction > - DI_FORWARD_STAND)) {
      // stand and turn
      if (speedAbsolute > 0) {
        speedAbsolute --;
      }
      walkFrward = true;
    } else if ((direction < DI_BACKWARD_STAND) && (direction > - DI_BACKWARD_STAND)) {
      // stand and turn
      speedAbsolute = 0;
      walkFrward = false;
    } else {
      // go back
      if (speedAbsolute < 4) {
        speedAbsolute ++;
      }
      walkFrward = false;
    }
  } else {
    // arrived to the destnation
    if (speedAbsolute > 0) {
      speedAbsolute --;
    }
    walkFrward = true;
  }
  // step turning
  speedLNow = _turnStep(-direction, speedAbsolute);
  speedRNow = _turnStep(direction, speedAbsolute);
  if (!stepTurningEnabled) {
    if (speedLNow < speedRNow) {
      speedLNow = speedRNow;
    } else if (speedRNow < speedLNow) {
      speedRNow = speedLNow;
    }
  }
  if (stepsDistanceCountEnabled) {
    // step size
    short stepSize = ((SERVO_HALF_CYCLE - (speedAbsolute))* speedAbsolute * rpState.speedMuliplierNow) / ROBOT_SIZE_DEVIDER;
    // update distance to target
    if (distanceToTarget > stepSize) {
      distanceToTarget -= stepSize;
    } else {
      distanceToTarget = 0;
    }
  }
}

// get left speed 0 t0 8
char getspeedLPath(void) {
  if (walkFrward) {
    return speedLNow * 2;
  }
  return -speedLNow * 2;
}

// get right speed 0 to 8
char getspeedRPath(void) {
  if (walkFrward) {
    return speedRNow * 2;
  }
  return -speedRNow * 2;
}

// get direction flag
bool getforwardPath(void) {
  return walkFrward;
}

// set distance to target in cm
void setDistancePath(short distance) {
  distanceToTarget = distance * 10;
}

// update distance to target in cm
void updateDistancePath(short distance) {
  distanceToTarget += distance * 10;
}

// get distance to target in cm
short getDistancePath(void) {
  return distanceToTarget / 10;
}

// calculate new direction
short calculateNewDirectionPath(unsigned char inputState, short wallAngle, short direction) {
  if ((direction < DI_FORWARD_FAR_TURN) && (direction > - DI_FORWARD_FAR_TURN)) {
    // direction is close to the target
    onThePath = true;
  }
  if (inputState == IN_NORMAL) {
    // no obstacle
    if (!onThePath) {
      // stop turning
      if (direction > DI_FORWARD) {
        direction = DI_FORWARD;
      } else if (direction < - DI_FORWARD) {
        direction = - DI_FORWARD;
      }
      return direction;
    }
    return 0;
  }
  // surface is flat
  // obstacle present
  if (onThePath) {
    onThePath = false;
    switch (inputState) {
      case IN_OBSTACLE_FRONT:
        {
          if (wallAngle > 0) {
            direction = DI_BACKWARD_TURN;
          } else {
            direction = - DI_BACKWARD_TURN;
          }
        }
      break;
      case IN_OBSTACLE_FRONTLEFT:
        {
          direction = DI_BACKWARD_STAND;
        }
      break;
      case IN_OBSTACLE_FRONTRIGHT:
        {
          direction = - DI_BACKWARD_STAND;
        }
      break;
      case IN_OBSTACLE_LEFT:
        {
          direction = DI_FORWARD_STAND;
        }
      break;
      case IN_OBSTACLE_RIGHT:
        {
          direction = - DI_FORWARD_STAND;
        }
      break;
      case IN_FAR_OBSTACLE_FRONT:
        {
          if (wallAngle > 0) {
            direction = DI_FORWARD_TURN;
          } else {
            direction = - DI_FORWARD_TURN;
          }
        }
      break;
      case IN_FAR_OBSTACLE_LEFT:
        {
          direction = DI_FORWARD_FAR_TURN;
        }
      break;
      case IN_FAR_OBSTACLE_RIGHT:
        {
          direction = - DI_FORWARD_FAR_TURN;
        }
      break;
      default:
        Serial.print(F(" Wrong input path state "));
      break;
    }
  } else {
    // keep turning
    switch (inputState) {
      case IN_OBSTACLE_FRONT:
        {
          if (direction > 0) {
            direction = DI_BACKWARD_TURN;
          } else {
            direction = - DI_BACKWARD_TURN;
          }
        }
      break;
      case IN_OBSTACLE_FRONTLEFT:
      case IN_OBSTACLE_FRONTRIGHT:
        {
          if (direction > 0) {
            direction = DI_BACKWARD_STAND;
          } else {
            direction = - DI_BACKWARD_STAND;
          }
        }
      break;
      case IN_OBSTACLE_LEFT:
      case IN_OBSTACLE_RIGHT:
        {
          if (direction > 0) {
            direction = DI_FORWARD_STAND;
          } else {
            direction = - DI_FORWARD_STAND;
          }
        }
      break;
      case IN_FAR_OBSTACLE_FRONT:
        {
          if (direction > 0) {
            direction = DI_FORWARD_TURN;
          } else {
            direction = - DI_FORWARD_TURN;
          }
        }
      break;
      case IN_FAR_OBSTACLE_LEFT:
        {
          direction = DI_FORWARD_FAR_TURN;
        }
      break;
      case IN_FAR_OBSTACLE_RIGHT:
        {
          direction = - DI_FORWARD_FAR_TURN;
        }
      break;
      default:
        Serial.print(F(" Wrong input path state "));
      break;
    }
  }
  return direction;
}

// set robot state
void setStatePath(unsigned char newState) {
  switch (newState) {
    case ROBOT_NORM:
    {
      rpState.speedMuliplierNow = 2;
      pathState = ROBOT_NORM;
    }
    break;
    case ROBOT_INO:
    {
      rpState.speedMuliplierNow = 2;
      if (pathState == ROBOT_NORM) {
        speedAbsolute = 2;
        speedLNow = 0;
        speedRNow = 0;
      }
      pathState = ROBOT_INO;
    }
    break;
    case ROBOT_CRAWL:
    {
      rpState.speedMuliplierNow = 2;
      if (pathState == ROBOT_NORM) {
        speedAbsolute = 2;
        speedLNow = 0;
        speedRNow = 0;
      }
      pathState = ROBOT_CRAWL;
    }
    break;
    default:
    break;
  }
}

// enable step turning
void enableTurningPath(bool turning) {
  stepTurningEnabled = turning;
}

// enable distance counting
void enableCountingPath(bool turning) {
  stepsDistanceCountEnabled = turning;
}
