#include <avr/interrupt.h>
#include <stdio.h>

#include "RTC.h"
#include "LCD/st7565.h"
#include "LCD/graphics.h"
#include "LCD/fonts/fonts.h"
#include "LCD/st7565-config.h"
#include "calendar.h"
#include "measurements_menu.h" // Menu of the measurements

void timer_RTC(void)
{
	OSC.XOSCCTRL |= OSC_XOSCSEL_32KHz_gc | // 32.768kHz crystal oscillator at TOSC
	OSC_X32KLPM_bm; // The low power mode
	OSC.CTRL |= OSC_XOSCEN_bm; // Enable external crystal
	
	CLK.RTCCTRL = CLK_RTCSRC_TOSC32_gc | //external 32.768 KHz crystal no division
				  CLK_RTCEN_bm;  // Enable oscillator for RTC
	
	RTC.PER = 32768; // One second is 32768 cycles
	
	while((RTC.STATUS & RTC_SYNCBUSY_bm));
	
	RTC.INTCTRL = RTC_OVFINTLVL_HI_gc;   // Low level for overflow interrupt
	
	RTC.CTRL = RTC_PRESCALER_DIV1_gc; // No prescaling and start
}

void show_RTC(void)
{
	RTC_time();
	draw_text(time_RTC, 5, 0, Tahoma7, 1);
	RTC_date();
	draw_text(date_RTC, 49, 0, Tahoma7, 1);
}

void RTC_time(void)
{
	sprintf(time_RTC, "%.2d:%.2d:%.2d",time_data.hour, time_data.minute, time_data.second);
}
void RTC_date(void)
{
	sprintf(date_RTC, "%.2d/%.2d/%.4d", time_data.date+1, time_data.month+1, time_data.year); // Correction of first day month and date 0-30 to 1-31
}

ISR(RTC_OVF_vect)
{
	seconds = calendar_date_to_timestamp(&time_data);  // Seconds for further use
	seconds++;
	calendar_timestamp_to_date(seconds,&time_data);
	
	if (init_start_measurements()) // To start measurements set at given time
	{
		start_measurements_global = true;
	}else start_measurements_global = false;
}
