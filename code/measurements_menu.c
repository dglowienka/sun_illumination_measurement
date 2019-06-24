#include <avr/io.h>
#include <stdio.h>	// Printf functions
#include <stdint.h>
#include <util/delay.h>
#include <limits.h> // Limiting values for simple types
#include <avr/pgmspace.h>
#include <stddef.h>
#include <math.h>

#include "measurements_menu.h" // Menu of the measurements
#include "calendar.h" // Calendar functions
#include "menu.h" //Menu for LCD
#include "LCD/fonts/fonts.h" //fonts
#include "LCD/st7565.h" // LCD low level
#include "LCD/graphics.h" // LCD high level
#include "SD.h" // High level SD
#include "RTC.h" // RTC

const unsigned char MN19[]="Start Time";
const unsigned char MN20[]="Frequency";
const unsigned char MN21[]="Single Measurement";
const unsigned char MN22[]="Multi Measurements";
const unsigned char MN23[]="Duration";

uint32_t measure_time_seconds = 60; // If no value set then default 30 seconds of measurements
uint16_t measure_frequency_seconds = 1; // If no value set then 1s frequency as default

struct calendar_date time_start_measurement = {0, 0, 6, 0, 0, 1970}; // Start measurements at 6:00 as default

// Set time of the measurements
void start_measurements(void)
{
	exit_callback = EXIT_NO;
	char buff_start_measurements[20];
	uint32_t start_measure_tmp = (time_start_measurement.hour*3600UL) + (time_start_measurement.minute*60UL) + time_start_measurement.second;
	
	static uint8_t start_measure_hours;
	static uint8_t start_measure_minutes;
	static uint8_t start_measure_seconds;
	
	while(!(exit_callback))
	{
		keyread_set();
		show_callback();
		
		if(start_measure_tmp > (ULONG_MAX-MAX_VALUE_KEY)) start_measure_tmp = 86399; // Boundaries for time
		if(start_measure_tmp > 86399) start_measure_tmp = 0;
		
		start_measure_hours = (start_measure_tmp/3600UL);
		start_measure_minutes = (start_measure_tmp/60UL)-(start_measure_hours*60UL);
		start_measure_seconds = start_measure_tmp-(start_measure_minutes*60UL)-(start_measure_hours*3600UL);
		
		if (menu_event == E_RIGHT) // Increase seconds
		{
			start_measure_tmp+=add_value_key;
		}
		if (menu_event == E_LEFT) // Decrease seconds
		{
			start_measure_tmp-=add_value_key;
		}
		
		draw_text((char*)MN19, 10, 0, Tahoma12, 1);
		sprintf(buff_start_measurements,"<-- %.2d:%.2d:%.2d -->", start_measure_hours, start_measure_minutes, start_measure_seconds);
		draw_text(buff_start_measurements,10,20,Tahoma7,1);
		
		glcd_refresh();
	}
	if (exit_callback == EXIT_YES_EX) //Abort without saving
	{
		start_measure_tmp=0;
		return;
	}
	if (exit_callback == EXIT_YES_OK)
	{
		time_start_measurement.second = start_measure_seconds;
		time_start_measurement.minute = start_measure_minutes;
		time_start_measurement.hour = start_measure_hours;
	}
}

bool init_start_measurements(void)
{
	if ((time_data.hour == time_start_measurement.hour) && (time_data.minute == time_start_measurement.minute) && (time_data.second == time_start_measurement.second))
	{
		return true;
	}
	return false;
}

