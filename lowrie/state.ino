/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
state file
*/

// counter to keep state the same
#define STATE_COUNTER                 2

// state counter
char stateCounter = 0;

// set robot state
void _setState(unsigned char newState) {
  switch (newState) {
    case ROBOT_NORM:
    {
      // drive
    }
    break;
    case ROBOT_INO:
    {
      // walk
    }
    break;
    case ROBOT_CRAWL:
    {
      // crawl
    }
    break;
    default:
    break;
  }
}

// update state
void stateUpdate(bool surfaceBumpy, bool surfaceFlat, char extraInputState) {
  // check for robot state
  if (!surfaceFlat || (extraInputState == EX_STEP_UP_BIG) || (extraInputState == EX_STEP_DOWN_BIG)) {
    stateCounter = STATE_COUNTER * 2;
  } else if (surfaceBumpy || (extraInputState == EX_STEP_UP_SMALL) || (extraInputState == EX_STEP_DOWN_SMALL)) {
    if (stateCounter < STATE_COUNTER) {
      stateCounter = STATE_COUNTER;
    }
  }
  // set state
  if (stateCounter == 0) {
    _setState(ROBOT_NORM);
  } else {
    if (stateCounter > STATE_COUNTER) {
      _setState(ROBOT_CRAWL);
    } else {
      _setState(ROBOT_INO);
    }
    stateCounter --;
  }
}