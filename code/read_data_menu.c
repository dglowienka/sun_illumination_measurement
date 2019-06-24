#include <avr/io.h>
#include <stdio.h>	// Printf functions
#include <stdint.h> // Uint
#include <string.h> // String functions
#include <math.h> // Math functions
#include <util/delay.h>

#include "SD.h" //High level SD
#include "menu.h" // Menu for LCD
#include "read_data_menu.h" // Menu to read the measurements
#include "LCD/fonts/fonts.h" //fonts
#include "LCD/st7565.h" // LCD low level
#include "LCD/graphics.h" // LCD high level
#include "RTC.h" // RTC
#include "battery_control.h" // Battery control ADC

const unsigned char graphics[189][2] = {{5,4}, {5,5},	{5,6},	{5,7},	{5,8},	{5,9},	{5,10},	{5,11},	{5,12},	{5,13},	{5,14},	{5,15},	{5,16},	{5,17},	{5,18},	{5,19},	{5,20},	{5,21},	{5,22},	{5,23},	{5,24},	{5,25},	{5,26},	{5,27},	{5,28},	{5,29},	{5,30},	{5,31},	{5,32},	{5,33},	{5,34},	{5,35},	{5,36},	{5,37},	{5,38},	{5,39},	{5,40},	{5,41},	{5,42},	{5,43},	{5,44},	{5,45},	{5,46},	{5,47},	{5,48},	{5,49},	{5,50},	{5,51},	{5,52},	{5,53},	{5,54},	{5,55},	{5,56},	{5,57},	{5,58},	{4,57},	{5,57},	{6,57},	{7,57},	{8,57},	{9,57},	{10,57},	{11,57},	{12,57},	{13,57},	{14,57},	{15,57},	{16,57},	{17,57},	{18,57},	{19,57},	{20,57},	{21,57},	{22,57},	{23,57},	{24,57},	{25,57},	{26,57},	{27,57},	{28,57},	{29,57},	{30,57},	{31,57},	{32,57},	{33,57},	{34,57},	{35,57},	{36,57},	{37,57},	{38,57},	{39,57},	{40,57},	{41,57},	{42,57},	{43,57},	{44,57},	{45,57},	{46,57},	{47,57},	{48,57},	{49,57},	{50,57},	{51,57},	{52,57},	{53,57},	{54,57},	{55,57},	{56,57},	{57,57},	{58,57},	{59,57},	{60,57},	{61,57},	{62,57},	{63,57},	{64,57},	{65,57},	{66,57},	{67,57},	{68,57},	{69,57},	{70,57},	{71,57},	{72,57},	{73,57},	{74,57},	{75,57},	{76,57},	{77,57},	{78,57},	{79,57},	{80,57},	{81,57},	{82,57},	{83,57},	{84,57},	{85,57},	{86,57},	{87,57},	{88,57},	{89,57},	{90,57},	{91,57},	{92,57},	{93,57},	{94,57},	{95,57},	{96,57},	{97,57},	{98,57},	{99,57},	{100,57},	{101,57},	{102,57},	{103,57},	{104,57},	{105,57},	{106,57},	{107,57},	{108,57},	{109,57},	{110,57},	{111,57},	{112,57},	{113,57},	{114,57},	{115,57},	{116,57},	{117,57},	{118,57},	{119,57},	{120,57},	{121,57},	{122,57},	{123,57},	{4,5},	{4,6},	{3,6},	{6,5},	{6,6},	{7,6},	{121,55},	{121,56},	{121,57},	{121,58},	{121,59},	{122,56},	{122,57},	{122,58}
};

str_files_menu main_files[5] = {
 //Name,  IL UP DN, Size
	{"", {0, 4, 1}, 0}, // (0)
	{"", {1, 0, 2}, 0}, // (1)
	{"", {2, 1, 3}, 0}, // (2)
	{"", {3, 2, 4}, 0}, // (3)
	{"", {4, 3, 5}, 0}, // (4)
};

// Read data from the SD card
void menu_read(void)
{
	exit_callback = EXIT_NO;
	current_file = E_IDDLE;
	res = f_opendir(&dir, "/"); // Open the directory
	if (res == FR_OK) (no_files = scan_files()); // Scan files in the directory
	
	while(!(exit_callback))
	{
		keyread_files();
		show_files();
		show_RTC();
		show_battery();
		glcd_refresh();
	}
	f_closedir(&dir);
}

