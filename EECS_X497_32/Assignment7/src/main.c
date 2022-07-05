/**
 * Week 8 Programming Assignment 7
 * Student: Paul Chernick
 *
 * 1. Download the UART_Base_AVR1522 zipped file
 * 2. The UART_Base_AVR1522 code has 3 task projects built.Use the Task1 for this project(Polling method).
 * 3. Program the XMega board with the task1.hex fileLocation: Task1-Polling Controlled -> Debug -> task1.hex
 * 4. Connect the USB to Serial adapter to your PC
 * 5. Connect Pin 3(Tx) and 4(Rx) on J1 Jumper to the Serial Port Rx and Tx(Cross wired: Pin 3(Tx) connects to Rx on Serial Port, and vice versa for Pin 4)
 * 6. Download the Tera Term (Serial Port Terminal program)
 * 7. Configure the USB to Serial COM Port with 9600 baud, 8 bit, No Parity, 1 stop bit on the Tera Term and hit Connect
 * 8. Unplug and plug back the Xplained USB cable
 * 9. You should see characters displayed on the Tera Term
 * 10. Now modify the code so that when you enter a character key on the keyboard in the Tera Term window, that character is echoed back to the Tera Term.
 */

#include <asf.h>
#include <stdbool.h>
#include "board.h"
/* Include the hardware address macros. */
#include "devreg.h"
/*
 * The I/O read and write wrapper functions are in a separate file, they will rarely
 * need to be recompiled.
 */
#include "reg_io_wrappers.h"

#define BSCALE_VALUE	0
#define BSEL_VALUE		12

#define TEST_CHARS		240 //smy 100

/*
 * Indicate that errors occurred.
 */
static void lightRedLED(void)
{
	uint8_t pin4 = 0x01 << 4;
	WriteReg(PORTD_DIR_REG, pin4);
	WriteReg(PORTD_OUTTGL_REG, ~pin4);
}

static void lightLEDSOnSuccess(bool TransferError)
{
	LEDPORT.DIR = 0xFF;
	LEDPORT.OUT = 0xFF;
	
	while (1){
		/* Check for transfer error */
		if (TransferError){
			/* No light */
			LEDPORT.OUT = 0xFF;
			} else {
			/* Light */
			LEDPORT.OUT = 0x00;
		}
	}
}

static void initUSART(void)
{
	/* Pin 3 (TX) as output */
	USART_PORT.DIRSET   = PIN3_bm;   
	/* Pin 2 (RX) as input */
	USART_PORT.DIRCLR   = PIN2_bm;

	/* USARTx0, 8 Data bits, No Parity, 1 Stop bit */
	USART.CTRLC = (uint8_t) USART_CHSIZE_8BIT_gc 
		| USART_PMODE_DISABLED_gc 
		| false;

	/* Target: Internal RC 2MHz (default) 
	 * Example (9600bps)  :   - I/O clock = 2MHz
	 *                        - 9600bps can be achieved by 9600bps / 2^0
	 *                        - UBRR = 2MHz/(16*9600)-1 = 12.02
	 *                        - ScaleFactor = 0
	 */
	USART.BAUDCTRLA = BSEL_VALUE;

	/* Enable both RX and TX */
	USART.CTRLB |= USART_RXEN_bm;
	USART.CTRLB |= USART_TXEN_bm;
}

/*
 * Receive all the characters being sent from the TeraTerm program. If there
 * are too many characters for the buffer provide an error signal using the
 * red LED. All input is expected to be terminated with a Carriage return.
 */
static uint8_t receiveCharactersFromTeraTerm(volatile char *Rx_Buf, uint8_t Rx_Buf_Size)
{
	uint8_t Rx_Count = 0;
	volatile char *rxb_ptr = Rx_Buf;

	while (true)
	{
		while ((USART.STATUS & USART_RXCIF_bm) == 0){}
		char temp = USART.DATA;

		if (Rx_Count < Rx_Buf_Size)
		{
			*rxb_ptr = temp;
			rxb_ptr++;
			Rx_Count++;
		}
		else
		{
			lightRedLED();
			return Rx_Buf_Size - 1;
		}

		if (temp == '\n')
		{
			return Rx_Count;
		}
	}

}

/*
 * Send the completed string back to the TeraTerm program.
 */
static void echoCharactersBackToTeraTeram(volatile char *Tx_Buf, uint8_t Tx_Count)
{
	uint8_t sent = 0;
	volatile char *txb_ptr = Tx_Buf;

	while (sent < Tx_Count)
	{
		/* Wait for previous character to be sent */
		while ((USART.STATUS & USART_DREIF_bm) == 0){}
		/* Transmit current character */
		USART.DATA = *txb_ptr;

		txb_ptr++;
		sent++;
	}
}

int main(void)
{
	initUSART();
	volatile char Rx_Buf[TEST_CHARS];

	uint8_t received = receiveCharactersFromTeraTerm(Rx_Buf, TEST_CHARS);
	echoCharactersBackToTeraTeram(Rx_Buf, received);
	lightLEDSOnSuccess(false);
}
