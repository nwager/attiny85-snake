# Tiny Snake on an ATTiny85

This project was a fun exercise in hardware-constrained C++, [PlatformIO](https://platformio.org/), and SPI communication.

## Firmware
The code runs on an ATTiny85 microprocessor using the Arduino framework. In the spirit of reducing memory usage in every possible way, I decided to write a bare-bones API for communicating with the MAX7219 chip, as opposed to using the popular [LedControl](http://wayoda.github.io/LedControl/) library (although that cource code was a helpful reference).

The LED matrix API constructs a controller object `MatrixController` that houses methods to set different modes on the MAX7219 and control the LEDs. Snake

## Hardware
All five input pins on the ATTiny85 are used (three for the MAX7219, two for the joystick pots). The system runs on 5V and draws a maximum of around 2A when all 64 LEDs are at full brightness.

## Results
```
PlatformIO build output
-----------------------
RAM:   [==        ]  22.9% (used 117 bytes from 512 bytes)
Flash: [===       ]  31.6% (used 2588 bytes from 8192 bytes)
```
Most of the RAM usage comes from the 64-byte array used to keep track of each segment of the snake. Overall, I'm satisfied with the memory efficiency of the implementation, as it leaves enough room to add extra features if desired.

Performance-wise, the display only updates a few times per second, so any performance issues are unnoticeable. The ATTiny85 running at 8MHz seems to handle the multiplexing and SPI communication well, at least in this situation.

## Parts List
- ATTiny85
- 8x8 red LED matrix module with a MAX7219 driver chip
- dual-potentiometer joystick