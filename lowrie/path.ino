/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot walking path
*/

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
  bool stepTurningEnabled;
  bool stepsDistanceCountEnabled;
} pathParameters;

// robot state
pathParameters pathParams = {
  100,                    // short maximalStep;
  false,                  // bool stepTurningEnabled;
  false                   // bool stepsDistanceCountEnabled
};

// speed relative value from 0 to 2
char speedNow = 0;
// distance to the target mm
short distanceToTarget = 0;
// on the path flag
bool onThePath = false;
// speed multiplier
const char speedMax = 2;

// calculate absolute speed
void _setAbsoluteSpeed(short direction) {
  // walking mode
  if (distanceToTarget > 0) {
    // plan to go
    if ((direction < DI_FORWARD_FAR_TURN) && (direction > - DI_FORWARD_FAR_TURN)) {
      // go forward
      if (speedNow < speedMax) {
        speedNow ++;
      }
    } else if ((direction < DI_FORWARD_TURN) && (direction > - DI_FORWARD_TURN)) {
      // go forward slow
      if (speedNow < speedMax - 1) {
        speedNow ++;
      }
      if (speedNow > speedMax - 1) {
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
      if (speedNow > -speedMax + 1) {
        speedNow --;
      }
      if (speedNow < -speedMax + 1) {
        speedNow ++;
      }
    } else {
      // go back
      if (speedNow > -speedMax) {
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
    if (speed < speedMax) {
      speed ++;
    }
  }
  if (direction <= - DI_FORWARD_OK) {
    if (speed > -speedMax) {
      speed --;
    }
  }
  return speed;
}

// get speed
void updatePath(short direction) {
  // calculate speed
  _setAbsoluteSpeed(direction);
  if (pathParams.stepTurningEnabled) {
    // step turning
    m_legsValue.fl.speed = _sideSpeed(-direction, speedNow);
    m_legsValue.fr.speed = _sideSpeed(direction, speedNow);
  } else {
    m_legsValue.fl.speed = speedNow;
    m_legsValue.fr.speed = speedNow;
  }
  // set rear legs speed and lift point
  m_legsValue.rl.speed = m_legsValue.fl.speed;
  m_legsValue.rr.speed = m_legsValue.fr.speed;
  // calculated distance to target
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

// calculate new direction
short calculateNewDirectionPath(char inputState, short wallAngle, short direction) {
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

// set distance to target in cm
void setDistancePath(short distance) {
  // convert to mm
  distanceToTarget = distance * 10;
}

// init path
void initPath(short stepSize, bool turning, bool counting) {
  pathParams.maximalStep = stepSize;
  pathParams.stepTurningEnabled = turning;
  pathParams.stepsDistanceCountEnabled = counting;
}

// set side speed
void setSideSpeed(char speedLeft, char speedRight) {
  m_legsValue.fl.speed = speedLeft;
  m_legsValue.fr.speed = speedRight;
  m_legsValue.rl.speed = speedLeft;
  m_legsValue.rr.speed = speedRight;
}
