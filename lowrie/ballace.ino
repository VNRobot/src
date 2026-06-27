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
// sum of accelerometer values
short sumF = 0;
short sumR = 0;


/*
uses
m_gyroState
m_legsValue
*/

// dynamic ballance
short getBallanceCount(unsigned char counter) {
  if (robalParam.dynamicEnabled) {
    // get diving value
    if (m_legsValue.fl.lifted) {
      sumF += m_gyroState.aLiftFL;
    } else if (m_legsValue.fr.lifted) {
      sumF += m_gyroState.aLiftFR;
    } else if (m_legsValue.rl.lifted) {
      sumR += m_gyroState.aLiftRL;
    } else if (m_legsValue.rr.lifted) {
      sumR += m_gyroState.aLiftRR;
    }
    if (counter == 0) {
      sumF /= 10;
      sumR /= 10;
      // too heavy - negative
      //Serial.print(" F ");
      //Serial.print((int)sumF);
      //Serial.print(" R ");
      //Serial.println((int)sumR);
      if (sumF < sumR) {
        // front too heavy
        //Serial.println(" Front heavy ");
        dynamicForward --;
      } else if (sumF > sumR) {
        // rear too heavy
        //Serial.println(" Rear heavy ");
        dynamicForward ++;
      }
      sumF = 0;
      sumR = 0;
      if (dynamicForward > DYNAMIC_BALLANCE_MAX) {
        dynamicForward = DYNAMIC_BALLANCE_MAX;
      }
      if (dynamicForward < -DYNAMIC_BALLANCE_MAX) {
        dynamicForward = -DYNAMIC_BALLANCE_MAX;
      }
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
