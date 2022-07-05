/*
 * devreg.h
 *	Portable Register Addressing to allow the code using this file to port to
 *	other devices.
 *  Required for assignments from EECS 497.32 Fundamentals of Embedded Systems
 *  Design and Programming.
 *
 * Created: 4/27/2022 3:33:12 PM
 *  Author: Paul Chernick
 *
 * Change History
 * Created 4/27/2022.Currently only contains macros to program the PORTR
 *		functionality, to enable and disable the LEDs. To make the LED
 *		programming more portable LED macros were added to hide the PORTR
 *		implementation on the device.
 *
 * 5/1/2022. Changed the PORT Address macros implementation, Ports can be added
 *		by copy and paste, select region, find and replace base port register
 *		name.
 *
 * 5/8/2022 Added PORTF Macros.
 *
 * 5/10/2022 Converted LED constants to use the complement of decimal numbers
 *		rather than Hex values.
 */ 

#ifndef DEVREG_H_
#define DEVREG_H_

// LED control values
// For this device there are 2 LEDs, so there are 4 LED states, all LEDs off,
// all LEDs on, LED 0 on, LED 1 on.
// The yellow LEDs are controlled by pins 0 and 1 in PORTR. The LED controlled
// by pin 0 will be called LED 0 and the LED controlled by pin 1 will be called
// LED 1. When driving low on either pin 0 or pin 1 the associated LED will
// light up.
#define ALL_LEDS_OFF		(unsigned char) ~0x00
#define ALL_LEDS_ON			(unsigned char) ~0x03
#define LED_0_ON			(unsigned char) ~0x01
#define LED_1_ON			(unsigned char) ~0x02
#define LED_MAX_STATES		4
#define LED_STATE_MASK		0x03 * The 2 LSBs in the PORT R register 

#define PORT_DIR_OUTPUT		0xFF /* Page 148 of the Manual Bits 0 and 1 output */
#define PORT_DIR_INPUT		0xFC /* Bits 0 and 1 input */

// Device Port Address Offsets
// These offsets are documented on page 160 of the manual
// Atmel-8331-8-and-16-bit-AVR-Microcontroller-XMEGA-AU_Manual.pdf
#define PORT_DIR_OFFSET		0x00
#define PORT_DIRSET_OFFSET	0x01
#define PORT_DIRCLR_OFFSET	0X02
#define PORT_DIRTGL_OFFSET	0X03
#define PORT_OUT_OFFSET		0x04
#define PORT_OUTSET_OFFSET	0x05
#define PORT_OUTCLR_OFFSET	0x06
#define PORT_OUTTGL_OFFSET	0x07
#define PORT_IN_OFFSET		0x08

// Adding Device Port Offsets
#define ADD_PORT_DIR_OFFSET(baseAddress)	\
		 (baseAddress + PORT_DIR_OFFSET)
#define ADD_PORT_DIRSET_OFFSET(baseAddress)	\
		(baseAddress + PORT_DIRSET_OFFSET)
#define ADD_PORT_DIRCLR_OFFSET(baseAddress)	\
		(baseAddress + PORT_DIRCLR_OFFSET)
#define ADD_PORT_DIRTGL_OFFSET(baseAddress)	\
		(baseAddress + PORT_DIRTGL_OFFSET)
#define ADD_PORT_OUT_OFFSET(baseAddress)	\
		(baseAddress + PORT_OUT_OFFSET)
#define ADD_PORT_OUTSET_OFFSET(baseAddress)	\
		(baseAddress + PORT_OUTSET_OFFSET)
#define ADD_PORT_OUTCLR_OFFSET(baseAddress) \
		(baseAddress + PORT_OUTCLR_OFFSET)
#define ADD_PORT_OUTTGL_OFFSET(baseAddress)	\
		(baseAddress + PORT_OUTTGL_OFFSET)
#define ADD_PORT_IN_OFFSET(baseAddress)		\
		(baseAddress + PORT_IN_OFFSET)

/* PORT R Device Addresses */
#define PORTR_BASE_ADDRESS	0x07E0
#define PORTR_DIR_REG		ADD_PORT_DIR_OFFSET(PORTR_BASE_ADDRESS)
#define PORTR_DIRSET_REG	ADD_PORT_DIRSET_OFFSET(PORTR_BASE_ADDRESS)
#define PORTR_DIRCLR_REG	ADD_PORT_DIRCLR_OFFSET(PORTR_BASE_ADDRESS)
#define PORTR_DIRTGL_REG	ADD_PORT_DIRTGL_OFFSET(PORTR_BASE_ADDRESS)
#define PORTR_OUT_REG		ADD_PORT_OUT_OFFSET(PORTR_BASE_ADDRESS)
#define PORTR_OUTSET_REG	ADD_PORT_OUTSET_OFFSET(PORTR_BASE_ADDRESS)
#define PORTR_OUTCLR_REG	ADD_PORT_OUTCLR_OFFSET(PORTR_BASE_ADDRESS)
#define PORTR_OUTTGL_REG	ADD_PORT_OUTTGL_OFFSET(PORTR_BASE_ADDRESS)
#define PORTR_IN_REG		ADD_PORT_IN_OFFSET(PORTR_BASE_ADDRESS)

