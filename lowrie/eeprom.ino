/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Read and write eeprom for software version and calibration data
*/

#include <EEPROM.h>

#define EEPROM_SIZE 13

void initEEPROM(void) {
#ifdef BOARD_ESP32
  EEPROM.begin(EEPROM_SIZE);
#endif
}

// write software version to eeprom
void writeSoftwareVersionEeprom(unsigned char _version) {
  // software version address is 0
#ifdef BOARD_ESP32
  EEPROM.write(0, _version);
  EEPROM.commit();
#else
  EEPROM.update(0, _version);
#endif
}

// read software version from eeprom
unsigned char readSoftwareVersionEeprom() {
  // software version address is 0
  return EEPROM.read(0);
}

// write calibration to eeprom
void writeCalibrationEeprom(allMotors calibration) {
  // calibration address is 1 to 12
#ifdef BOARD_ESP32
  EEPROM.write(1, (unsigned char)(calibration.hl.motor1 + 128));
  EEPROM.write(2, (unsigned char)(calibration.hl.motor2 + 128));
  EEPROM.write(3, (unsigned char)(calibration.hr.motor1 + 128));
  EEPROM.write(4, (unsigned char)(calibration.hr.motor2 + 128));
  EEPROM.write(5, (unsigned char)(calibration.fl.motor1 + 128));
  EEPROM.write(6, (unsigned char)(calibration.fl.motor2 + 128));
  EEPROM.write(7, (unsigned char)(calibration.fr.motor1 + 128));
  EEPROM.write(8, (unsigned char)(calibration.fr.motor2 + 128));
  EEPROM.write(9, (unsigned char)(calibration.rl.motor1 + 128));
  EEPROM.write(10, (unsigned char)(calibration.rl.motor2 + 128));
  EEPROM.write(11, (unsigned char)(calibration.rr.motor1 + 128));
  EEPROM.write(12, (unsigned char)(calibration.rr.motor2 + 128));
  EEPROM.commit();
#else
  EEPROM.update(1, (unsigned char)(calibration.hl.motor1 + 128));
  EEPROM.update(2, (unsigned char)(calibration.hl.motor2 + 128));
  EEPROM.update(3, (unsigned char)(calibration.hr.motor1 + 128));
  EEPROM.update(4, (unsigned char)(calibration.hr.motor2 + 128));
  EEPROM.update(5, (unsigned char)(calibration.fl.motor1 + 128));
  EEPROM.update(6, (unsigned char)(calibration.fl.motor2 + 128));
  EEPROM.update(7, (unsigned char)(calibration.fr.motor1 + 128));
  EEPROM.update(8, (unsigned char)(calibration.fr.motor2 + 128));
  EEPROM.update(9, (unsigned char)(calibration.rl.motor1 + 128));
  EEPROM.update(10, (unsigned char)(calibration.rl.motor2 + 128));
  EEPROM.update(11, (unsigned char)(calibration.rr.motor1 + 128));
  EEPROM.update(12, (unsigned char)(calibration.rr.motor2 + 128));
#endif
}

// read calibration from eeprom
void readCalibrationEeprom(allMotors * calibration) {
  // calibration address is 1 to 12
  calibration->hl.motor1 = (char)(EEPROM.read(1) - 128);
  calibration->hl.motor2 = (char)(EEPROM.read(2) - 128);
  calibration->hr.motor1 = (char)(EEPROM.read(3) - 128);
  calibration->hr.motor2 = (char)(EEPROM.read(4) - 128);
  calibration->fl.motor1 = (char)(EEPROM.read(5) - 128);
  calibration->fl.motor2 = (char)(EEPROM.read(6) - 128);
  calibration->fr.motor1 = (char)(EEPROM.read(7) - 128);
  calibration->fr.motor2 = (char)(EEPROM.read(8) - 128);
  calibration->rl.motor1 = (char)(EEPROM.read(9) - 128);
  calibration->rl.motor2 = (char)(EEPROM.read(10) - 128);
  calibration->rr.motor1 = (char)(EEPROM.read(11) - 128);
  calibration->rr.motor2 = (char)(EEPROM.read(12) - 128);
}