// Set time of the measurements
void duration_measurements(void)
{
	exit_callback = EXIT_NO;
	char buff_time_measurements[20];
	uint32_t measure_time_seconds_tmp = measure_time_seconds;
	
	static uint8_t measure_hours;
	static uint8_t measure_minutes;
	static uint8_t measure_seconds;
	
	while(!(exit_callback))
	{
		keyread_set();
		show_callback();
		
		if(measure_time_seconds_tmp > (ULONG_MAX-MAX_VALUE_KEY)) measure_time_seconds_tmp = 86400; // Boundaries for time
		if(measure_time_seconds_tmp > 86400) measure_time_seconds_tmp = 0;
		
		measure_hours = (measure_time_seconds_tmp/3600UL);
		measure_minutes = (measure_time_seconds_tmp/60UL)-(measure_hours*60UL);
		measure_seconds = measure_time_seconds_tmp-(measure_minutes*60UL)-(measure_hours*3600UL);
		
		if (menu_event == E_RIGHT) // Increase seconds
		{
			measure_time_seconds_tmp+=add_value_key;
		}
		if (menu_event == E_LEFT) // Decrease seconds
		{
			measure_time_seconds_tmp-=add_value_key;
		}
		
		draw_text((char*)MN23, 10, 0, Tahoma12, 1);
		sprintf(buff_time_measurements,"<-- %.2d:%.2d:%.2d -->", measure_hours, measure_minutes, measure_seconds);
		draw_text(buff_time_measurements,10,20,Tahoma7,1);
		
		glcd_refresh();
	}
	if (exit_callback == EXIT_YES_EX) //Abort without saving
	{
		measure_time_seconds_tmp=0;
		return;
	}
	if (exit_callback == EXIT_YES_OK)
	{
		measure_time_seconds = measure_time_seconds_tmp;
	}
}

// Set frequency of the measurements
void freq_measurements(void)
{
	exit_callback = EXIT_NO;
	char buff_frequency_measurements[15];
	uint16_t measure_frequency_tmp = measure_frequency_seconds;
	
	while(!(exit_callback))
	{
		keyread_set();
		show_callback();
		
		if (menu_event == E_RIGHT) // Increase seconds
		{
			measure_frequency_tmp+=add_value_key;
			if(measure_frequency_tmp > 3600) measure_frequency_tmp = 3600;
		}
		if (menu_event == E_LEFT) // Decrease seconds
		{
			measure_frequency_tmp-=add_value_key;
			if(measure_frequency_tmp > (USHRT_MAX-MAX_VALUE_KEY)) measure_frequency_tmp = 0; // Boundaries for frequency
		}
		
		draw_text((char*)MN20, 10, 0, Tahoma12, 1);
		sprintf(buff_frequency_measurements,"<-- Every %d s -->", measure_frequency_tmp);
		draw_text(buff_frequency_measurements,10,20,Tahoma7,1);
		glcd_refresh();
	}
	if (exit_callback == EXIT_YES_EX) //Abort without saving
	{
		measure_frequency_tmp=0;
		return;
	}
	if (exit_callback == EXIT_YES_OK)
	{
		measure_frequency_seconds = measure_frequency_tmp;
	}
}

// Signle measurements for two channels
void single_measurements(void)
{
	exit_callback = EXIT_NO;
	//char buff_adc[15];
	char buff_adc2[20];
	
	while(!(exit_callback))
	{
		keyread();
		show_callback();
		show_battery();
		draw_text("Irradiance",10,0,Tahoma12,1);

		//sprintf(buff_adc, "Current at CH1: %d [mV]",measure_ADC_CH1());
		//draw_text(buff_adc,10,20,Tahoma7,1);
		//_delay_ms(100);
		sprintf(buff_adc2, "%d [Wm^-2] %d", irradiation_CH2, measure_ADC_CH2());
		draw_text(buff_adc2,10,30,Tahoma12,1);
		glcd_refresh();
	}
}

// Save mode on SD card, give time and frequency
void multi_measurements(void)
{
	exit_callback = EXIT_NO;
	
	char Buff[128];		// Working buffer
	char buff_name[13]; // Buffer for name storage
	uint8_t buff_size;
	uint8_t measure_no_day = 0; // Reset number of measures in day
	
	do{ // Create new files if one already exists
		measure_no_day++;
		sprintf(buff_name,"%.2d%.2d%.2d%.2d.txt", measure_no_day, time_data.date+1, time_data.month+1, (time_data.year)%100);
	}while ((f_open(&Fil, (TCHAR *)buff_name, FA_WRITE | FA_CREATE_NEW) == FR_EXIST));
	
	uint32_t count_down_seconds_measure = seconds + measure_time_seconds; // Time to count down
	uint32_t count_down_seconds_frequency_tmp = seconds + measure_frequency_seconds; // For the frequency of the measurements
	
	while(!(exit_callback))
	{
		keyread();
		show_callback();
		show_battery();
		
		draw_text("SD recording",10,0,Tahoma12,1); // Main text
		draw_text("Time, Irriadiance",10,20,Tahoma7,1); // Values written
		
		if (count_down_seconds_frequency_tmp == seconds) // Occurs only once per given frequency
		{
			count_down_seconds_frequency_tmp = seconds + measure_frequency_seconds; // New value of seconds
			buff_size = sprintf(Buff,"%.2d:%.2d:%.2d, %d [Wm^-2], %d",time_data.hour, time_data.minute, time_data.second, irradiation_CH2 ,measure_ADC_CH2()); // String written on the SD
			
			f_write(&Fil, Buff, buff_size, &bw); // Write the data to SD
			f_write(&Fil, "\r\n", sizeof("\r\n"), &bw); // Next line
		}
		
		draw_text(Buff,10,30,Tahoma7,1); // Show data written
		draw_text(buff_name,10,50,Tahoma7,1); // Show name of the current file
		
		glcd_refresh();
		if (seconds == count_down_seconds_measure)
		{
			break;
		}
	}
	f_close(&Fil); //close the SD card
	return;
}

