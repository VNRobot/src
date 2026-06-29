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

// path parameters structure
typedef struct pathParameters {
  short maximalStep;
  bool stepTurningEnabled;
  bool stepsDistanceCountEnabled;
} pathParameters;

// robot state
pathParameters pathParams = {
  120,                    // short maximalStep;
  false,                  // bool stepTurningEnabled;
  false                   // bool stepsDistanceCountEnabled
};

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
// speed multiplier
char speedMultiplier = 0;

// calculate absolute speed
void _setAbsoluteSpeed(short direction) {
  if (speedAbsolute < -speedMultiplier) {
    speedAbsolute ++;
  }
  if (speedAbsolute > speedMultiplier) {
    speedAbsolute --;
  }
  // walking mode
  if (distanceToTarget > 0) {
    // plan to go
    if ((direction < DI_FORWARD_TURN) && (direction > - DI_FORWARD_TURN)) {
      // go forward
      if (speedAbsolute < speedMultiplier) {
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
      if (speedAbsolute < speedMultiplier) {
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
}

// calculate side speed
char _sideSpeed(short direction, char speed) {
  if (walkFrward) {
    // forward
    if (direction >= DI_FORWARD) {
      if (speed < speedMultiplier) {
        speed ++;
      }
      if (speed < speedMultiplier) {
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
      if (speed < speedMultiplier) {
        speed ++;
      }
      if (speed < speedMultiplier) {
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
  // calculate speed
  _setAbsoluteSpeed(direction);
  // step turning
  speedLNow = _sideSpeed(-direction, speedAbsolute);
  speedRNow = _sideSpeed(direction, speedAbsolute);
  if (!pathParams.stepTurningEnabled) {
    if (speedLNow < speedRNow) {
      speedLNow = speedRNow;
    } else if (speedRNow < speedLNow) {
      speedRNow = speedLNow;
    }
  }
  if (pathParams.stepsDistanceCountEnabled) {
    // step size
    short stepSize = pathParams.maximalStep / ROBOT_SIZE_DEVIDER;
    // update distance to target
    if (distanceToTarget > stepSize) {
      distanceToTarget -= stepSize;
    } else {
      distanceToTarget = 0;
    }
  }
}

// get left speed
char getspeedLPath(void) {
  if (walkFrward) {
    return speedLNow;
  }
  return -speedLNow;
}

// get right speed
char getspeedRPath(void) {
  if (walkFrward) {
    return speedRNow;
  }
  return -speedRNow;
}
/*
// get direction flag
bool getforwardPath(void) {
  return walkFrward;
}

// update distance to target in cm
void updateDistancePath(short distance) {
  distanceToTarget += distance * 10;
}

// get distance to target in cm
short getDistancePath(void) {
  return distanceToTarget / 10;
}
*/
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

// set distance to target in cm
void setDistancePath(short distance) {
  // convert to mm
  distanceToTarget = distance * 10;
}

// set maximal speed in mm per step
void setMaxPathStep(short stepSize, short fullCycle, short liftPoint) {
  pathParams.maximalStep = stepSize;
  speedMultiplier = stepSize / (fullCycle - (liftPoint - 1) * 2); // 120 / (64 - (4 - 1) * 2) = 2
}

// enable step turning
void enableTurningPath(bool turning) {
  pathParams.stepTurningEnabled = turning;
}

// enable distance counting
void enableCountingPath(bool counting) {
  pathParams.stepsDistanceCountEnabled = counting;
}
