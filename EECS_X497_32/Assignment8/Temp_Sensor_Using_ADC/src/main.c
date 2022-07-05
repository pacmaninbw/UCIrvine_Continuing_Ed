/*
 * EECS 497.32 Week 9 Programming assignment #8
 *
 * Student:		Paul Chernick
 *
 * Extend the week 7 programming assignment (assignment #6) so that the 
 * desired temperature is stored in the Non volatile ram so that the
 * desired temperature is available when the device is powered up, or
 * after reset. Note that I chose to use my assignment #6 as the basis for
 * the project rather than the template supplied.
 *
 *	06/17/2022
 *		- renamed update_desired_temperature() to 
 *			update_and_save_desired_temperature() to represent the change
 *			in functionality. The function now saves the updated desired
 *			temperature to the EEPROM.
 *
 *	06/18/2022
 *		- Added getStoredDesiredTemperatureFromNVRam() to retrieve the desired
 *			temperature
 *		- Added storeDesiredTemperatureInNVRam() to stire the desired temperature.
 *
 *	06/20/2022
 *		- Fixed bugs on first run of program,
 *			- If current temperature is -1% F the desired won't be set correctly and
 *				will not be written to the EEPROM for future use.
 *		- added getEEPROMHasNotBeenWritten() and FirstWriteEEPROM().
 *
 * EECS 497.32 Week 7 Programming assignment #6
 *
 * Student:		Paul Chernick
 *
 * Add the following logic to the existing code.	
 *	Use LED_off() and LED_on() functions to control LED0 and LED1
 *	Use gpio_pin_is_low() function to check GPIO_PUSH_BUTTON_1 and GPIO_PUSH_BUTTON_2 state
 *
 *	1. Set Desired temperature using switches switch1 to increase Desired temperature,
 *	   and switch2 to decrease Desired temperature	 
 *	2. If current temperature is equal to desired temperature, both LED0 and LED1 are off
 *	3. If current temperature is above desired temperature, turn LED0 on, LED1 off
 *	4. If current temperature is below desired temperature, turn LED0 off, LED1 on
 */

/*
 * Since we are using library routines for everything, my header files are
 * not included, they aren't necessary, but they are in the source tree. They
 * were originally included, but I didn't need them.
 */
#include <asf.h>
#include "stdio.h"
#include "adc_sensors.h"
#include "ntc_sensor.h"
#include "sysfont.h"

//! The X position of the temperature image on screen
#define X_POS 10

/*
 * Temperature Conversion Constants
 */
#define TEMP_STR_SIZE					15
#define FAHRENHEIT_FREEZING_POINT		32
#define CELCIUS_TO_FAHRENHEIT_DEGREES	1.8

/*
 * Button values for communications between functions. Could use enums here.
 */
#define BUTTON_ONE					(uint8_t) 0x01
#define BUTTON_TWO					(uint8_t) 0x02
#define BOTH_BUTTONS				(uint8_t) 0x03

//! Flash text for the heading
PROGMEM_DECLARE(char const, header[]) = "Temperature(F)";

/*
 * ! The thermometer image
 */
uint8_t tempscale_data[] = {
	0x01, 0xf9, 0xfd, 0xfd, 0xf9, 0x01,
	0x41, 0xff, 0xff, 0xff, 0xff, 0x41,
	0x10, 0xff, 0xff, 0xff, 0xff, 0x10,
	0x9e, 0xbf, 0xbf, 0xbf, 0xbf, 0x9e
};

/*
 * Initialization functions.
 */
/*
 * Initialize the temperature graphic with the thermometer bitmap.
 */
static void init_tempscale_struct(struct gfx_mono_bitmap *tempscale)
{
	/*
	 * Struct for holding the temperature scale background
	 */
	tempscale->type = GFX_MONO_BITMAP_RAM;
	tempscale->width = 6;
	tempscale->height = 32;
	tempscale->data.pixmap = tempscale_data;
}

/*
 * Initialize the LCD display with the header and the thermometer bitmap. 
 */
static void init_lcd_temp_display(struct gfx_mono_bitmap *tempscale)
{
	// Enable LCD display back light
	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	cpu_irq_enable();

	init_tempscale_struct(tempscale);

	// Screen border
	gfx_mono_draw_rect(0, 0, 128, 32, GFX_PIXEL_SET);
	// Clear screen
	gfx_mono_draw_filled_rect(1, 1, 126, 30, GFX_PIXEL_CLR);

	// Draw the header
	gfx_mono_draw_progmem_string((char PROGMEM_PTR_T)header, 27, 2, &sysfont);
	// Draw the thermometer
	gfx_mono_put_bitmap(tempscale, X_POS, 0);
}

