#ifndef pins__1
#define pins__1
#include <Arduino.h>

namespace Pins {
const uint8_t switchButton = D7;
const uint8_t buttonDown = D3;
const uint8_t buttonUp = D4;
const uint8_t ir = D2;
const uint8_t relayUp = D6;
const uint8_t relayDown = D5;
const uint8_t lightSensor = A0;
}  // namespace Pins

namespace Setup {
void pinSetup() {
    pinMode(Pins::switchButton, OUTPUT);
    pinMode(Pins::buttonDown, INPUT_PULLUP);
    pinMode(Pins::buttonUp, INPUT_PULLUP);
    pinMode(Pins::relayUp, OUTPUT);
    pinMode(Pins::relayDown, OUTPUT);

    digitalWrite(Pins::relayDown, HIGH);
    digitalWrite(Pins::relayUp, HIGH);
}
}  // namespace Setup

#endif