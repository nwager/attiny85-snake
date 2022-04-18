#include "MatrixController.h"

// opcodes (register addresses) for the MAX7219
#define OP_NOOP   0x0
#define OP_DIGIT0 0x1
#define OP_DIGIT2 0x3
#define OP_DIGIT1 0x2
#define OP_DIGIT3 0x4
#define OP_DIGIT4 0x5
#define OP_DIGIT5 0x6
#define OP_DIGIT6 0x7
#define OP_DIGIT7 0x8
#define OP_DECODEMODE  0x9
#define OP_INTENSITY   0xA
#define OP_SCANLIMIT   0xB
#define OP_SHUTDOWN    0xC
#define OP_DISPLAYTEST 0xF

MatrixController::MatrixController(uint8_t dataPin,
                                   uint8_t clkPin,
                                   uint8_t csPin)
  : DIN{dataPin}, CLK{clkPin}, CSB{csPin} {}

void MatrixController::begin() {
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(CSB, OUTPUT);
  digitalWrite(CSB, HIGH);
  // turn off display test
  spiWrite(OP_DISPLAYTEST, 0);
  // set scan limit to max (8 digits)
  spiWrite(OP_SCANLIMIT, 7);
  // no decoding
  spiWrite(OP_DECODEMODE, 0);
  // deafult to medium intensity
  setIntensity(8);

  setShutdown(false);
  clear();
  show();
}

void MatrixController::setShutdown(bool state) {
  spiWrite(OP_SHUTDOWN, state ? 0 : 1);
}

void MatrixController::show() {
  for (uint8_t i = 1; i < 9; i++) {
    spiWrite(i, leds[i-1]); // MAX7219 rows are 1-based
  }
}

void MatrixController::setIntensity(uint8_t val) {
  spiWrite(OP_INTENSITY, val & 0x0F); // max value of 15
}

void MatrixController::clear() {
  for (uint8_t i = 0; i < 8; i++) {
    leds[i] = 0x00;
  }
}

void MatrixController::setLed(uint8_t row, uint8_t col, bool state) {
  uint8_t mask = 1 << col;
  if (state) leds[row] |= mask; // turn on led at "x" without changing others
  else leds[row] &= ~mask; // turn off led at "x" without changing others
}

bool MatrixController::getLed(uint8_t row, uint8_t col) {
  return leds[row] & (1 << col);
}

void MatrixController::spiWrite(uint8_t opcode, uint8_t data) {
  // enable line
  digitalWrite(CSB, LOW);
  // send opcode
  shiftOut(DIN, CLK, MSBFIRST, opcode);
  // send data
  shiftOut(DIN, CLK, MSBFIRST, data);
  // disable line
  digitalWrite(CSB, HIGH);
}

uint8_t & MatrixController::operator[](uint8_t i) {
  return leds[i];
}