/*
 * Initialize all parts of the device and the microprocessor.
 */
static void init_all_required_subsystems(struct gfx_mono_bitmap *tempscale)
{
	sysclk_init();
	board_init();
	pmic_init();
	gfx_mono_init();
	adc_sensors_init();
	init_lcd_temp_display(tempscale);
}

/*
 * LCD Display functions.
 */
/*
 * Displays the current temperature and the desired temperature on the LCD screen.
 */
static void display_temps_lcd(int16_t current_temperature, int16_t desired_temperature,
	uint8_t temp_scale)
{
	/* String to hold the converted temperature reading */
	char temperature_string[TEMP_STR_SIZE];

	// Draw the scale element on top of the background temperature image
	gfx_mono_draw_filled_rect(X_POS + 2, 3, 2, temp_scale, GFX_PIXEL_CLR);
		
	snprintf(temperature_string, sizeof(temperature_string), "%3i Current",
		current_temperature);

	// Draw the Fahrenheit temperature string
	gfx_mono_draw_string(temperature_string, X_POS + 12, 21, &sysfont);
		
	snprintf(temperature_string, sizeof(temperature_string), "%3i Desired",
		desired_temperature);

	// Draw the Desired string
	gfx_mono_draw_string(temperature_string, X_POS + 12, 13, &sysfont);
}

/*
 * Conversion functions.
 */
/*
 * Convert the temperature into the thermometer scale. I have tried to make
 * this function inline, but based on the assembly code generated by the
 * floating point arithmetic, I don't think the inline status is actually
 * true.
 */
static inline uint8_t get_thermometer_scale(int16_t current_temperature)
{
	uint8_t temp_scale = -0.36 * current_temperature + 20.25;
	if (temp_scale <= 0) {
		temp_scale = 0;
	}

	return temp_scale;
}

/*
 * Convert the temperature to Fahrenheit.  I have tried to make
 * this function inline, but based on the assembly code generated by the
 * floating point arithmetic, I don't think the inline status is actually
 * true.
 */
static inline int16_t convert_celcius_to_fahrenheit(int16_t current_temperature)
{
	return current_temperature * CELCIUS_TO_FAHRENHEIT_DEGREES +
		FAHRENHEIT_FREEZING_POINT;
}

/*
 * Functions to manipulate the desired temperature.
 */

/*
 * Get the button input if the user wants to alter the desired temperature.
 * Pressing both SW1 and SW2 is an error in this case. If both buttons are
 * pushed light the red LED to indicate an error. Note the error state goes
 * beyond the project specification.
 */
static uint8_t get_button_input(void)
{
	uint8_t button_pushed = BOTH_BUTTONS;

	while (button_pushed == BOTH_BUTTONS)
	{
		button_pushed = gpio_pin_is_low(GPIO_PUSH_BUTTON_1)? BUTTON_ONE : 0;
		button_pushed |= gpio_pin_is_low(GPIO_PUSH_BUTTON_2)? BUTTON_TWO : 0;
		if (button_pushed == BOTH_BUTTONS)
		{
			LED_On(LED2);
		}
	}

	LED_Off(LED2);

	return button_pushed;
}

#define MOST_SIGNIFICANT_BYTE_ADDRESS	0x0
#define LEAST_SIGNIFICANT_BYTE_ADDRESS	0x1
#define EEPROM_NOT_USED					0x3
/*
 * Retrieve the desired temperature from the EEPROM. The retrieval will
 * involved 2 reads from the EEPROM since the desired temperature is a
 * 16 bit value and the EEPROM can only read and write 1 byte at a time.
 * Retrieve the most significant byte first, shift the value up and then
 * retrieve the least significant byte.
 */
static uint16_t getStoredDesiredTemperatureFromNVRam(void)
{
	uint16_t desired_temperature = 0;
	
	uint8_t msByte = nvm_eeprom_read_byte(MOST_SIGNIFICANT_BYTE_ADDRESS);
	uint8_t lsByte = nvm_eeprom_read_byte(LEAST_SIGNIFICANT_BYTE_ADDRESS);
	
	desired_temperature = (msByte << 8) + lsByte;
	
	return desired_temperature;
}

/*
 * Store the desired temperature value in the EEPROM. This is a two step
 * process since we are storing a 16 bit value in two 8 bit locations in
 * the EEPROM. To make reading the values easier we will store the most
 * significant byte first and then store the least significant byte second.
 */
