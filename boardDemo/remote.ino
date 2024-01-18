/*
Board Lowrie 1.1 demo code
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Remote control receiver
*/

#define DECODE_NEC          // comment to enable all protocols
#define IR_RECEIVE_PIN 2    // works only with pin 2
#include <IRremote.hpp>

// init IR remote receiver
void initRemote(void) {
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

// read data from IR remote receiver
unsigned char readRemote(void) {
  if (IrReceiver.decode()) {
    switch (IrReceiver.decodedIRData.command) {
      case 22: // 0
        return 0;
      break;
      case 12: // 1
        return 1;
      break;
      case 24: // 2
        return 2;
      break;
      case 94: // 3
        return 3;
      break;
      case 8: // 4
        return 4;
      break;
      case 28: // 5
        return 5;
      break;
      case 90: // 6
        return 6;
      break;
      case 66: // 7
        return 7;
      break;
      case 82: // 8
        return 8;
      break;
      case 74: // 9
        return 9;
      break;
      default:
        Serial.print("Unknown command with code ");
        Serial.println(IrReceiver.decodedIRData.command);
      break;
    }
    IrReceiver.resume(); // Enable receiving of the next value
  }
  return 255;
}