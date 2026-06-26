/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// center position in the leg forward shift. bigger the number more weight on front
#define FORWARD_BALLANCE_SHIFT    -10
// ballance correction max value
#define DYNAMIC_BALLANCE_MAX       10
#define STATIC_BALLANCE_MAX        10
#define STATIC_BALLANCE_MULIPLIER  4

// robot state structure
typedef struct roboBallanceState {
  bool staticEnabled;
  bool dynamicEnabled;
} roboBallanceState;

// robot state
roboBallanceState robalParam = {
  false,           // bool staticEnabled;
  false            // bool dynamicEnabled
};

// dynamic forward ballance value
char dynamicForward = 0;
// static forward ballance value
char staticForward = 0;

/*
uses
m_gyroState
m_legsValue
*/

// dynamic ballance
short getBallanceCount(void) {
  if (robalParam.dynamicEnabled) {
    // get diving value
    if (m_legsValue.fl.lifted) {
      if (m_gyroState.aLiftFL < m_gyroState.aLiftRR) {
        // fl leg too heavy
        dynamicForward --;
      }
    } else if (m_legsValue.fr.lifted) {
      if (m_gyroState.aLiftFR < m_gyroState.aLiftRL) {
        // fr leg too heavy
        dynamicForward --;
      }
    } else if (m_legsValue.rl.lifted) {
      if (m_gyroState.aLiftRL < m_gyroState.aLiftFR) {
        // rl leg too heavy
        dynamicForward ++;
      }
    } else if (m_legsValue.rr.lifted) {
      if (m_gyroState.aLiftRR < m_gyroState.aLiftFL) {
        // rr leg too heavy
        dynamicForward ++;
      }
    }
    if (dynamicForward > DYNAMIC_BALLANCE_MAX) {
      dynamicForward = DYNAMIC_BALLANCE_MAX;
    }
    if (dynamicForward < -DYNAMIC_BALLANCE_MAX) {
      dynamicForward = -DYNAMIC_BALLANCE_MAX;
    }
  }
  if (robalParam.staticEnabled) {
    short staticForwardTemp = (short)(m_gyroState.aPitchNow * STATIC_BALLANCE_MULIPLIER);
    if ((staticForward > staticForwardTemp) && (staticForward > -STATIC_BALLANCE_MAX)) {
      staticForward --;
    } else if ((staticForward < staticForwardTemp) && (staticForward < STATIC_BALLANCE_MAX)) {
      staticForward ++;
    }
  }
  return FORWARD_BALLANCE_SHIFT + dynamicForward + staticForward;
}

// enable dynamic ballance
void enableDynamicBallance(bool ballance) {
  robalParam.dynamicEnabled = ballance;
}

// enable static ballance
void enableStaticBallance(bool ballance) {
  robalParam.staticEnabled = ballance;
}
