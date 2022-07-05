/**
 * Assignment 2 EECS 497.32
 *
 * 1. Write a portable code using programming guidelines mentioned in the next slide.
 * 2. Turn ON and OFF the LEDs continuously with the ON and OFF duration to be approximately between 1 and 2 seconds.
 * 3. Use a software loop to create a delay routine to control the ON and OFF duration
 * 4. DO NOT use the delay library functions provided in the AVR libraries.
 *
 */

 // Include the hardware address macros.
#include "devreg.h"
// The I/O read and write wrapper functions are in a separate file, they will rarely
// need to be recompiled.
#include "reg_io_wrappers.h"

#define MY_MAX_UNSIGNED_INT ~0

static void myLoopDelay(unsigned int loopCount)
{
	// Delay using loop, one time through the loop didn't seem quite long enough
	for (volatile unsigned int outerTimingLoopCount = 0; outerTimingLoopCount < loopCount; outerTimingLoopCount++)
		for (volatile unsigned int timeLoopCounter = 0; timeLoopCounter < (unsigned int)MY_MAX_UNSIGNED_INT; timeLoopCounter++)
			;
}

// The LED state should be maintained for 1 to 2 seconds.
int main (void)
{
// What I am trying to accomplish is to get a 3, 2, 1, 0 sequence of LEDs, that repeats.
// I only see 3 of the 4 states when I run the program on the device.

	unsigned char ledEnableBits[] = {ALL_LEDS_ON, LED_1_ON, LED_0_ON, ALL_LEDS_OFF};
	unsigned int toggle_led = 0;

	// Future experiment, time how long until LEDs stay dark.
	while (1)
	{
		WriteReg(LED_ENABLE_REG, ENABLE_LEDS);
		// Use a modulo of 4 to control which value in the array is being used
		unsigned char try2 = ledEnableBits[toggle_led];
		WriteReg(LED_TOGGLE_REG, try2);

		// Keep each display state to approximately 1 to 2 seconds.
		myLoopDelay(2);
		WriteReg(LED_ENABLE_REG, DISABLE_LEDS);
		toggle_led++;
		if (toggle_led >= 4)
		{
			toggle_led = 0;
		}
			
	}
}
