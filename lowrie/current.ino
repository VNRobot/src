/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Gets analog current inputs
 - Reads power input voltage im mV
 - Reads power current from 3 independed channels in mA
    current1: A7 power current on connectors D2, D3, D4, D5
    current2: A3 power current on connectors D6, D7, D8, D9
    current3: A2 power current on connectors D10, D11, D12, D13
*/

// analog sensors structure
struct aCurrent {
  unsigned short battery;   // A6
  unsigned short current1;  // A7 current1
};

// analog input values array
aCurrent analogValueCurrent = {LOW_BATTERY, 0}; // processed values
unsigned long batteryV = 0;
unsigned long currentSum = 0;

// init current inputs
void initCurrent(void) {
}

// read and remember analog current sensors readings
void updateCurrent(void) {
  // end of previous cycle
  if (m_sequenceCounter.m == 0) {
    // average
    batteryV /= MAIN_FULL_CYCLE;
    currentSum /= MAIN_FULL_CYCLE;
    // to ma
    if (batteryV > currentSum) {
      analogValueCurrent.current1 = (batteryV - currentSum) * 3; // 8;
    } else {
      analogValueCurrent.current1 = 0;
    }
    analogValueCurrent.battery = (batteryV * 25) / 3;
    // get current state
    if (analogValueCurrent.battery < DEAD_BATTERY) {
      // battery dead
      m_robotState.currentStateNow = C_DEAD_BATTERY;
    } else if (analogValueCurrent.current1 > MAX_CURRENT) {
      // current too high
      m_robotState.currentStateNow = C_HIGH_CURRENT;
    } else if (analogValueCurrent.battery < LOW_BATTERY) {
      // battery low
      m_robotState.currentStateNow = C_LOW_BATTERY;
    } else {
      m_robotState.currentStateNow = C_NORMAL;
    }
    //_printLineCurrent();
    // new value
    batteryV = (unsigned short)analogRead(A6);
    currentSum = (unsigned short)analogRead(A7);
  } else {
    // add to value
    batteryV += (unsigned short)analogRead(A6);
    currentSum += (unsigned short)analogRead(A7);
  }
}
/*
// print raw data
void _printLineCurrent(void) {
  Serial.print(F(" Battery "));
  Serial.print((int)analogValueCurrent.battery);
  Serial.print(F(" Current "));
  Serial.print((int)analogValueCurrent.current1);
}
*/