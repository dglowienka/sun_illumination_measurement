#include <util/delay.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stdio.h>	//printf functions
#include <avr/interrupt.h>
#include <stdint.h> //uint

#include "menu.h" //Menu for LCD
#include "LCD/st7565.h" //LCD low level
#include "LCD/graphics.h" //LCD high level
#include "LCD/fonts/fonts.h" //fonts
#include "LCD/st7565-config.h" //LCD config
#include "measurements_menu.h" // Menu of the measurements
#include "power_reduction.h" //power reduction
#include "brightness.h" // Brightness option
#include "contrast.h" //Contrast menu
#include "time_menu.h" //Time menu
#include "SD.h" //High level SD
#include "read_data_menu.h" // Menu to read the measurements

const unsigned char MN01[]="Measurements";
const unsigned char MN02[]="Read data";
const unsigned char MN03[]="Options";
const unsigned char MN04[]="Contrast";
const unsigned char MN05[]="Brightness";
const unsigned char MN06[]="Time";
const unsigned char MN07[]="Interpolation";
const unsigned char MN08[]="[EXIT]";

const unsigned char MN10[]="-->"; //symbol
const unsigned char MN11[]="Choose option";
const unsigned char MN12[]="Press [OK] to cancel";
const unsigned char MN13[]="or [EXIT] to abort";

const menu_item *menu = main_menu;

const menu_item main_menu[] = { //empty rows to always have the same number of elements
	//Name,  IL UP DN, callbac function, prev., next
	{MN01, { 0, 3, 1}, 0,				 main_menu, measurements_menu },//Measurements 		(0)
	{MN02, { 1, 0, 2}, menu_read,		 main_menu, 0 },//Read data							(1)
	{MN03, { 2, 1, 3}, 0,				 main_menu, options_menu },//Options				(2)
	{MN08, { 3, 2, 0}, exit_menu,		 main_menu, 0 },// Sleep mode						(3)
	{0,	   { 0, 0, 0}, 0,				 main_menu, 0 },//NULL								(4)
};

const menu_item options_menu[] = {
	//Name,  IL UP DN, callbac function, prev., next
	{MN04, { 0, 2, 1}, menu_contrast,	 main_menu, 0 },//Contrast			 				(0)
	{MN05, { 1, 0, 2}, menu_brightness,	 main_menu, 0 },//Brightness						(1)
	{MN06, { 2, 1, 0}, 0,				 main_menu, time_menu },//Time setting				(2)
	{0,	   { 0, 0, 0}, 0,				 0, 0 },//NULL										(3)
	{0,	   { 0, 0, 0}, 0,				 0, 0 },//NULL										(4)
};

const menu_item time_menu[] = {
	//Name,  IL UP DN, callbac function, prev., next
	{MN14, { 0, 3, 1}, set_date, options_menu, 0 },//Date									(0)
	{MN15, { 1, 0, 2}, set_time, options_menu, 0 },//Time									(1)
	{0,	   { 0, 0, 0}, 0, 0, 0 },//NULL														(2)
	{0,    { 0, 0, 0}, 0, 0, 0 },//NULL														(3)
	{0,    { 0, 0, 0}, 0, 0, 0 },//NULL														(4)
};

const menu_item measurements_menu[] = {
	//Name,  IL UP DN, callbac function, prev., next
	{MN21, { 0, 4, 1}, single_measurements,main_menu, 0 },//Start the measurements			(0)
	{MN19, { 1, 0, 2}, start_measurements, main_menu, 0 },//Time of the measurements			(1)
	{MN23, { 2, 1, 3}, duration_measurements, main_menu, 0 },//Duration of the measurements	(2)	
	{MN20, { 3, 2, 4}, freq_measurements, main_menu, 0 },//Frequency of the measurements	(3)
	{MN22, { 4, 3, 0}, multi_measurements, main_menu, 0 },//Multi measurements				(4)
	
};

