/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot walking path
*/

// robot size devider
#define ROBOT_SIZE_DEVIDER       2

// enable steps distance count
bool stepsDistanceCountEnable = false;
// speed relative value from 0 to 2
char speedAbsolute = 0;
// walking direction
bool walkFrward = true;
// distance to the target mm
short distanceToTarget = 0;
// absolute speed
char  speedLNow = 0;
char  speedRNow = 0;

// step rotation
char _turnStep(short direction, char speed) {
  if (walkFrward) {
    // forward
    if (direction > 4) {
      if (speed < 2) {
        speed ++;
      }
    }
    if (direction < -4) {
      if (speed > 0) {
        speed --;
      }
    }
  } else {
    // backward
    if (direction < -4) {
      if (speed < 2) {
        speed ++;
      }
    }
    if (direction > 4) {
      if (speed > 0) {
        speed --;
      }
    }
  }
  return speed;
}

// get speed
void updatePath(short direction, char speedMuliplier) {
  // walking mode
  if (distanceToTarget > 0) {
    // plan to go
    if ((direction < 30) && (direction > -30)) {
      // go forward
      if (speedAbsolute < 1) {
        speedAbsolute = 1;
      } else {
        speedAbsolute = 2;
      }
      walkFrward = true;
    } else if ((direction < 150) && (direction > -150)) {
      // stand and turn
      speedAbsolute = 0;
      walkFrward = true;
    } else {
      // go back
      if (speedAbsolute < 1) {
        speedAbsolute = 1;
      } else {
        speedAbsolute = 2;
      }
      walkFrward = false;
    }
  } else {
    // arrived to the destnation
    speedAbsolute = 0;
    walkFrward = true;
  }
  // step turning
  speedLNow = _turnStep(-direction, speedAbsolute);
  speedRNow = _turnStep(direction, speedAbsolute);
  if (stepsDistanceCountEnable) {
    // step size
    short stepSize = ((SERVO_HALF_CYCLE - (2 + speedAbsolute))* speedAbsolute * speedMuliplier) / ROBOT_SIZE_DEVIDER;
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
  return speedLNow;
}

// get right speed
char getspeedRPath(void) {
  return speedRNow;
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
short calculateNewDirectionPath(short wallAngle, unsigned char inputState) {
  short newDirection = 0;
  switch (inputState) {
    case IN_OBSTACLE_FRONT:
      {
        if (wallAngle > 0) {
          newDirection = 90 - wallAngle;
        } else {
          newDirection = - 90 - wallAngle;
        }
      }
    break;
    case IN_OBSTACLE_FRONTLEFT:
      {
        if (wallAngle > 20) {
          newDirection = 90 - wallAngle;
        } else {
          newDirection = 60;
        }
      }
    break;
    case IN_OBSTACLE_FRONTRIGHT:
      {
        if (wallAngle < -20) {
          newDirection = - 90 - wallAngle;
        } else {
          newDirection = - 60;
        }
      }
    break;
    case IN_OBSTACLE_LEFT:
      {
        if (wallAngle > 40) {
          newDirection = 90 - wallAngle;
        } else {
          newDirection = 30;
        }
      }
    break;
    case IN_OBSTACLE_RIGHT:
      {
        if (wallAngle < -40) {
          newDirection = - 90 - wallAngle;
        } else {
          newDirection = - 30;
        }
      }
    break;
    case IN_FAR_OBSTACLE_LEFT:
      {
        if (wallAngle > 60) {
          newDirection = 90 - wallAngle;
        } else {
          newDirection = 10;
        }
      }
    break;
    case IN_FAR_OBSTACLE_RIGHT:
      {
        if (wallAngle < -60) {
          newDirection = - 90 - wallAngle;
        } else {
          newDirection = - 10;
        }
      }
    break;
    case IN_NORMAL:
      // do nothing
    break;
    default:
      Serial.print(F(" Wrong input path state "));
    break;
  }
  return newDirection;
}