#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>	/*printf functions */
#include <stdint.h> // uint

#include "SD.h" //High level SD
#include "measurements_menu.h" // Menu of the measurements
#include "LCD/fonts/fonts.h" //fonts
#include "LCD/st7565.h" //LCD low level
#include "LCD/graphics.h" //LCD high level
#include "RTC.h" //RTC and interrupts for LCD
#include "menu.h" //menu for LCD
#include "calendar.h"

// Initialize SD card and show result on LCD
void init_SD(void)
{	
	disk_initialize(0);
	mount = f_mount(&FatFs, "", 1);
	switch(mount)
	{
		case FR_DISK_ERR:
			while (!(mount==FR_OK))
			{
				draw_text("FR_DISK_ERR",10,20,Tahoma7,1);
				glcd_refresh();
			}
			break;
		case FR_OK:
				draw_text("FR_OK",10,20,Tahoma7,1);
				glcd_refresh();
				_delay_ms(1000);
			break;
		case FR_NO_FILESYSTEM:
			while (!(mount==FR_OK))
			{
				draw_text("FR_NO_FILESYSTEM",10,20,Tahoma7,1);
				glcd_refresh();
			}
			break;
		case FR_NOT_READY:
			while(!(mount==FR_OK))
			{
				draw_text("FR_NO_READY",10,20,Tahoma7,1);
				draw_text("Mount?",10,30,Tahoma7,1);
				glcd_refresh();
				if (OK_KEY)
				{
					mount = f_mount(&FatFs, "", 1);
				}
			}
			break;
		default:
			while (!(mount==FR_OK))
			{
				draw_text("Other ERROR",10,20,Tahoma7,1);
				glcd_refresh();
			}		
			break;
	}
}