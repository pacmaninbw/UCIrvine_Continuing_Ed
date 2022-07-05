/*
 * reg_io_wrappers.c
 *
 * Functions to read and write the hardware registers on the device.
 * Required for assignments from EECS 497.32 Fundamentals of Embedded Systems
 * Design and Programming
 *
 * Created: 4/27/2022 4:08:13 PM
 *  Author: Paul Chernick
 *
 * Change History:
 * 5/11/2022	Changed added include for stdint-gcc.h to reg_io_wrappers.h,
 *		changed input and output types to uint8_t. Easier to find and change
 *		than unsigned char, better representation.
 */ 

#include "reg_io_wrappers.h"

/* Write data to a hardware register */
void WriteReg(uint16_t RegAddress, uint8_t Value)
{
	*((volatile unsigned char *)RegAddress) = Value;
}

 /* Read data from a hardware register */
uint8_t ReadReg(uint16_t RegAddress)
{
	// disable interrupts
	uint8_t return_val = *((volatile uint8_t *)RegAddress);
	// enable interrupts

	return return_val;
}