/* PORT F Device Addresses */
#define PORTF_BASE_ADDRESS	0x06A0
#define PORTF_DIR_REG		ADD_PORT_DIR_OFFSET(PORTF_BASE_ADDRESS)
#define PORTF_DIRSET_REG	ADD_PORT_DIRSET_OFFSET(PORTF_BASE_ADDRESS)
#define PORTF_DIRCLR_REG	ADD_PORT_DIRCLR_OFFSET(PORTF_BASE_ADDRESS)
#define PORTF_DIRTGL_REG	ADD_PORT_DIRTGL_OFFSET(PORTF_BASE_ADDRESS)
#define PORTF_OUT_REG		ADD_PORT_OUT_OFFSET(PORTF_BASE_ADDRESS)
#define PORTF_OUTSET_REG	ADD_PORT_OUTSET_OFFSET(PORTF_BASE_ADDRESS)
#define PORTF_OUTCLR_REG	ADD_PORT_OUTCLR_OFFSET(PORTF_BASE_ADDRESS)
#define PORTF_OUTTGL_REG	ADD_PORT_OUTTGL_OFFSET(PORTF_BASE_ADDRESS)
#define PORTF_IN_REG		ADD_PORT_IN_OFFSET(PORTF_BASE_ADDRESS)

/* PORT A Device Addresses */
#define PORTA_BASE_ADDRESS	0x0600
#define PORTA_DIR_REG		ADD_PORT_DIR_OFFSET(PORTA_BASE_ADDRESS)
#define PORTA_DIRSET_REG	ADD_PORT_DIRSET_OFFSET(PORTA_BASE_ADDRESS)
#define PORTA_DIRCLR_REG	ADD_PORT_DIRCLR_OFFSET(PORTA_BASE_ADDRESS)
#define PORTA_DIRTGL_REG	ADD_PORT_DIRTGL_OFFSET(PORTA_BASE_ADDRESS)
#define PORTA_OUT_REG		ADD_PORT_OUT_OFFSET(PORTA_BASE_ADDRESS)
#define PORTA_OUTSET_REG	ADD_PORT_OUTSET_OFFSET(PORTA_BASE_ADDRESS)
#define PORTA_OUTCLR_REG	ADD_PORT_OUTCLR_OFFSET(PORTA_BASE_ADDRESS)
#define PORTA_OUTTGL_REG	ADD_PORT_OUTTGL_OFFSET(PORTA_BASE_ADDRESS)
#define PORTA_IN_REG		ADD_PORT_IN_OFFSET(PORTA_BASE_ADDRESS)

/* PORT B Device Addresses */
#define PORTB_BASE_ADDRESS	0x0620
#define PORTB_DIR_REG		ADD_PORT_DIR_OFFSET(PORTB_BASE_ADDRESS)
#define PORTB_DIRSET_REG	ADD_PORT_DIRSET_OFFSET(PORTB_BASE_ADDRESS)
#define PORTB_DIRCLR_REG	ADD_PORT_DIRCLR_OFFSET(PORTB_BASE_ADDRESS)
#define PORTB_DIRTGL_REG	ADD_PORT_DIRTGL_OFFSET(PORTB_BASE_ADDRESS)
#define PORTB_OUT_REG		ADD_PORT_OUT_OFFSET(PORTB_BASE_ADDRESS)
#define PORTB_OUTSET_REG	ADD_PORT_OUTSET_OFFSET(PORTB_BASE_ADDRESS)
#define PORTB_OUTCLR_REG	ADD_PORT_OUTCLR_OFFSET(PORTB_BASE_ADDRESS)
#define PORTB_OUTTGL_REG	ADD_PORT_OUTTGL_OFFSET(PORTB_BASE_ADDRESS)
#define PORTB_IN_REG		ADD_PORT_IN_OFFSET(PORTB_BASE_ADDRESS)

/* PORT C Device Addresses */
#define PORTC_BASE_ADDRESS	0x0620
#define PORTC_DIR_REG		ADD_PORT_DIR_OFFSET(PORTC_BASE_ADDRESS)
#define PORTC_DIRSET_REG	ADD_PORT_DIRSET_OFFSET(PORTC_BASE_ADDRESS)
#define PORTC_DIRCLR_REG	ADD_PORT_DIRCLR_OFFSET(PORTC_BASE_ADDRESS)
#define PORTC_DIRTGL_REG	ADD_PORT_DIRTGL_OFFSET(PORTC_BASE_ADDRESS)
#define PORTC_OUT_REG		ADD_PORT_OUT_OFFSET(PORTC_BASE_ADDRESS)
#define PORTC_OUTSET_REG	ADD_PORT_OUTSET_OFFSET(PORTC_BASE_ADDRESS)
#define PORTC_OUTCLR_REG	ADD_PORT_OUTCLR_OFFSET(PORTC_BASE_ADDRESS)
#define PORTC_OUTTGL_REG	ADD_PORT_OUTTGL_OFFSET(PORTC_BASE_ADDRESS)
#define PORTC_IN_REG		ADD_PORT_IN_OFFSET(PORTC_BASE_ADDRESS)

/* Portable names for device registers does not require knowledge of device */
#define LED_ENABLE_REG		PORTR_DIR_REG			
#define LED_TOGGLE_REG		PORTR_OUTTGL_REG
#define ENABLE_LEDS			PORT_DIR_OUTPUT
#define DISABLE_LEDS		PORT_DIR_INPUT

/*
 * Buttons
 * The SW1 and SW2 button input is available on PORTF PIN1 and PIN2 respectively
 * as documented on page 10 of doc8394.pdf.
 */
 #define SW1_AND_SW2_ARE_PRESSED	(uint8_t) ~0x06		// Pins 1 and 2 are low
 #define SW1_IS_PRESSED				(uint8_t) ~0x02		// Pin 1 is low
 #define SW2_IS_PRESSED				(uint8_t) ~0x04		// Pin 2 is low
 #define BUTTON_ENABLE_REG			PORTF_DIR_REG
 #define BUTTON_VALUE_REG			PORTF_IN_REG
 #define ENABLE_BUTTON_INPUT		(uint8_t) ~0x06


#endif /* DEVREG_H_ */