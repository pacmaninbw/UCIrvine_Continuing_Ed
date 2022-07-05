/**
 * EECS 497.32 Programming Assignment 5: Add timing using library interrupt calls
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
 *
 * 05/21/2022 - Implemented delay using timing interrupt. To delay a full second
 *		a timing loop was also added. To see fast flashing of the LEDs comment out
 *		line # 54 #define USING_LONG_DELAY 1. Was able to debug issues in the
 *		flashing LEDs and button by slowing the timing down. Removed earlier timing
 *		delays.
 */
#include <asf.h>
#include <stdbool.h>
#include <stdint-gcc.h>

/* Include the hardware address macros. */
#include "devreg.h"
/*
 * The I/O read and write wrapper functions are in a separate file, they will rarely
 * need to be recompiled.
 */
#include "reg_io_wrappers.h"

#define MAX_LEDS 2
//#define RED_DEBUG 1
#define USING_BUTTONS 1
#define USING_TIMER_INTERRUPT 1
#define USING_LONG_DELAY 1

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

static void turnOffRedLED(void)
{
	WriteReg(PORTD_DIR_REG, 0);
}
#endif

#ifdef USING_BUTTONS
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
static uint8_t readButtons(void)
{
	/*	Set PORTF direction to input
	 */
	WriteReg(BUTTON_ENABLE_REG, ENABLE_BUTTON_INPUT);
	uint8_t button_read = ReadReg(BUTTON_VALUE_REG) & BUTTON_MASK;	/* Read back if any buttons are pushed. */
	button_read >>= 1;
	button_read = ~button_read & (uint8_t)0x03;

	return button_read;
}
#endif // USING_BUTTONS

#ifdef USING_TIMER_INTERRUPT
static uint8_t TimerInterruptFlag = 0;

/*
 * Copied from tc.h line 1400
 */
static void my_callback(void)
{
#ifdef RED_DEBUG
	lightRedLED();
#endif
	// User code to execute when the overflow occurs here
	//Important to clear Interrupt Flag
	tc_clear_overflow(&TCC0);
	TimerInterruptFlag = ~TimerInterruptFlag;
#ifdef RED_DEBUG
	turnOffRedLED();
#endif
}

/*
 * Set up the timer. Copied from tc.h line 1411
 */
static void timer_init(void)
{
	tc_enable(&TCC0);
	tc_set_overflow_interrupt_callback(&TCC0, my_callback);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, 64000);		/* 1000 isn't enough time, max period is 2 to the 16th power */
	tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
	cpu_irq_enable();
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
}

/*
 * Delay for one complete cycle of the timer. Depending on when this function
 * is called we may actually delay for almost 2 cycles.
 */
static void delay(void)
{
	volatile uint8_t oldFlagValue = TimerInterruptFlag;

	/*
	 * Wait for timer interval to complete. We don't know where in the cycle
	 * the timer/counter currently is. This seems to extend the period on the
	 * very first flash.
	 */
	while (TimerInterruptFlag == oldFlagValue)
		;
	/*
	 * Start a timing loop based on cycles of the timer/counter.
	 */
	while (TimerInterruptFlag == oldFlagValue)
		;
}

#ifdef USING_LONG_DELAY
static void longDelay(void)
{
	/*
	 * For a 1 second delay the period value just isn't long enough with
	 * only 16 bits. Loop over the delay, current value isn't exact, but
	 * it is close to one second.
	 */
	for (uint8_t i = 0; i < 50; i++)
	{
		delay();
	}
}
#endif // USING_LONG_DELAY

#endif // USING_TIMER_INTERRUPT

static void Toggle_LEDs(unsigned char This_LED)
{
	WriteReg(LED_TOGGLE_REG, This_LED);
#ifdef USING_LONG_DELAY
	longDelay();
#else
	delay();
#endif

	WriteReg(LEDS_OFF_REG, 0xFF);
	delay();
}

int main (void)
{
	/*
	 * System clock initialization and interrupt initialization.
	 */
	pmic_init();
	sysclk_init();

	/*
	 * Timer initialization.
	 */
	timer_init();

	/*
	 * Only need to enable the LEDs once.
	 */
	WriteReg(LED_ENABLE_REG, 0xff);

	while (true)
	{
		uint8_t buttonValue = readButtons();
		uint8_t mask = 0x01;
		for(uint8_t LED_index=0; LED_index < MAX_LEDS; LED_index++)
		{
			if (buttonValue)
			{
				Toggle_LEDs(buttonValue);
			}
			else
			{
				Toggle_LEDs(~mask);
			}
			mask <<= 1;
		}

	}
}
