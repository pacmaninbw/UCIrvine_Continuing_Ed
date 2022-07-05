/**
 * EECS 497.32 Programming Assignment 3: LED On/Off using switches
 *
 * 1. Create a code using Round Robin architecture to blink both LED’s when no
 *		switch is pressed like in previous assignment. 
 *
 * 2. When any switch is pressed, then only blink the LED(s) associated to the
 *		switch number. Use the SW1 switch to control LED 0 and SW2 switch to
 *		control LED 1.
 *
 * 3. Create a function that controls the blink operation.
 *
 * 4. The main task loop should check the switch setting and make decisions 
 *		whether to blink both LED's on at a time OR only blink the LEDS associated
 *		with the switches that are pressed.
 *
 * Change History
 * 05/11/2022 - Moved call to delay function from main round robin to service_leds().
 *		This will allow buttons to interrupt the delay loop and force an early
 *		from the service_leds() function.
 *		Added service_buttons() function.
 *		Added include for stdint-gcc.h so that uint8_t is defined, better than 
 *			putting unsigned char everywhere. Easier to modify if necessary.
 */

 #include <stdint-gcc.h>

/* Include the hardware address macros. */
#include "devreg.h"
/*
 * The I/O read and write wrapper functions are in a separate file, they will rarely
 * need to be recompiled.
 */
#include "reg_io_wrappers.h"

#define TRUE 1
#define FALSE 0

static uint8_t toggle = 0;
/*
 * Non-interrupt service function.
 * First determine if a button has been pushed, then determine which button
 * it is. Set the global button pushed flag.
 *
 * The button read back is in bits 1 and 2 (pins 1 and 2). Since the toggle
 * is an index into an array this needs to change to bits 0 and 1 so right
 * shift 1 bit. Since the pins are low when the button is pushed we need to
 * get the complement to find the proper value.
 */
static uint8_t service_buttons(uint8_t toggle)
{
	/*	Set PORTF direction to input
	 */
	WriteReg(BUTTON_ENABLE_REG, ENABLE_BUTTON_INPUT);
	uint8_t current_pin_read = ReadReg(BUTTON_VALUE_REG);	// Read back if any buttons are pushed.
	current_pin_read = current_pin_read >> 1;
	current_pin_read = ~current_pin_read & (uint8_t)0x03;
	current_pin_read = (current_pin_read)? current_pin_read : toggle;
	return current_pin_read;
}

static void myMillisecondDelay(unsigned int milliseconds)
{
/*
 * Inner loop should provide 1 millisecond delay, based on a processor speed
 * of 16 MIPS each instruction in the inner loop is is 0.065 microseconds.
 * An empty for loop has 6 instructions based on the assembly code so each
 * loop execution takes approximately 3.75 microseconds. 1 microsecond should
 * be approximately 267 executions of the loop. Subtract one loop execution
 * to allow for the 6 cycles the outer loop takes
 */
	const uint8_t ONE_MICROSECOND = 255;

	for (volatile unsigned int millisecondLoopCount = 0;
		millisecondLoopCount < milliseconds; millisecondLoopCount++)			
		for (volatile uint8_t microsecond_count = 0;
			microsecond_count < ONE_MICROSECOND; microsecond_count++)
			;
}

/*
 * Non-interrupt service function.
 */
static void service_leds(uint8_t pin_index)
{
	uint8_t pins[] = {0x01, 0x02, 0x03};
	uint8_t pin = pins[pin_index];

	WriteReg(LED_ENABLE_REG, pin);	/* Enable output on the specific pin */
	WriteReg(LED_TOGGLE_REG, ~pin);	/* Drive the pin low. */

	/*
	 * Keep each display state to approximately 1 to 2 seconds. Hopefully 1000
	 * loop executions equals 1 second. The delay function will be terminated
	 * if the button status has changed.
	 */
	myMillisecondDelay(350);
}

int main (void)
{
	/*
	 * toggle is a static variable, but it's scope is global to this file.
	 * The toggle variable can take on a third value if both buttons are down,
	 * within the main loop it can only be set to zero or 1.
	 */

	while (TRUE)
	{
	/* 
	 * Simple Round Robin Architecture without interrupts.
	 * Buttons are a higher priority than the LEDs because the buttons provide
	 * user input and require a hire response time. The buttons also change the
	 * status of which LEDs to display.
	 */
		toggle = service_buttons(toggle);
		service_leds(toggle);
		toggle = (toggle)? 0 : 1;
	}
}
