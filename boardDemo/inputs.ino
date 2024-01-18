/*
Board Lowrie 1.1 demo code
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Get analog inputs
*/

// sensors structure
struct sensors {
  unsigned short left;
  unsigned short right;
  unsigned short current1;
  unsigned short current2;
  unsigned short current3;
  unsigned short battery;
};
// analog input values array
sensors analogInputs = {0, 0, 0, 0, 0, 0};

// read and remember analog sensors readings
void readInputs(void) {
    analogInputs.left = (unsigned short)analogRead(A1);
    analogInputs.right = (unsigned short)analogRead(A0);
    analogInputs.current1 = (unsigned short)analogRead(A7);
    analogInputs.current2 = (unsigned short)analogRead(A3);
    analogInputs.current3 = (unsigned short)analogRead(A2);
    analogInputs.battery = (unsigned short)analogRead(A6);
}

// get battery voltage in mV or 0 if can't read
unsigned short getBatteryInputs(void) {
  // battery low level is 6.2v - 750
  if (analogInputs.battery >= 400) {
    return (unsigned short)((int)analogInputs.battery * 100) / 12;
  }
  // battery is not available
  return 0;
}

// check mode button pressed
bool checkModeButtonPressedInputs(void) {
  // button is pressed if lower than 400
  if (analogInputs.battery < 400) {
    return true;
  }
  return false;
}

// get current of S1-S4 motors in mA
unsigned short getCurrent1Inputs(void) {
  if (analogInputs.battery < analogInputs.current1) {
    return 0;
  }
  return (analogInputs.battery - analogInputs.current1) * 8;
}

// get current of S5-S8 motors in mA
unsigned short getCurrent2Inputs(void) {
  if (analogInputs.battery < analogInputs.current2) {
    return 0;
  }
  return (analogInputs.battery - analogInputs.current2) * 8;
}

// get current of S9-S12 motors in mA
unsigned short getCurrent3Inputs(void) {
  if (analogInputs.battery < analogInputs.current3) {
    return 0;
  }
  return (analogInputs.battery - analogInputs.current3) * 8;
}

// get left analog sensor
unsigned short getLeftInputs(void) {
  return analogInputs.left;
}

// get right analog sensor
unsigned short getRightInputs(void) {
  return analogInputs.right;
}
