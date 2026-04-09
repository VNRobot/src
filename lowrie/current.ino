/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Gets analog current inputs
 - Reads power input voltage im mV
 - Reads power current from 3 independed channels in mA
    current1: A7 power current on connectors D2, D3, D4, D5
    current2: A3 power current on connectors D6, D7, D8, D9
    current3: A2 power current on connectors D10, D11, D12, D13
*/

// maximal pair of legs current in ma 2000 to disable
#define MAX_CURRENT             1000
// low battery level in mv
#define LOW_BATTERY             6200
#define DEAD_BATTERY            6000

// analog sensors structure
struct aCurrent {
  unsigned short battery;   // A6
  unsigned short current1;  // A7 current1
  unsigned short current2;  // A3 current2
  unsigned short current3;  // A2 current3
};

// analog input values array
aCurrent analogValueCurrent = {LOW_BATTERY, 0, 0, 0}; // processed values
unsigned long batteryV = 0;
unsigned long currentSum1 = 0;
unsigned long currentSum2 = 0;
unsigned long currentSum3 = 0;

/*
uses
m_robotState
m_getButtonPressed()
*/

// init current inputs
void initCurrent(bool calibrationMode) {
  unsigned char counter = 0;
  while (calibrationMode) {
    delay(100);
    if (m_getButtonPressed()) {
      calibrationMode = false;
    }
    updateCurrent(counter);
    if (counter == 0) {
      _printLineCurrent();
    }
    counter ++;
    if (counter >= MAIN_FULL_CYCLE) {
      counter = 0;
    }
  }
}

// read and remember analog current sensors readings
void updateCurrent(unsigned char counter) {
  // end of previous cycle
  if (counter == 0) {
    // average
    batteryV /= MAIN_FULL_CYCLE;
    currentSum1 /= MAIN_FULL_CYCLE;
    currentSum2 /= MAIN_FULL_CYCLE;
    currentSum3 /= MAIN_FULL_CYCLE;
    // to ma
    // current 1
    if (batteryV > currentSum1) {
      analogValueCurrent.current1 = (batteryV - currentSum1) * 8;
    } else {
      analogValueCurrent.current1 = 0;
    }
    // current 2
    if (batteryV > currentSum2) {
      analogValueCurrent.current2 = (batteryV - currentSum2) * 8;
    } else {
      analogValueCurrent.current2 = 0;
    }
    // current 3
    if (batteryV > currentSum3) {
      analogValueCurrent.current3 = (batteryV - currentSum3) * 8;
    } else {
      analogValueCurrent.current3 = 0;
    }
    analogValueCurrent.battery = (batteryV * 25) / 3;
    // get current state
    if (analogValueCurrent.battery < DEAD_BATTERY) {
      // battery dead
      m_robotState.currentStateNow = C_DEAD_BATTERY;
    } else if ((analogValueCurrent.current1 > MAX_CURRENT) || (analogValueCurrent.current2 > MAX_CURRENT) || (analogValueCurrent.current3 > MAX_CURRENT)) {
      // current too high
      m_robotState.currentStateNow = C_HIGH_CURRENT;
    } else if (analogValueCurrent.battery < LOW_BATTERY) {
      // battery low
      m_robotState.currentStateNow = C_LOW_BATTERY;
    } else {
      m_robotState.currentStateNow = C_NORMAL;
    }
    // new value
    batteryV = (unsigned short)analogRead(A6);
    currentSum1 = (unsigned short)analogRead(A7);
    currentSum2 = (unsigned short)analogRead(A3);
    currentSum3 = (unsigned short)analogRead(A2);
  } else {
    // add to value
    batteryV += (unsigned short)analogRead(A6);
    currentSum1 += (unsigned short)analogRead(A7);
    currentSum2 += (unsigned short)analogRead(A3);
    currentSum3 += (unsigned short)analogRead(A2);
  }
}

// print raw data
void _printLineCurrent(void) {
  Serial.print(F(" Battery "));
  Serial.print((int)analogValueCurrent.battery);
  Serial.print(F(" Current1 "));
  Serial.print((int)analogValueCurrent.current1);
  Serial.print(F(" Current2 "));
  Serial.print((int)analogValueCurrent.current2);
  Serial.print(F(" Current3 "));
  Serial.println((int)analogValueCurrent.current3);
}
