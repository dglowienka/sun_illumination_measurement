#include <avr/io.h>
#include <util/delay.h>

#include "clock.h"

void clock_init(void) //set 16MHz
{
	OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc |
	OSC_XOSCSEL_XTAL_16KCLK_gc; //external clock with 6 cycles
	OSC.CTRL = OSC_XOSCEN_bm; //enable external crystal
	
	for (uint8_t i=0;i<255;i++)
	{
		if ((OSC.STATUS & OSC_XOSCRDY_bm))
		{
			CPU_CCP = CCP_IOREG_gc; // through change protection mechanism
			CLK.CTRL |= CLK_SCLKSEL_XOSC_gc; // Selects clock system as external clock
			
			CPU_CCP = CCP_IOREG_gc; //mechanism protection enable
			OSC.XOSCFAIL |= OSC_XOSCFDEN_bm; //failure detection
			break;
		}
		_delay_us(10);
	}
	while((OSC.STATUS & OSC_XOSCRDY_bm)==0) {}; // If there is no oscillator
}