static void storeDesiredTemperatureInNVRam(uint16_t desired_temperature)
{
	uint8_t msByte = desired_temperature >> 8;
	uint8_t lsByte = desired_temperature & 0xFF;
	
	nvm_eeprom_write_byte(MOST_SIGNIFICANT_BYTE_ADDRESS, msByte);
	nvm_eeprom_write_byte(LEAST_SIGNIFICANT_BYTE_ADDRESS, lsByte);
}

/*
 * Read the address used to indicate if the EEPROM has been written or not.
 */
static uint8_t getEEPROMHasNotBeenWritten(void)
{
	return nvm_eeprom_read_byte(EEPROM_NOT_USED);
}

/*
 * This function will initialize the EEPROM with the first value for the desired
 * temperature. It will return without writing the EEPROM if the EEPROM has been
 * written to previously.
 *
 * Assumptions:
 *	-	If the EEPROM hasn't been written to before, the value of each byte is -1
 *		(all bits are high).
 */
static void FirstWriteEEPROM(uint16_t current_temperature)
{
	if (nvm_eeprom_read_byte(EEPROM_NOT_USED))
	{
		nvm_eeprom_write_byte(EEPROM_NOT_USED, 0);
		storeDesiredTemperatureInNVRam(current_temperature);
	}
}

/*
 * Updates the desired temperature based on the value of the button pushed.
 * Set Desired temperature using switches switch1 to increase Desired temperature,
 * and switch2 to decrease Desired temperature.
 * Saves the desired temperature in the EEPROM.
 * 06/17/2022 renamed due to updated functionality.
 */
static void update_and_save_desired_temperature(uint8_t button, int16_t *desired_temperature)
{
	if (button == 0) /* no button was pushed */
	{
		return;
	}
	else if (button == BUTTON_ONE)
	{
		(*desired_temperature)++;
	}
	else /* BUTTON_TWO */
	{
		(*desired_temperature)--;
	}
	
	storeDesiredTemperatureInNVRam(*desired_temperature);
}

/*
 * Keep a history of the LEDs, so that they aren't flashing.
 * If current temperature is equal to desired temperature, both LED0 and LED1 are off
 * If current temperature is above desired temperature, turn LED0 on, LED1 off
 * If current temperature is below desired temperature, turn LED0 off, LED1 on
 */
static int16_t old_current_temperature = 0;
static int16_t old_desired_temperature = 0;
static void light_led_if_different(int16_t current_temperature, int16_t desired_temperature)
{
	/*
	 * If no change from last execution, nothing needs to change in the hardware.
	 */
	if (current_temperature == old_current_temperature && desired_temperature == old_desired_temperature)
	{
		return;
	}
	
	/*
	 * If either of the temperatures changed since the last execution of this
	 * function then we need to turn off at least one LED. To decrease the
	 * complexity of the code, just turn off all lit LEDs.
	 */
	LED_Off(LED0);
	LED_Off(LED1);

	if (current_temperature > desired_temperature)
	{
		LED_On(LED0);
	}

	if (current_temperature < desired_temperature)
	{
		LED_On(LED1);
	}

	old_current_temperature = current_temperature;
	old_desired_temperature = desired_temperature;
}

/*
 * Rather than having an overly complex main, all code has been moved to functions,
 * except for the core loop. This was done to make the code more readable and
 * maintainable.
 */
int main (void)
{
	/* Bitmap to hold the actual thermometer image */
	struct gfx_mono_bitmap tempscale;

    // Initialize board here
    init_all_required_subsystems(&tempscale);

   int16_t current_temperature;
   /* Updated 06/17/2022 - Get stored desired temperature. */
   int16_t desired_temperature = getStoredDesiredTemperatureFromNVRam();
   int8_t eeprom_not_used = getEEPROMHasNotBeenWritten();	/* renamed 6/20/2022 */
   
	while(true)
	{
		current_temperature = read_temperature();	
	
		uint8_t temp_scale = get_thermometer_scale(current_temperature);

		current_temperature = convert_celcius_to_fahrenheit(current_temperature);

		if (eeprom_not_used)
		{
			/*
			 * Added 06/20/2022.
			 * Store the desired temperature the first time the program ever runs.
			 * Assumption - eeprom_not_used will be all high or -1 the first time
			 *		the program runs.
			 */
			FirstWriteEEPROM(current_temperature);
			eeprom_not_used = getEEPROMHasNotBeenWritten();
			desired_temperature = getStoredDesiredTemperatureFromNVRam();
		}

		display_temps_lcd(current_temperature, desired_temperature, temp_scale);

		uint8_t user_input = get_button_input();
		update_and_save_desired_temperature(user_input, &desired_temperature);

		light_led_if_different(current_temperature, desired_temperature);
	}
}

