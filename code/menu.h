extern const unsigned char MN01[];
extern const unsigned char MN02[];
extern const unsigned char MN03[];
extern const unsigned char MN04[];
extern const unsigned char MN05[];
extern const unsigned char MN06[];
extern const unsigned char MN07[];
extern const unsigned char MN08[];
extern const unsigned char MN10[];
extern const unsigned char MN11[];
extern const unsigned char MN12[];
extern const unsigned char MN13[];

#define	UP_KEY		(!(PORTB.IN & PIN4_bm))
#define	DW_KEY		(!(PORTB.IN & PIN0_bm))
#define	LEFT_KEY	(!(PORTB.IN & PIN2_bm))
#define	RIGHT_KEY	(!(PORTB.IN & PIN1_bm))
#define	OK_KEY		(!(PORTB.IN & PIN3_bm))
#define	EXIT_KEY	(!(PORTB.IN & PIN5_bm))

#define E_IDDLE		0
#define E_UP 		1
#define E_DOWN 		2
#define E_OK 		3
#define E_RIGHT		4
#define E_LEFT 		5
#define E_EXIT 		6

#define EXIT_YES_EX 2
#define EXIT_YES_OK 1
#define EXIT_NO 0

#define MAX_VALUE_KEY 3600

void menu_init(void);
void show_menu(void);
void keyread (void);
void keyread_set(void);
void keyread_data(void);
void show_callback (void);

void menu_options(void);
void menu_interpolation(void);

unsigned char current_menu;
unsigned char menu_event;
volatile uint8_t button_call;
uint16_t add_value_key;

uint8_t exit_callback;

typedef struct _menu_item
{
	const unsigned char* name;				//name
	unsigned char next_state[3];			//states for buttons
	void (*callback)(void);					//callback function
	const struct _menu_item *prev_menu;		//pointer to the previous menu
	const struct _menu_item *next_menu;		//pointer to the following menu
} menu_item;

extern const menu_item main_menu[];
extern const menu_item options_menu[];
extern const menu_item time_menu[];
extern const menu_item measurements_menu[];
extern const menu_item *menu;