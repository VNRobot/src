/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
state file
*/

// counter to keep state the same
#define STATE_COUNTER                 2

// state counter
char stateCounter = STATE_COUNTER * 2;

// update state
char  stateUpdate(bool surfaceBumpy, bool surfaceFlat, char extraInputState) {
  // state value
  char stateValue = ROBOT_NORM;
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
    stateValue = ROBOT_NORM;
  } else {
    if (stateCounter > STATE_COUNTER) {
      stateValue = ROBOT_CRAWL;
    } else {
      stateValue = ROBOT_INO;
    }
    stateCounter --;
  }
  return stateValue;
}