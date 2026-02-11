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
  unsigned short current2;  // A3
  unsigned short current3;  // A2
};

// analog input values array
aCurrent analogValueCurrent = {LOW_BATTERY, 0, 0}; // processed values
unsigned int batteryV = 0;
unsigned int currentF = 0;
unsigned int currentR = 0;

// init current inputs
void initCurrent(void) {
}

// read and remember analog current sensors readings
void updateCurrent(void) {
  // end of previous cycle
  if (m_sequenceCounter.m == 0) {
    // average
    batteryV /= SERVO_FULL_CYCLE;
    currentF /= SERVO_FULL_CYCLE;
    currentR /= SERVO_FULL_CYCLE;
    // to ma
    if (batteryV > currentF) {
      analogValueCurrent.current2 = (batteryV - currentF) * 8;
    } else {
      analogValueCurrent.current2 = 0;
    }
    if (batteryV > currentR) {
      analogValueCurrent.current3 = (batteryV - currentR) * 8;
    } else {
      analogValueCurrent.current3 = 0;
    }
    analogValueCurrent.battery = (batteryV * 25) / 3;
    // get current state
    if (analogValueCurrent.battery < DEAD_BATTERY) {
      // battery dead
      m_robotState.currentStateNow = C_DEAD_BATTERY;
    } else if ((analogValueCurrent.current2 > MAX_CURRENT) || (analogValueCurrent.current3 > MAX_CURRENT)) {
      // motor 2 or 3 current too high
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
    currentF = (unsigned short)analogRead(A3);
    currentR = (unsigned short)analogRead(A2);
  } else {
    // add to value
    batteryV += (unsigned short)analogRead(A6);
    currentF += (unsigned short)analogRead(A3);
    currentR += (unsigned short)analogRead(A2);
  }
}
/*
// print raw data
void _printLineCurrent(void) {
  Serial.print(F(" battery "));
  Serial.print((int)analogValueCurrent.battery);
  Serial.print(F(" F "));
  Serial.print((int)analogValueCurrent.current2);
  Serial.print(F(" R "));
  Serial.println((int)analogValueCurrent.current3);
}
*/