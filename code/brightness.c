#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdio.h>	//printf functions
#include <stdint.h> //uint
#include <limits.h> // limiting values for simple types

#include "brightness.h" // Brightness option
#include "menu.h" //Menu for LCD
#include "LCD/st7565.h" //LCD low level
#include "LCD/fonts/fonts.h" //fonts
#include "LCD/graphics.h" //LCD high level

void menu_brightness(void)
{
	exit_callback = EXIT_NO;
	brightness_old = brightness;
	uint16_t brightness_max = TCD0.PER;
	uint32_t brightness_pc = ((brightness*100UL)/brightness_max); // Percentages of CCA
	char buff_brightness[10];
	while(!(exit_callback))
	{
		keyread_set();
		show_callback();
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			brightness = (brightness_pc*brightness_max)/100UL;
			if(brightness < 1) brightness = 1; // Bottom boundaries for brightness
		}
		
		if (menu_event == E_RIGHT)
		{
			brightness_pc+=add_value_key;
			if(brightness_pc > 100) brightness_pc = 100; //Top boundaries for brightness
		}

		if (menu_event == E_LEFT)
		{
			brightness_pc-=add_value_key;
			if(brightness_pc > (ULONG_MAX-MAX_VALUE_KEY)) brightness_pc = 1; // Bottom boundaries for brightness
		}
		draw_text((char*)MN05, 10, 0, Tahoma12, 1);
		sprintf(buff_brightness,"<-- %lu%% -->", brightness_pc);
		draw_text(buff_brightness,10,20,Tahoma7,1);
		glcd_refresh();
	}

	if (exit_callback == EXIT_YES_EX) // Abort without saving
	{
		brightness = brightness_old;
	}
}

void timer_pwm(void)
{
	//OCOA at TCD0 is PORTD.0
	PORTD.DIRSET |= PIN0_bm; //PWM for LED control
	
	TCD0.CTRLB = TC_WGMODE_SINGLESLOPE_gc | //single slope PWM
				 TC0_CCAEN_bm; // control PORTD.0 by PWM
	TCD0.INTCTRLA = TC_OVFINTLVL_MED_gc; // Low level interrupt enable at overflow of timer
	
	TCD0.PER = 2500UL;//PER = 16000000/(Frequency*N) - 1, N=64, PER=4165, F = 100 Hz
	TCD0.CCA = brightness; //50% PWM filling
	TCD0.CTRLA = TC_CLKSEL_DIV64_gc; //64 prescaler and start pwm
}

ISR(TCD0_OVF_vect)
{
	TCD0.CCA = brightness; // Set brightness of LCD
}