// Control the keys in the read data menu
void keyread_files(void)
{
	keyread();
	if (menu_event == E_EXIT) // Abort the menu
	{
		menu_event = E_IDDLE;
		exit_callback = EXIT_YES_EX;
	}
	if (menu_event == E_OK) // Go to the file
	{
		read_data();	
		menu_event = E_IDDLE;
	}
	if (menu_event == E_LEFT) menu_event = E_IDDLE;
	if (menu_event == E_RIGHT) menu_event = E_IDDLE;
}

// Show the files names 
void show_files(void)
{
	if(main_files[current_file].next_file[menu_event] == 5) // At the bottom of the list
	{
		no_files = scan_files(); // Show next files
		current_file=E_IDDLE; // Zero position
		menu_event = E_IDDLE;
	}
	main_files->next_file[1] = no_files-1; // For top value
	current_file = (main_files[current_file].next_file[menu_event])%no_files; // Check the current position with the number of files(modulo)
	
	uint8_t position = 0;
	char buff_f_size[10];
	for (uint8_t i=0;i<no_files;i++)
	{
		position=i*10 + 10;
		draw_text((char*)((main_files+i)->f_name),20,position,Tahoma7,1); // Draw the name of the file
		if ((main_files+i)->f_name) // If there is some file
		{
			sprintf(buff_f_size,"%d [kB]",(main_files+i)->file_size);
			draw_text(buff_f_size,90,position,Tahoma7,1); // Draw the size of the file
		}
	}
	position=current_file*10 + 10;
	draw_text((char*)MN10,5,position,Tahoma7,1); // Show symbol
}

// Start node to be scanned (also used as work area) 
int scan_files (void) 
{	
	uint8_t n = 0;
	for (uint8_t i=0;i<5;i++) // Scan max first 5 files
	{
		res = f_readdir(&dir, &fno); // Read a directory item
		if (res != FR_OK || fno.fname[0] == 0) break;  // Break on error or end of dir
		if (fno.fname[0] == '.') continue; // Ignore dot entry
		main_files[i].file_size = roundf((fno.fsize + 1023UL)/1024UL); // Copy the file size to the structure [kB]
		if (fno.fattrib & AM_DIR) // Copy the directory name to the structure
		{
			strcpy((char *)main_files[i].f_name,(char *)fno.fname);
			res = scan_files();
			if (res != FR_OK) break;
		} else strcpy((char *)main_files[i].f_name,(char *)fno.fname); // Copy the file name to the structure
		n++;
	}
	return n; // Return number of files read
}

// Read a text file from SD and display
int read_data(void)
{
	FIL fil;       // File object
	FRESULT fr;    // FatFs return code
	char buff_data[10]; // Buffer for one line of data
	unsigned int x_data_tmp = 0; // Temporary data for x axis
	unsigned int y_data_tmp = 0; // Temporary data for y axis
	uint16_t x_data_buffer = 0; // Buffer data for x axis
	uint16_t y_data_buffer = 0; // Buffer data for y axis
	uint8_t count_average = 0;
	uint8_t n = 0;
	
	// Open a text file
	fr = f_open(&fil, (char *) ((main_files+current_file)->f_name), FA_READ);
	if (fr) return (int)fr;
	
	// Read all data and save to array
	while(f_gets(buff_data, sizeof buff_data, &fil)) //Enter button is '\0' so function cycles at every enter button
	{
		count_average++;
		sscanf(buff_data,"%u,%u", &x_data_tmp, &y_data_tmp); // Copy the string data to variables
		x_data_buffer += x_data_tmp;
		y_data_buffer += y_data_tmp;
		if (count_average == XY_MEAN)
		{
			x_data[n] = x_data_buffer/XY_MEAN;
			y_data[n] = y_data_buffer/XY_MEAN;
			if(n>X_AXIS) break;
			count_average=0;
			x_data_buffer=0;
			y_data_buffer=0;
			
			//glcd_pixel(n, y_data[n], 1);
			n++;
		}
	}
	//draw_text(x_data,10,10,Tahoma7,1);
	while(1)
	{
		draw_result_graph();
		draw_text("04.06.15",95,0,Tahoma7,1);
		glcd_refresh();
		keyread();
		if(menu_event == E_EXIT) break;
	}
	// Close the file
	f_close(&fil);
	return 0;
}

// Graphics of the x and y axis
void draw_result_graph(void)
{
	for (uint8_t i=0;i<189;i++)
	{
		glcd_pixel(graphics[i][0],graphics[i][1],1);
	}
	for (uint8_t i=0;i<X_AXIS;i++)
	{
		glcd_pixel(graphics[i][0],graphics[i][1],1);
		glcd_pixel(i, y_data[i], 1);
	}
	//x=x+3; // move (0,0) to (3,3)
	//y=(65-y)-3; // Reverse the y axis for convenience 
}