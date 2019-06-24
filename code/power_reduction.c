#include <avr/sleep.h>
#include <util/atomic.h> //atomic block
#include <util/delay.h> //for delays

#include "power_reduction.h" // Power reduction
#include "LCD/graphics.h" // LCD high level
#include "LCD/fonts/fonts.h" // Fonts and logo
#include "menu.h" // Menu for LCD
#include "LCD/st7565.h" // LCD low level
#include "LCD/st7565-config.h" // LCD config
#include "logo.h" // Logo
#include "RTC.h" // RTC
#include "measurements_menu.h" // Menu of the measurements

// Turn on sleep mode - power save, only RTC works
void enable_sleep_psave(void)
{
	SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_PSAVE_gc; // Power save mode
	SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep, else SLEEP-instruction is ineffective.
	sleep_cpu(); // Assembler code
	SLEEP.CTRL &= ~SLEEP_SEN_bm; // Disable sleep mode just afer it wake up
}

void disable_sleep_psave(void)
{
	TCD0.INTCTRLA = TC_OVFINTLVL_MED_gc; // Turn ON LCD brightness
}

// Show logo and go sleep function
void exit_menu (void)
{
	load_logo(); // Load University logo
	
	ATOMIC_BLOCK(ATOMIC_FORCEON) // Turn OFF LCD brightness
	{
		TCD0.CCA = 1UL; 
		TCD0.INTCTRLA = TC_OVFINTLVL_OFF_gc; 
	}
	_delay_ms(100);

	while(1)
	{
		enable_sleep_psave();
		keyread();
		if (start_measurements_global) // Measurements are started
		{
			disable_sleep_psave();
			multi_measurements();
			break;
		}
		if (menu_event == E_EXIT) break; // Button pressed
	}
	menu_event = E_IDDLE;
	disable_sleep_psave();
}

uint8_t count_down_seconds;
uint32_t count_down_seconds_tmp;

// Power down if there is no movement for 30 seconds
void count_down(void)
{
	if (count_down_seconds_tmp != seconds) //Each second occurs
	{
		count_down_seconds++;
	}
	count_down_seconds_tmp = seconds; // Copy new value
	if (menu_event || button_call)//If there is movement
	{
		count_down_seconds = 0; //Reset the counting down
	}
	if (count_down_seconds == SLEEP_TIME) //For 30 seconds no movement
	{
		exit_menu(); //Then set sleep and show logo
		count_down_seconds = 0;
	}
}

// Minimize power consumption
void init_power_reduction(void)
{
	// Power reduction: Stop unused peripherals
	PR.PRGEN = PR_USB_bm | //stop the clock to the USB module
			   PR_AES_bm | //stop the clock to the AES module
			   PR_EBI_bm | //stop the clock to the external bus interface
			   PR_EVSYS_bm | //stop the clock to the event system
			   PR_DMA_bm; //stop the clock to the DMA controller
	PR.PRPA = PR_DAC_bm | //stop the clock to the DAC
		      //PR_ADC_bm | //stop the clock to the ADC
			  PR_AC_bm; //stop the clock to the analog comparator
	PR.PRPB = PR_DAC_bm | //stop the clock to the DAC
			  PR_ADC_bm | //stop the clock to the ADC
			  PR_AC_bm; //stop the clock to the analog comparator
	PR.PRPC = PR_TWI_bm | //stop the clock to the two-wire interface
			  PR_USART1_bm | //stop the clock to USART1
			  PR_USART0_bm | //stop the clock to USART0
			  PR_SPI_bm | //stop the clock to the SPI
			  PR_HIRES_bm | //stops the clock to the high-resolution extension
			  //PR_TC1_bm| //stop the clock to timer/counter 1
			  PR_TC0_bm; //stops the clock to timer/counter 0
	PR.PRPD = PR_TWI_bm | //stop the clock to the two-wire interface
			PR_USART1_bm | //stop the clock to USART1
			PR_USART0_bm | //stop the clock to USART0
			PR_SPI_bm | //stop the clock to the SPI
			PR_HIRES_bm; //stops the clock to the high-resolution extension
			//PR_TC1_bm;//stop the clock to timer/counter 1
			//PR_TC0_bm; //stops the clock to timer/counter 0
	PR.PRPE = PR_TWI_bm | //stop the clock to the two-wire interface
			  PR_USART1_bm | //stop the clock to USART1
			  PR_USART0_bm | //stop the clock to USART0
			  PR_SPI_bm | //stop the clock to the SPI
			  PR_HIRES_bm | //stops the clock to the high-resolution extension
			  PR_TC1_bm | //stop the clock to timer/counter 1
			  PR_TC0_bm; //stops the clock to timer/counter 0
	PR.PRPF = PR_TWI_bm | //stop the clock to the two-wire interface
			  PR_USART1_bm | //stop the clock to USART1
			  PR_USART0_bm | //stop the clock to USART0
			  PR_SPI_bm | //stop the clock to the SPI
			  PR_HIRES_bm | //stops the clock to the high-resolution extension
			  PR_TC1_bm | //stop the clock to timer/counter 1
			  PR_TC0_bm; //stops the clock to timer/counter 0	  			  			  		  
}