// Initialization of the menu
 void menu_init(void)
 {
	menu_event = E_IDDLE;
	current_menu = E_IDDLE;
	exit_callback = EXIT_NO;
	 
	contrast = 19UL;
	brightness = 2088UL;
	 
	glcd_contrast(6, contrast);
	 
	//Port configuration
	PORTB.DIRCLR	=	PIN0_bm |		// Pins B0-B4 as inputs for buttons
					PIN1_bm |
					PIN2_bm |		
					PIN3_bm |
					PIN4_bm |
					PIN5_bm;
	PORTB.PIN0CTRL	=	PORT_OPC_PULLUP_gc |	// pull-up on B0
					PORT_ISC_FALLING_gc;	// interrupt at falling edge
	PORTB.PIN1CTRL	=	PORT_OPC_PULLUP_gc |	// pull-up on B1
	 				PORT_ISC_FALLING_gc;	// interrupt at falling edge
	PORTB.PIN2CTRL	=	PORT_OPC_PULLUP_gc |	// pull-up on B2
					PORT_ISC_FALLING_gc;	// interrupt at falling edge
	PORTB.PIN3CTRL	=	PORT_OPC_PULLUP_gc |	// pull-up on B3
					PORT_ISC_FALLING_gc;	// interrupt at falling edge
	PORTB.PIN4CTRL	=	PORT_OPC_PULLUP_gc |	// pull-up on B4
					PORT_ISC_FALLING_gc;	// interrupt at falling edge
	PORTB.PIN5CTRL	=	PORT_OPC_PULLUP_gc |	// pull-up on B5
					PORT_ISC_FALLING_gc;	// interrupt at falling edge					
	 
	// Interrupt INT0 as button is pressed
	PORTB.INT0MASK = PIN0_bm |
					 PIN1_bm |
					 PIN2_bm |
					 PIN3_bm |
					 PIN4_bm |
					 PIN5_bm;
	PORTB.INTCTRL = PORT_INT0LVL_LO_gc;

	show_menu();
 }
 
// Key read function -> menu_event
void keyread (void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		menu_event = button_call;
		button_call = E_IDDLE;
	}
	if (!(menu_event == E_IDDLE))
	{
		PORTB.INTCTRL = PORT_INT0LVL_OFF_gc;
		_delay_ms(200);
		PORTB.INTCTRL = PORT_INT0LVL_LO_gc;
	}
}
// Key read for settings menu
void keyread_set(void)
{
	static uint8_t key_counter;
	keyread();
	if(RIGHT_KEY) menu_event = E_RIGHT; // Otherwise, the values would change in every cycle
	if(LEFT_KEY) menu_event = E_LEFT;
	if (RIGHT_KEY || LEFT_KEY) // Rapid change of values
	{
		key_counter++;
		if(key_counter == 20)
		{
			key_counter = 0;
			add_value_key +=5;
			if (add_value_key > MAX_VALUE_KEY) add_value_key = MAX_VALUE_KEY;
		}
	}else
	{
		key_counter = 0;
		add_value_key = 1;
	}
}

// External interrupt for switches
ISR(PORTB_INT0_vect)
{
	if (DW_KEY)		button_call = E_DOWN;
	if (UP_KEY)		button_call = E_UP;
	if (OK_KEY)		button_call = E_OK;
	if (EXIT_KEY)	button_call = E_EXIT;
	if (LEFT_KEY)	button_call = E_LEFT;
	if (RIGHT_KEY)  button_call = E_RIGHT;
}
 
// Show menu function and switches in menu control OK, EXIT, UP and DOWN switches
void show_menu(void)
 {
	if (menu_event == E_EXIT)
	{
		menu_event = E_IDDLE;
		if ((menu+current_menu)->prev_menu)
		{
			menu=(menu+current_menu)->prev_menu;
			current_menu = E_IDDLE;// Set symbol to the zero position
		}
	}
	if (menu_event == E_OK)
	{
		menu_event = E_IDDLE;
		if ((menu+current_menu)->callback)
		{
			(menu+current_menu)->callback(); // If there is callback function then execute
		}
		if ((menu+current_menu)->next_menu)	// If not then sub menu or previous menu is executed
		{
			menu=(menu+current_menu)->next_menu;
			current_menu = E_IDDLE; // Set symbol to the zero position
		}
	}
	if (menu_event == E_LEFT) menu_event = E_IDDLE;
	if (menu_event == E_RIGHT) menu_event = E_IDDLE;
	current_menu = (menu+current_menu)->next_state[menu_event];
	uint8_t position=0;
	for (uint8_t i=0;i<5;i++) // Five lines of the menu
	{
		position=i*10 + 10;
		draw_text((char*)((menu+i)->name),20,position,Tahoma7,1);
	}
	position=current_menu*10 + 10;
	draw_text((char*)MN10,5,position,Tahoma7,1); // Show symbol
 }

// Control the callback functions
void show_callback (void)
{
	if (menu_event == E_OK) // Save data
	{
		menu_event = E_IDDLE;
		exit_callback = EXIT_YES_OK;
	}
	if (menu_event == E_EXIT) // Abort or cancel
	{
		while(1) 
		{
			keyread();
			if (menu_event == E_OK) // Cancel
			{
				menu_event = E_IDDLE;
				exit_callback = EXIT_NO;
				break;
			}
			if (menu_event == E_EXIT) // Abort
			{
				menu_event = E_IDDLE;
				exit_callback = EXIT_YES_EX;
				break;
			}
			draw_text((char*)MN11,10,10,Tahoma7,1);
			draw_text((char*)MN12,10,30,Tahoma7,1);
			draw_text((char*)MN13,10,40,Tahoma7,1);
			glcd_refresh();
		}
	}
}