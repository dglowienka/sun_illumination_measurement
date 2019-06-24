#include <avr/io.h>
#include <stdio.h>	//printf functions

#include "contrast.h" //Contrast menu
#include "menu.h" //Menu for LCD
#include "LCD/st7565.h" //LCD low level
#include "LCD/fonts/fonts.h" //fonts
#include "LCD/graphics.h" //LCD high level

void menu_contrast(void)
{
	exit_callback = EXIT_NO;
	char buff_contrast[5];
	uint8_t contrast_old = contrast;
	while(!(exit_callback))
	{
		keyread();
		show_callback();
		
		if (menu_event == E_LEFT)
		{
			contrast--;
			if(contrast<1)
			{
				contrast=0;
			}
		}
		if (menu_event == E_RIGHT)
		{
			contrast++;
			if (contrast>63)
			{
				contrast=63;
			}
		}
		glcd_contrast(6, contrast);
		
		draw_text((char*)MN04,10,0,Tahoma12,1);
		sprintf(buff_contrast, "<-- %d -->",contrast);
		draw_text(buff_contrast,10,20,Tahoma7,1);
		glcd_refresh();
	}
	if (exit_callback == EXIT_YES_EX) // Abort without saving
	{
		contrast = contrast_old;
		glcd_contrast(6, contrast);
	}else
	{ // Save value
		glcd_contrast(6, contrast);
	}
}