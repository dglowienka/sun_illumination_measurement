#define X_AXIS 128
#define Y_AXIS 64
#define XY_MEAN 1

void menu_read(void);
void keyread_files(void);
void show_files(void);
int read_data (void);
int scan_files (void);
void draw_result_graph(void);

uint8_t no_files;
unsigned char x_data[130]; // Storage data for x axis
unsigned char y_data[130]; // Storage data for y axis
typedef struct _files_menu
{
	unsigned char f_name[13];		// Name of the file
	unsigned char next_file[3];		// States for the buttons
	uint16_t file_size;				// File size
} str_files_menu;

str_files_menu main_files[5];

FRESULT res;
FILINFO fno;
/*
	DWORD	fsize;			// File size
	WORD	fdate;			// Last modified date 
	WORD	ftime;			// Last modified time 
	BYTE	fattrib;		// Attributes AM_DIR, AM_RDO, AM_HID, AM_SYS and AM_ARC
	TCHAR	fname[13];		// Short file name (8.3 format)
*/
DIR dir;

unsigned char current_file;