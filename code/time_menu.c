#include <avr/io.h>
#include <stdio.h>	//printf functions
#include <stdint.h> //uint
#include <limits.h> // limiting values for simple types

#include "time_menu.h" //Time menu
#include "calendar.h" //Calendar functions
#include "menu.h" //Menu for LCD
#include "LCD/st7565.h" //LCD low level
#include "LCD/graphics.h" //LCD high level
#include "LCD/fonts/fonts.h" //fonts

const unsigned char MN14[]="Set Date";
const unsigned char MN15[]="Set Time";

const char *MONTHS[12]={
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
	}; 

void set_date(void)
{
	menu_time_years();
	menu_time_months();
	menu_time_days();
}

void menu_time_years(void)
{
	exit_callback = EXIT_NO;
	char buff_time_year[15];
	uint16_t time_years = time_data.year;
	uint16_t time_years_old = time_years;
	while(!(exit_callback))
	{
		keyread_set();
		show_callback();
		if (menu_event == E_RIGHT)
		{
			time_years+=add_value_key;
			if (time_years>2105) time_years = 2105;
		}
		if (menu_event == E_LEFT)
		{
			time_years-=add_value_key;
			if (time_years<1970) time_years = 1970;
		}
		draw_text((char*)MN14, 10, 0, Tahoma12, 1);
		sprintf(buff_time_year,"<-- %.2d %s %.4d -->", time_data.date+1, MONTHS[time_data.month], time_years);
		draw_text(buff_time_year,10,20,Tahoma7,1);
		glcd_refresh();
	}
	if (exit_callback == EXIT_YES_EX) //Abort without saving
	{
		time_data.year = time_years_old;
	}
	if (exit_callback == EXIT_YES_OK)
	{
		time_data.year = time_years;
	}
}

void menu_time_months(void)
{
	exit_callback = EXIT_NO;
	char buff_time_months[20];
	uint8_t time_months = time_data.month;
	uint8_t time_months_old = time_months;
	while(!(exit_callback))
	{
		keyread();
		show_callback();
		if (menu_event == E_RIGHT)
		{
			time_months++;
			if (time_months>11) time_months = 0;
		}
		if (menu_event == E_LEFT)
		{
			time_months--;
			if (time_months==255) time_months = 11; //After zero value there is 255
		}
		draw_text((char*)MN14, 10, 0, Tahoma12, 1);
		sprintf(buff_time_months,"<-- %.2d %s %.4d -->", time_data.date+1, MONTHS[time_months], time_data.year);
		draw_text(buff_time_months,10,20,Tahoma7,1);
		glcd_refresh();
	}
	if (exit_callback == EXIT_YES_EX) //Abort without saving
	{
		time_data.month = time_months_old;
	}
	if (exit_callback == EXIT_YES_OK)
	{
		time_data.month = time_months;
	}
}

void menu_time_days(void)
{
	exit_callback = EXIT_NO;
	char buff_time_days[20];
	uint8_t time_days_lap;
	uint8_t time_days_max;
	uint8_t time_days = time_data.date;
	uint8_t time_days_old = time_days;
	while(!(exit_callback))
	{
		keyread_set();
		show_callback();
		if (calendar_leapyear(time_data.year)) //Check if the actual year is lap
		{
			time_days_lap = 1;
		}else time_days_lap = 0;

		time_days_max = month[time_days_lap][time_data.month] - 1; //Copy the number of days

		if (menu_event == E_RIGHT)
		{
			time_days++;
			if (time_days>time_days_max) time_days = time_days_max;
		}
		if (menu_event == E_LEFT)
		{
			time_days--;
			if (time_days>254) time_days = 0;
		}
		draw_text((char*)MN14, 10, 0, Tahoma12, 1);
		sprintf(buff_time_days,"<-- %.2d %s %.4d -->", time_days+1, MONTHS[time_data.month], time_data.year);
		draw_text(buff_time_days,10,20,Tahoma7,1);
		glcd_refresh();
	}
	if (exit_callback == EXIT_YES_EX) //Abort without saving
	{
		time_data.date = time_days_old;
	}
	if (exit_callback == EXIT_YES_OK)
	{
		time_data.date = time_days;
	}
}

void set_time(void)
{
	exit_callback = EXIT_NO;
	char buff_time_hours[15];
	uint32_t time_set_seconds_tmp = (time_data.hour*3600UL) + (time_data.minute*60UL) + time_data.second;
	
	static uint8_t time_set_hours;
	static uint8_t time_set_minutes;
	static uint8_t time_set_seconds;
	
	while(!(exit_callback))
	{
		keyread_set();
		show_callback();
		
		if(time_set_seconds_tmp > (ULONG_MAX-MAX_VALUE_KEY)) time_set_seconds_tmp = 86399; // Boundaries for time
		if(time_set_seconds_tmp > 86399) time_set_seconds_tmp = 0;
				
		time_set_hours = (time_set_seconds_tmp/3600UL);
		time_set_minutes = (time_set_seconds_tmp/60UL)-(time_set_hours*60UL);
		time_set_seconds = time_set_seconds_tmp-(time_set_minutes*60UL)-(time_set_hours*3600UL);
		
		if (menu_event == E_RIGHT)
		{
			time_set_seconds_tmp+=add_value_key;
		}
		if (menu_event == E_LEFT)
		{
			time_set_seconds_tmp-=add_value_key;
		}
		
		draw_text((char*)MN15, 10, 0, Tahoma12, 1);
		sprintf(buff_time_hours,"<-- %.2d:%.2d:%.2d -->", time_set_hours, time_set_minutes, time_set_seconds);
		draw_text(buff_time_hours,10,20,Tahoma7,1);
		glcd_refresh();
	}
	if (exit_callback == EXIT_YES_EX) //Abort without saving
	
	{
		time_set_seconds_tmp=0;
		return;
	}
	if (exit_callback == EXIT_YES_OK)
	{
		time_data.second = time_set_seconds;
		time_data.minute = time_set_minutes;
		time_data.hour = time_set_hours;
	}
}