/*
 * reg_io_wrappers.h
 *
 * Functions to read and write the hardware registers on the device.
 * Required for assignments from EECS 497.32 Fundamentals of Embedded Systems
 * Design and Programming/
 *
 * Created: 4/27/2022 4:20:26 PM
 *  Author: Paul Chernick
 */ 

#ifndef REG_IO_WRAPPERS_H_
#define REG_IO_WRAPPERS_H_

extern	void WriteReg(unsigned int RegAddress, unsigned int Value);
extern  unsigned char ReadReg(unsigned int RegAddress);

#endif /* REG_IO_WRAPPERS_H_ */