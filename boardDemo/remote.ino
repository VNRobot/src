/*
Board Lowrie 1.1 demo code
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Reads Infrared Remote Control Receiver
 - Connect IR receiver to S1 connector
 - When using NEC remote it will decode numbers buttons
*/

#define DECODE_NEC          // comment to enable all protocols
#define IR_RECEIVE_PIN 2    // works only with pin 2 (connector S1)
#include <IRremote.hpp>

unsigned char irCode = 255; // received ir code

// init IR remote receiver
void initRemote(void) {
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

// read data from IR remote receiver
unsigned char readRemote(void) {
  irCode = 255;
  if (IrReceiver.decode()) {
    switch (IrReceiver.decodedIRData.command) {
      case 22: // 0
        irCode = 0;
      break;
      case 12: // 1
        irCode = 1;
      break;
      case 24: // 2
        irCode =  2;
      break;
      case 94: // 3
        irCode =  3;
      break;
      case 8: // 4
        irCode =  4;
      break;
      case 28: // 5
        irCode =  5;
      break;
      case 90: // 6
        irCode =  6;
      break;
      case 66: // 7
        irCode =  7;
      break;
      case 82: // 8
        irCode =  8;
      break;
      case 74: // 9
        irCode =  9;
      break;
      default:
        Serial.print("Unknown command with code ");
        Serial.println(IrReceiver.decodedIRData.command);
      break;
    }
    IrReceiver.resume(); // Enable receiving of the next value
  }
  return irCode;
}