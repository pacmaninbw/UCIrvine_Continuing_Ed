/**
 * EECS 497.32 Programming Assignment 4: Add timing using Clock to LED On/Off using switches
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
 *
 * 05/14/2022 - 05/15/2022 - Converted the delay subroutine to use the
 *		TCC0 timer/counter from a timing loop.
 *
 * 05/16/2022 - Fixed bug in delay function, was not complementing the read back
 *		of the overflow flag. Moved the declaration of toggle into main(), there
 *		was no need for it to be global to the file.
 */

 #include <stdint-gcc.h>

/* Include the hardware address macros. */
#include "devreg.h"
/*
 * The I/O read and write wrapper functions are in a separate file, they will rarely
 * need to be recompiled.
 */
#include "reg_io_wrappers.h"

/*
 * Not including stdbool.h, using K & R solution instead.
 */
#define TRUE 1
#define FALSE 0
#define MAX_LEDS 2
// #define RED_DEBUG 1

#ifdef RED_DEBUG
/*
 * Used for debugging purposes.
 */
static void lightRedLED(void)
{
	uint8_t pin4 = 0x01 << 4;
	WriteReg(PORTD_DIR_REG, pin4);
	WriteReg(PORTD_OUTTGL_REG, ~pin4);
}
#endif

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
static uint8_t service_buttons(void)
{
	/*	Set PORTF direction to input
	 */
	WriteReg(BUTTON_ENABLE_REG, ENABLE_BUTTON_INPUT);
	uint8_t button_read = ReadReg(BUTTON_VALUE_REG) & BUTTON_MASK;	/* Read back if any buttons are pushed. */
	button_read >>= 1;
	button_read = ~button_read & (uint8_t)0x03;

	return button_read;
}

	/*
	 * Keep each display state to approximately 1 to 2 seconds. Hopefully 1000
	 * loop executions equals 1 second. The delay function will be terminated
	 * if the button status has changed. Use the TCC0 timer for the delay.
	 */
static void DelayUsingTCC0(uint8_t requestedDelay)
{
	/*
	 * As per the instructions for this weeks lab,
	 * 1) Set CTRLA for prescaling divide by 1024
	 * 2) Since we want the counter to overflow based on only the 8 bits in
	 *		the low counter register, set the high count register to all high.
	 * 3) Set the desired delay value in the lower 8 bits of the counter.
	 * 4) Make sure the overflow interrupt bit is cleared.
	 * 5) Poll the clock interrupt flags to check for overflow conditions.
	 */
	WriteReg(TCC0_INTFLAGS, CLEAR_OVFIF);
	WriteReg(TCC0_CTRLA, CLK_CTRL_OPTS_DIV1024);
	WriteReg(TCC0_COUNT_HI, 0xFF);
	/*
	 * Subtract the desired delay from the overflow value for the lower
	 * 8 bits of the timer/counter. The overflow should occur when the
	 * lower 8 bits reaches 0xFF.
	 */
	WriteReg(TCC0_COUNT_LOW, 0xFF - requestedDelay);

	/*
	 * When overflow occurs the overflow bit in the interrupt register goes low.
	 * Poll the interrupt flags for overflow.
	 */
	volatile uint8_t noOverflow = TRUE;
	while(noOverflow)
	{
		volatile uint8_t overFlowCheck = ReadReg(TCC0_INTFLAGS);
		noOverflow = ~overFlowCheck & OVFIF_MASK;
	}

	/* Make sure there are not interrupts for the rest of the program. */
	WriteReg(TCC0_INTFLAGS, CLEAR_OVFIF);
}

/*
 * Non-interrupt service function.
 *
 * Light one or more LEDs based on input.
 */
#define PIN0 (uint8_t) 0x01
#define PIN1 (uint8_t) 0x02
#define PINS_0_AND_1 (uint8_t) 0x03
static void service_leds(uint8_t pin_index)
{
	uint8_t pins[] = {PIN0, PIN1, PINS_0_AND_1};
	uint8_t pin = pins[pin_index];

	WriteReg(LED_ENABLE_REG, pin);	/* Enable output on the specific pin(s) */
	WriteReg(LED_TOGGLE_REG, ~pin);	/* Drive the pin low. */

	DelayUsingTCC0(250);
	WriteReg(LED_ENABLE_REG, 0);	/* Disable output on the pin */

}

int main (void)
{
	while (TRUE)
	{
	/* 
	 * Simple Round Robin Architecture without interrupts.
	 * Buttons are a higher priority than the LEDs because the buttons provide
	 * user input and require a better response time. The buttons also change
	 * the status of which LEDs to display.
	 */
		uint8_t LED_Switch = service_buttons();

		for(uint8_t LED_index=0; LED_index < MAX_LEDS; LED_index++)
		{					
			uint8_t This_LED = LED_Switch? LED_Switch : LED_index;

			service_leds(This_LED);
		}

	}
}
