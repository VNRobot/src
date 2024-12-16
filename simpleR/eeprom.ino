/*
Simple Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Read and write eeprom for software version and calibration data
*/

#include <EEPROM.h>

// software version hardcoded
// should be changed manually
unsigned char _version = 71;

// get software version
unsigned char getSoftwareVersionEeprom(void) {
  return _version;
}

// write software version to eeprom
void writeSoftwareVersionEeprom(void) {
  // software version address is 0
  EEPROM.update(0, _version);
}

// read software version from eeprom
unsigned char readSoftwareVersionEeprom() {
  // software version address is 0
  return EEPROM.read(0);
}

// write calibration to eeprom
void writeCalibrationEeprom(allMotors calibration) {
  // calibration address is 1 to 10
  EEPROM.update(1, (unsigned char)(calibration.left + 128));
  EEPROM.update(2, (unsigned char)(calibration.right + 128));
  EEPROM.update(3, (unsigned char)(calibration.m.wheel.motorL + 128));
  EEPROM.update(4, (unsigned char)(calibration.m.wheel.motorR + 128));
}

// read calibration from eeprom
void readCalibrationEeprom(allMotors * calibration) {
  // calibration address is 1 to 10
  calibration->left = (char)(EEPROM.read(1) - 128);
  calibration->right = (char)(EEPROM.read(2) - 128);
  calibration->m.wheel.motorL = (char)(EEPROM.read(3) - 128);
  calibration->m.wheel.motorR = (char)(EEPROM.read(4) - 128);
}
