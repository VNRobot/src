/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs switches
*/

// pin numbers for switch
enum swPins {
  FL_SWITCH = 2,
  FR_SWITCH = 3,
  RL_SWITCH = 4,
  RR_SWITCH = 5
};

// switches data
switches swSum = {0, 0, 0, 0};

/*
uses
m_swState
m_swCount
*/

// init Switches
void initSwitches(bool calibrationMode) {
  Serial.println(F("initSwitches"));
  pinMode(FL_SWITCH, INPUT_PULLUP);
  pinMode(FR_SWITCH, INPUT_PULLUP);
  pinMode(RL_SWITCH, INPUT_PULLUP);
  pinMode(RR_SWITCH, INPUT_PULLUP);
  unsigned char counter = 0;
  while (calibrationMode) {
    counter ++;
    if (counter >= SERVO_FULL_CYCLE) {
      counter = 0;
    }
    delay(20);
    readSwitchesCount(counter);
    if (m_getButtonPressed()) {
      calibrationMode = false;
    }
    if (counter == 0) {
      Serial.print(F(" Count fl "));
      Serial.print((int)m_swCount.fl);
      Serial.print(F(" fr "));
      Serial.print((int)m_swCount.fr);
      Serial.print(F(" rl "));
      Serial.print((int)m_swCount.rl);
      Serial.print(F(" rr "));
      Serial.println((int)m_swCount.rr);
    }
  }
}

// read switches
void readSwitchesCount(unsigned char counter) {
  m_swState.fl = digitalRead(FL_SWITCH);
  m_swState.fr = digitalRead(FR_SWITCH);
  m_swState.rl = digitalRead(RL_SWITCH);
  m_swState.rr = digitalRead(RR_SWITCH);
  //
  if (counter == 0) {
    m_swCount.fl = swSum.fl;
    swSum.fl = 0;
    m_swCount.fr = swSum.fr;
    swSum.fr = 0;
    m_swCount.rl = swSum.rl;
    swSum.rl = 0;
    m_swCount.rr = swSum.rr;
    swSum.rr = 0;
    //Serial.println(" ");
  }
  //Serial.print(F(" "));
  //Serial.print((int)m_swState.fl);
  swSum.fl += m_swState.fl;
  swSum.fr += m_swState.fr;
  swSum.rl += m_swState.rl;
  swSum.rr += m_swState.rr;
  //
}
