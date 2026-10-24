/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot walking path
*/

// maximal speed
#define PATH_MAX_SPEED                2
// step size mm
#define PATH_STEP_SIZE                90

// direction state
enum diState {
  DI_FORWARD_OK = 10,
  DI_FORWARD_FAR_TURN = 20,
  DI_FORWARD_TURN = 40,
  DI_FORWARD_STAND = 90,
  DI_BACKWARD_STAND = 100,
  DI_BACKWARD_TURN = 120
};

// path parameters structure
typedef struct pathParameters {
  short maximalStep;
  short sizeDevider;
  bool stepTurningEnabled;
  bool stepsDistanceCountEnabled;
} pathParameters;

// robot state
pathParameters pathParams = {
  PATH_STEP_SIZE,         // short maximalStep;
  2,                      // short sizeDevider;
  false,                  // bool stepTurningEnabled;
  false                   // bool stepsDistanceCountEnabled
};

// speed relative value from 0 to 2
char speedNow = 0;
// distance to the target mm
short distanceToTarget = 100;
// on the path flag
bool onThePath = false;
// path attached flag
bool pathAttached = false;

// calculate absolute speed
void _setAbsoluteSpeed(short direction) {
  // walking mode
  if (distanceToTarget > 0) {
    // plan to go
    if ((direction < DI_FORWARD_FAR_TURN) && (direction > - DI_FORWARD_FAR_TURN)) {
      // go forward
      if (speedNow < PATH_MAX_SPEED) {
        speedNow ++;
      }
    } else if ((direction < DI_FORWARD_TURN) && (direction > - DI_FORWARD_TURN)) {
      // go forward slow
      if (speedNow < PATH_MAX_SPEED - 1) {
        speedNow ++;
      }
      if (speedNow > PATH_MAX_SPEED - 1) {
        speedNow --;
      }
    } else if ((direction < DI_FORWARD_STAND) && (direction > - DI_FORWARD_STAND)) {
      // stand and turn
      if (speedNow > 0) {
        speedNow --;
      }
      if (speedNow < 0) {
        speedNow ++;
      }
    } else if ((direction < DI_BACKWARD_STAND) && (direction > - DI_BACKWARD_STAND)) {
      // go back slow
      if (speedNow > -PATH_MAX_SPEED + 1) {
        speedNow --;
      }
      if (speedNow < -PATH_MAX_SPEED + 1) {
        speedNow ++;
      }
    } else {
      // go back
      if (speedNow > -PATH_MAX_SPEED) {
        speedNow --;
      }
    }
  } else {
    // arrived to the destnation
    if (speedNow > 0) {
      speedNow --;
    }
    if (speedNow < 0) {
      speedNow ++;
    }
  }
}

// calculate side speed
char _sideSpeed(short direction, char speed) {
  // forward
  if (direction >= DI_FORWARD_OK) {
    if (speed < PATH_MAX_SPEED) {
      speed ++;
    }
  }
  if (direction <= - DI_FORWARD_OK) {
    if (speed > -PATH_MAX_SPEED) {
      speed --;
    }
  }
  return speed;
}

// attach path
void attachPath(short sizeDevider, bool turning, bool counting) {
  pathAttached = true;
  pathParams.sizeDevider = sizeDevider;
  pathParams.stepTurningEnabled = turning;
  pathParams.stepsDistanceCountEnabled = counting;
}

// get speed
void updatePath(short direction) {
  if (pathAttached) {
    // calculate speed
    _setAbsoluteSpeed(direction);
    if (pathParams.stepTurningEnabled) {
      // step turning
      m_mainData.speedLeft = _sideSpeed(-direction, speedNow);
      m_mainData.speedRight = _sideSpeed(direction, speedNow);
    } else {
      m_mainData.speedLeft = speedNow;
      m_mainData.speedRight = speedNow;
    }
    // calculated distance to target
    if (pathParams.stepsDistanceCountEnabled) {
      // step size
      short stepSize = pathParams.maximalStep / pathParams.sizeDevider;
      // update distance to target
      if (distanceToTarget > stepSize) {
        distanceToTarget -= stepSize;
      } else {
        distanceToTarget = 0;
      }
    }
  }
}

// calculate new direction
short calculateNewDirectionPath(char inputState, short wallAngle, short direction) {
  if (pathAttached) {
    if ((direction < DI_FORWARD_FAR_TURN) && (direction > - DI_FORWARD_FAR_TURN)) {
      // direction is close to the target
      onThePath = true;
    }
    if (inputState == IN_NORMAL) {
      // no obstacle
      if (!onThePath) {
        // stop turning
        if (direction > DI_FORWARD_OK) {
          direction = DI_FORWARD_OK;
        } else if (direction < - DI_FORWARD_OK) {
          direction = - DI_FORWARD_OK;
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
        break;
      }
    }
    return direction;
  }
  return 0;
}

// set distance to target in cm
void setDistancePath(short distance) {
  // convert to mm
  distanceToTarget = distance * 10;
}

// init path
void initPath(void) {
}

// set side speed
void setSideSpeed(char speedLeft, char speedRight) {
  m_mainData.speedLeft = speedLeft;
  m_mainData.speedRight = speedRight;
}
