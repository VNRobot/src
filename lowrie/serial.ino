/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Serial communication
*/

// serial attached flag
bool serialAttached = false;

/*
uses
m_mainData
*/

// attach current
void attachSerial(void) {
  serialAttached = true;
}

// send data
void sendSerial() {
  if (serialAttached) {
    if (m_mainData.masterDevice) {
      // send m_mainData.robotVersion, m_mainData.patternNow, m_mainData.speedLeft, m_mainData.speedRight, m_mainData.shiftForward
      Serial.print(m_mainData.robotVersion);
      Serial.print(m_mainData.patternNow);
      Serial.print(m_mainData.speedLeft);
      Serial.print(m_mainData.speedRight);
      Serial.print(m_mainData.shiftForward);
      Serial.print(m_mainData.directionCorrection);
    } else {
      // send m_mainData.currentState
      Serial.print(m_mainData.currentState);
    }
  }
}

// receive data
int receiveSerial() {
  if (serialAttached) {
    int counter = Serial.available();
    if (m_mainData.masterDevice) {
      // receive m_mainData.currentState
      if (counter > 0) {
          m_mainData.currentState = Serial.read();
      }
    } else {
      // receive m_mainData.robotVersion, m_mainData.patternNow, m_mainData.speedLeft, m_mainData.speedRight, m_mainData.shiftForward
      if (counter > 5) {
        char version = Serial.read();
        if (version == m_mainData.robotVersion) {
          m_mainData.patternNow = Serial.read();
          m_mainData.speedLeft = Serial.read();
          m_mainData.speedRight = Serial.read();
          m_mainData.shiftForward = Serial.read();
          m_mainData.directionCorrection = Serial.read();
          // set counter
          setPatternOfTask(m_mainData.patternNow);
          setHalfCounter(getWalkingModeInTask());
        }
      }
    }
    return Serial.available();
  }
  return 0;
}
