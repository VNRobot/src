/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// center position in the leg forward shift. bigger the number more weight on front
#define FORWARD_BALLANCE_SHIFT    -10
// ballance correction max value
#define DYNAMIC_BALLANCE_MAX       20
#define STATIC_BALLANCE_MAX        20
#define STATIC_BALLANCE_MULIPLIER  4
#define SIDE_BALLANCE_MAX         20

// robot state structure
typedef struct roboBallanceState {
  bool staticEnabled;
  bool dynamicEnabled;
  bool sideEnabled;
} roboBallanceState;

// robot state
roboBallanceState robalParam = {
  false,           // bool staticEnabled;
  false,           // bool dynamicEnabled;
  false            // bool sideEnabled;
};

// dynamic forward ballance value
char dynamicForward = 0;
// static forward ballance value
char staticForward = 0;
// accelerometer values
short rockFL = 0;
short rockFR = 0;
short rockRL = 0;
short rockRR = 0;
short rockValFL = 0;
short rockValFR = 0;
short rockValRL = 0;
short rockValRR = 0;
// side ballance hight
short sideBallance = 0;

/*
uses
m_gyroState
m_legsValue
*/

// dynamic ballance
short getBallanceCount(unsigned char counter) {
  if (robalParam.dynamicEnabled) {
    // get diving value
    if (m_legsValue.fl.state == LEG_LIFTED) {
      rockFL = m_gyroState.aLiftFL;
    } else if (m_legsValue.fr.state == LEG_LIFTED) {
      rockFR = m_gyroState.aLiftFR;
    } else if (m_legsValue.rl.state == LEG_LIFTED) {
      rockRL = m_gyroState.aLiftRL;
    } else if (m_legsValue.rr.state == LEG_LIFTED) {
      rockRR = m_gyroState.aLiftRR;
    }
    // remember value
    if (m_legsValue.fl.state == LEG_LOWERING) {
      rockValFL = rockFL;
    } else if (m_legsValue.fr.state == LEG_LOWERING) {
      rockValFR = rockFR;
    } else if (m_legsValue.rl.state == LEG_LOWERING) {
      rockValRL = rockRL;
    } else if (m_legsValue.rr.state == LEG_LOWERING) {
      rockValRR = rockRR;
    }
    if (counter == 0) {
      // too heavy - negative
      //Serial.print(" F ");
      //Serial.print((int)sumF);
      //Serial.print(" R ");
      //Serial.println((int)sumR);
      if ((rockValFL + rockValFR) < (rockValRL + rockValRR)) {
        // front too heavy
        //Serial.println(" Front heavy ");
        dynamicForward --;
      } else if ((rockValFL + rockValFR) > (rockValRL + rockValRR)) {
        // rear too heavy
        //Serial.println(" Rear heavy ");
        dynamicForward ++;
      }
      if (dynamicForward > DYNAMIC_BALLANCE_MAX) {
        dynamicForward = DYNAMIC_BALLANCE_MAX;
      }
      if (dynamicForward < -DYNAMIC_BALLANCE_MAX) {
        dynamicForward = -DYNAMIC_BALLANCE_MAX;
      }
    }
  }
  if (robalParam.staticEnabled) {
    if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
      short staticForwardTemp = (short)(m_gyroState.aPitchNow * STATIC_BALLANCE_MULIPLIER);
      if ((staticForward > staticForwardTemp) && (staticForward > -STATIC_BALLANCE_MAX)) {
        staticForward --;
      } else if ((staticForward < staticForwardTemp) && (staticForward < STATIC_BALLANCE_MAX)) {
        staticForward ++;
      }
    }
  }
  return FORWARD_BALLANCE_SHIFT + dynamicForward + staticForward;
}

// side ballance
short getSideBallanceCount(void) {
  if (robalParam.sideEnabled) {
    if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
      if (m_gyroState.aRollNow > 1) {
        if (sideBallance < SIDE_BALLANCE_MAX) {
          sideBallance ++;
        }
      } else if (m_gyroState.aRollNow < -1) {
        if (sideBallance > -SIDE_BALLANCE_MAX) {
          sideBallance --;
        }
      }
      //Serial.print(" Roll ");
      //Serial.print((int)m_gyroState.aRollNow);
      //Serial.print(" side ");
      //Serial.println((int)sideBallance);
    }
  }
  return sideBallance;
}

// enable dynamic ballance
void enableDynamicBallance(bool ballance) {
  robalParam.dynamicEnabled = ballance;
}

// enable static ballance
void enableStaticBallance(bool ballance) {
  robalParam.staticEnabled = ballance;
}

// enable side ballance
void enableSideBallance(bool ballance) {
  robalParam.sideEnabled = ballance;
}
