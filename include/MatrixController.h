#ifndef MATRIX_CONTROLLER_H
#define MATRIX_CONTROLLER_H

#include <Arduino.h>


// Class for controlling an 8x8 LED matrix with a MAX7219
class MatrixController {
 private:
  // Keep track of each led val (array of rows)
  uint8_t leds[8];
  // Data is shifted from this pin
  uint8_t DIN;
  // Clock signal for shifting
  uint8_t CLK;
  // Chip select, active low
  uint8_t CSB;
  // Send command "opcode" with data "data"
  void spiWrite(uint8_t opcode, uint8_t data);

 public:
  MatrixController(uint8_t dataPin,
                   uint8_t clkPin,
                   uint8_t csPin);

  // Initialize controller
  void begin();
  // set chip to shutdown mode iff "state"
  void setShutdown(bool state);
  // update the leds from the array values
  void show();
  // Set display intensity (0-15)
  void setIntensity(uint8_t val);
  // Clear values
  void clear();
  // Set the led at [row, col]
  void setLed(uint8_t row, uint8_t col, bool state);
  // Get the status of the led at [row, col]
  bool getLed(uint8_t row, uint8_t col);

  // Get the "i"th row
  uint8_t & operator[](uint8_t i);
};

#endif // MATRIX_CONTROLLER_H
