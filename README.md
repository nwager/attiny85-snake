# Tiny Snake on an ATTiny85

This project was a fun exercise in hardware-constrained C++, [PlatformIO](https://platformio.org/), and SPI communication.

## Firmware
The code runs on an ATTiny85 microprocessor using the Arduino framework. In the spirit of reducing memory usage in every possible way, I decided to write a bare-bones API for communicating with the MAX7219 chip, as opposed to using the popular [LedControl](http://wayoda.github.io/LedControl/) library (although that cource code was a helpful reference).

The LED matrix API constructs a controller object `MatrixController` that houses methods to set different modes on the MAX7219 and control the LEDs. Snake

## Hardware
All five input pins on the ATTiny85 are used (three for the MAX7219, two for the joystick pots). The system runs on 5V and draws a maximum of around 2A when all 64 LEDs are at full brightness.

## Results
### Memory
```
PlatformIO build output
-----------------------
RAM:   [==        ]  22.9% (used 117 bytes from 512 bytes)
Flash: [===       ]  31.6% (used 2588 bytes from 8192 bytes)
```
Most of the RAM usage comes from the 64-byte array used to keep track of each segment of the snake. Overall, I'm satisfied with the memory efficiency of the implementation, as it leaves enough room to add extra features if desired.

### Performance
The display only updates a few times per second, so any performance issues are unnoticeable. The ATTiny85 running at 8MHz seems to handle the multiplexing and SPI communication well, at least in this situation.

### Back-of-the-Envelope Power Calculations
Since the LEDs draw most of the current, I will assume the other electrical components draw negligible current.

Assuming the current draw is linear with respect to the snake length (i.e. the number fo active LEDs), the current can be modeled with the equation:

![i(L) = 2.5 L + 27.5 \text{ mA}](https://latex.codecogs.com/svg.image?i(L)%20=%202.5%20L%20&plus;%2027.5%20%5Ctext%7B%20mA%7D#gh-light-mode-only)

![i(L) = 2.5 L + 27.5 \text{ mA}](https://latex.codecogs.com/svg.image?%7B%5Ccolor%7BWhite%7D%20i(L)%20=%202.5%20L%20&plus;%2027.5%20%5Ctext%7B%20mA%7D%7D#gh-dark-mode-only)

In my test, the snake stayed at each length for an average of `3.19s`. This means that each interval for length `L` draws:

![5 \text{V} \cdot i(L) \cdot 3.19 \text{s} \text{ mJ}](https://latex.codecogs.com/svg.image?5%20%5Ctext%7BV%7D%20%5Ccdot%20i(L)%20%5Ccdot%203.19%20%5Ctext%7Bs%7D%20%5Ctext%7B%20mJ%7D#gh-light-mode-only)

![5 \text{V} \cdot i(L) \cdot 3.19 \text{s} \text{ mJ}](https://latex.codecogs.com/svg.image?%7B%5Ccolor%7BWhite%7D%205%20%5Ctext%7BV%7D%20%5Ccdot%20i(L)%20%5Ccdot%203.19%20%5Ctext%7Bs%7D%20%5Ctext%7B%20mJ%7D%7D#gh-dark-mode-only)

Each game starts at length `3`. Thus, the total energy used in a game played to length `L_max` is:

![E(L_{\text{max}}) = \sum_{n=3}^{L_{\text{max}}} \big( 5 \text{ V} \cdot i(n) \cdot 3.19 \text{ s} \big) \text{ mJ}](https://latex.codecogs.com/svg.image?E(L_%7B%5Ctext%7Bmax%7D%7D)%20=%20%5Csum_%7Bn=3%7D%5E%7BL_%7B%5Ctext%7Bmax%7D%7D%7D%20%5Cbig(%205%20%5Ctext%7B%20V%7D%20%5Ccdot%20i(n)%20%5Ccdot%203.19%20%5Ctext%7B%20s%7D%20%5Cbig)%20%5Ctext%7B%20mJ%7D#gh-light-mode-only)

![E(L_{\text{max}}) = \sum_{n=3}^{L_{\text{max}}} \big( 5 \text{ V} \cdot i(n) \cdot 3.19 \text{ s} \big) \text{ mJ}](https://latex.codecogs.com/svg.image?%7B%5Ccolor%7BWhite%7D%20E(L_%7B%5Ctext%7Bmax%7D%7D)%20=%20%5Csum_%7Bn=3%7D%5E%7BL_%7B%5Ctext%7Bmax%7D%7D%7D%20%5Cbig(%205%20%5Ctext%7B%20V%7D%20%5Ccdot%20i(n)%20%5Ccdot%203.19%20%5Ctext%7B%20s%7D%20%5Cbig)%20%5Ctext%7B%20mJ%7D%7D#gh-dark-mode-only)

To get the equivalent mWh, simply divide the value by `3600`. For example, my test game went to length `20`. Plugging this into the equation, we get a value of `6.18 mWh`. A 3V CR2032 battery has a nominal capacity of `660mWh`, so the game would have depleted `0.94%` of the battery (not inclding the energy used to boost the 3V battery to 5V).

From this expression, a full game (length 63) uses about `51.5 mWh`, or `7.8%` of a CR2032.

## Parts List
- ATTiny85
- 8x8 red LED matrix module with a MAX7219 driver chip
- dual-potentiometer joystick

## Prototype Setup
![The prototyping phase, complete with a cluttered desk.](/images/attiny85-snake-proto.jpeg)

The ATTiny85 sits on a [Sparkfun Tiny AVR Programmer](https://www.sparkfun.com/products/11801) connected to the matrix and joystick, surrounded by haphazard wires and electronics.
