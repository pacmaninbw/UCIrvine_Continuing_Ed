/*
 * reg_io_wrappers.h
 *
 * Functions to read and write the hardware registers on the device.
 * Required for assignments from EECS 497.32 Fundamentals of Embedded Systems
 * Design and Programming/
 *
 * Created: 4/27/2022 4:20:26 PM
 *  Author: Paul Chernick
 *
 * Change History:
 * 5/11/2022	Changed added include for stdint-gcc.h to reg_io_wrappers.h,
 *		changed input and output types to uint8_t. Easier to find and change
 *		than unsigned char, better representation.
 */ 

#ifndef REG_IO_WRAPPERS_H_
#define REG_IO_WRAPPERS_H_

#include <stdint-gcc.h>

extern	void WriteReg(unsigned int RegAddress, uint8_t Value);
extern  uint8_t ReadReg(unsigned int RegAddress);

#endif /* REG_IO_WRAPPERS_H_ */