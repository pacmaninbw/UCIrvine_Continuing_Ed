/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
//#include <asf.h>

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

//	board_init();

	/* Insert application code here, after the board has been initialized. */

	// Set PORTR Direction Register (DIR) as output
	*((volatile unsigned char *)0x7E0) = 0xFF;

	// Turn LED0 ON and LED 1 OFF, by writing 0xFE to PORTR Toggle Register(OUTTGL), bit 0 set to 0 to turn On LED 0
	*((volatile unsigned char *)0x7E7) = 0xFE;

}
