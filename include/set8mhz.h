/*
 * For some reason, whenever PlatformIO programs the ATTiny85, it runs at 4MHZ.
 * This happens regardless of the set fuses, so putting this macro at the top of
 * setup() is the only way I've found that fixes the issue.
 * https://community.platformio.org/t/attiny-8mhz-wrong-timing-in-delay-fastled-and-neopixel/24992/2
 */
#define SET_8MHZ cli();CLKPR = (1<<CLKPCE);CLKPR = 0x00;sei();
/* code explanation
cli(); // Disable interrupts
CLKPR = (1<<CLKPCE); // Prescaler enable
CLKPR = 0x00; // Clock division factor
sei(); // Enable interrupts
*/
