#include <avr/io.h>
#include <stdio.h>	// Sprintf functions
#include <string.h> // strlen, memmove
#include <math.h>
#include <util/atomic.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "battery_control.h" // Battery control ADC
#include "LCD/st7565.h" // LCD low level
#include "LCD/graphics.h" // LCD high level
#include "LCD/fonts/fonts.h" // Fonts
#include "SD.h" // High level SD
#include "measurements_menu.h" // Menu of the measurements

void init_ADC_battery_CH3(void)
{
	ADCA.CH3.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc; // Single-ended positive input signal for unsigned mode
	ADCA.CH3.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc; // Single ended at ADC PORTA2
}

void battery_sleep_pdown(void)
{
	glcd_blank();
	ATOMIC_BLOCK(ATOMIC_FORCEON) // Turn OFF LCD brightness
	{
		TCD0.CCA = 1UL;
		TCD0.INTCTRLA = TC_OVFINTLVL_OFF_gc;
	}
	f_close(&Fil); //close the SD card
	_delay_ms(100);
	SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_PDOWN_gc; // Power save mode
	SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep, else SLEEP-instruction is ineffective.
	sleep_cpu(); // Assembler code
}

void measure_ADC_battery_CH3(void)
{
	uint32_t ADC_accum_CH3 = 0;
	uint16_t ADC_res_CH3 = 0;
	
	for(uint8_t i=0;i<SAMPLE_NUMBER_BT;i++) // Oversampling
	{
		ADCA.CTRLA |= ADC_CH3START_bm;	  // Channel 3 start
		while(!(ADCA.INTFLAGS & ADC_CH3IF_bm)); // Wait until ready
		ADC_accum_CH3 +=ADCA.CH3RES;
		ADCA.CH3RES = 0;
	}
	
	ADC_res_CH3 = (ADC_accum_CH3/(SAMPLE_NUMBER_BT)); // Average
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		//ADC_voltage_CH3 = ADC_res_CH3;
		ADC_voltage_CH3 = ((ADC_res_CH3*V_REF)/(TOP+1))+dV;
		ADC_voltage_CH3 = ADC_voltage_CH3*(BT_R1+BT_R2)/BT_R2;
	}
}

void show_battery(void)
{
	char buff_battery[15];
	uint8_t buff_size = 0;
	
	sprintf(buff_battery,"%d", ADC_voltage_CH3); // Show the result on LCD
	buff_size = strlen(buff_battery);
	memmove(&buff_battery[buff_size-3], &buff_battery[buff_size-4],5); // Move strings to add ','
	buff_battery[buff_size-3] = '.'; // Add symbols
	buff_battery[buff_size] = 'V';
	
	draw_text(buff_battery,102,0,Tahoma7,1);
}