// Initialize ADC at PORTA
void init_ADCA(void)
{
	ADCA.CAL = pgm_read_word(PROD_SIGNATURES_START+offsetof(NVM_PROD_SIGNATURES_t,ADCACAL0)); //Calibrate
	
	ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc; // 12bits resolution
	
	ADCA.REFCTRL = ADC_REFSEL_VCC_gc; // Reference source VCC/1.6
	ADCA.CTRLA = ADC_ENABLE_bm; // Enable ADC
	init_ADC_CH1();
	init_ADC_CH2();
}

// Initialize ADC at CH1
void init_ADC_CH1(void)//single run mode with unsigned
{
	ADCA.CH1.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc; //Single-ended positive input signal for unsigned mode
	ADCA.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN3_gc; // Single ended at ADC PORTA1
}

// Take single measure at CH1
uint16_t measure_ADC_CH1(void)
{
	uint32_t ADC_accum_CH1 = 0;
	uint16_t ADC_res_CH1 = 0;
	//uint16_t ADC_mvoltage_CH1;
	//uint16_t ADC_mamperes_CH1;
	
	for(uint16_t i=0;i<SAMPLE_NUMBER;i++) // Oversampling
	{
		ADCA.CTRLA |= ADC_CH1START_bm;	  // Channel 1 start
		while(!(ADCA.INTFLAGS & ADC_CH1IF_bm));
		ADC_accum_CH1 += ADCA.CH1RES;
		ADCA.CH1RES = 0;
	}
	
	ADC_res_CH1 = (ADC_accum_CH1/SAMPLE_NUMBER);
	//ADC_mvoltage_CH1=((ADC_res_CH1*V_REF)/(TOP+1));
	//ADC_mamperes_CH1=(ADC_mvoltage_CH1*10UL/(57UL));
	
	return ADC_res_CH1;
}

// Initialize ADC at CH2
void init_ADC_CH2(void)
{
	ADCA.CH2.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc; //Single-ended positive input signal for unsigned mode
	ADCA.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc; // Single ended at ADC PORTA2
}

// Take single measure at CH2
uint16_t measure_ADC_CH2(void)
{
	uint32_t ADC_accum_CH2 = 0;
	uint16_t ADC_res_CH2 = 0;
	irradiation_CH2 = 0;
	
	TCD1.INTCTRLA = TC_OVFINTLVL_OFF_gc; // Turn off battery measurements
	
	for(uint16_t i=0;i<SAMPLE_NUMBER;i++) // Oversampling
	{
		ADCA.CTRLA |= ADC_CH2START_bm;	  // Channel 2 start
		while(!(ADCA.INTFLAGS & ADC_CH2IF_bm));
		ADC_accum_CH2 += ADCA.CH2RES;
		ADCA.CH2RES = 0;
	}

	ADC_res_CH2 = (ADC_accum_CH2/SAMPLE_NUMBER);
	irradiation_CH2 = 9.24951082847867*ADC_res_CH2 - 1874.40282163027;
	if (irradiation_CH2 > 2000) irradiation_CH2 = 0;
	
	TCD1.INTCTRLA = TC_OVFINTLVL_LO_gc; // Turn on battery measurements
	
	return ADC_res_CH2;	
}