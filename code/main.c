#include <avr/io.h> //I/O pins
#include <util/atomic.h> //atomic block
#include <util/delay.h> //for delays
#include <stdio.h>	//printf functions
#include <stdint.h> // uint

#include "main.h"
#include "clock.h"  // External crystal
#include "SD/ff.h"		// Declarations of FatFs API
#include "SD/diskio.h" // High level SD control
#include "SD.h" //High level SD
#include "LCD/st7565.h" //LCD low level
#include "LCD/graphics.h" //LCD high level
#include "menu.h" //menu for LCD
#include "LCD/fonts/fonts.h" //fonts
#include "RTC.h" //RTC
#include "calendar.h" //real time callendar
#include "power_reduction.h" //power reduction
#include "measurements_menu.h" // Menu of the measurements
#include "brightness.h" // Brightness option
#include "battery_control.h" // Battery control ADC
#include "time_menu.h" //Time menu

void timer_global(void)
{
	TCD1.CTRLB = TC_WGMODE_NORMAL_gc; // Normal mode
	TCD1.INTCTRLA = TC_OVFINTLVL_LO_gc; // Overflow generates low level interrupt
	TCD1.CTRLA = TC_CLKSEL_DIV256_gc; // // 256 prescaler and start the timer
}

int main(void)
{
	//clock init
	clock_init();
	
	//power reduction
	init_power_reduction();	
	
	//LCD
	glcd_init();
	glcd_blank();
	glcd_contrast(6, 17); // Resistor ratio and contrast
	timer_pwm();
	
	//ADC
	init_ADCA();
	PORTA.DIRCLR = PIN1_bm; // Input
	
	//RTC
	timer_RTC();
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		seconds=calendar_date_to_timestamp(&time_data);
	}
	
	//BATTERY CONTROL
	init_ADC_battery_CH3();
	measure_ADC_battery_CH3();
	
	//SD
	init_SD();
	
	//Menu
	menu_init();

	//Timer global
	timer_global();
	
	//Interrupts enable
	PMIC.CTRL = PMIC_HILVLEN_bm | // Enable interrupts of high, middle and low level
				PMIC_MEDLVLEN_bm |
				PMIC_LOLVLEN_bm;
	sei();
	
	//Set time
	set_date();
	set_time();
	
	while (1)
	{
		keyread();
		show_menu();
		show_RTC();
		count_down();
		show_battery();
		glcd_refresh();
		if (start_measurements_global) multi_measurements();
	}
}

ISR(TCD1_OVF_vect)
{
	measure_ADC_battery_CH3();
	//if (ADC_voltage_CH3 < 3400) battery_sleep_pdown(); // Critical voltage
}