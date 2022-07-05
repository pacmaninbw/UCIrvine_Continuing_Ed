/*
 * reg_io_wrappers.c
 *
 * Functions to read and write the hardware registers on the device.
 * Required for assignments from EECS 497.32 Fundamentals of Embedded Systems
 * Design and Programming
 *
 * Created: 4/27/2022 4:08:13 PM
 *  Author: Paul Chernick
 */ 

 #include "reg_io_wrappers.h"

 // Write data to a hardware register
 void WriteReg(unsigned int RegAddress, unsigned int Value)
 {
	unsigned char Mask = ~0;
	*((volatile unsigned char *)RegAddress) = Mask & Value;
 }

 // Read data from a hardware register
 unsigned char ReadReg(unsigned int RegAddress)
 {
	return *((volatile unsigned char *)RegAddress);
 }
