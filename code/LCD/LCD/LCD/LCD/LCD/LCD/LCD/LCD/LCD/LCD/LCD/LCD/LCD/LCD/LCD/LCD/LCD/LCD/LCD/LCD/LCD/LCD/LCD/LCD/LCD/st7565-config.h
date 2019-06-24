// Setup for ST7565R in SPI mode
/** The chip select pin */
#define GLCD_CS1_set PORTD.OUTSET = PIN1_bm
#define GLCD_CS1_clr PORTD.OUTCLR = PIN1_bm
/** The reset pin (this is required and should not be tied high) */
#define GLCD_RESET_set PORTD.OUTSET = PIN2_bm
#define GLCD_RESET_clr PORTD.OUTCLR = PIN2_bm
/** The A0 pin, which selects command or data mode */
#define GLCD_A0_set PORTD.OUTSET = PIN3_bm
#define GLCD_A0_clr PORTD.OUTCLR = PIN3_bm
/** The clock pin */
#define GLCD_SCL_set PORTD.OUTSET = PIN7_bm
#define GLCD_SCL_clr PORTD.OUTCLR = PIN7_bm
/** The data pin */
#define GLCD_SDA_set PORTD.OUTSET = PIN5_bm
#define GLCD_SDA_clr PORTD.OUTCLR = PIN5_bm

/** Screen width in pixels (tested with 128) */
#define SCREEN_WIDTH 128
/** Screen height in pixels (tested with 64) */
#define SCREEN_HEIGHT 64

/** Define this if your screen is incorrectly shifted by 4 pixels */
#define ST7565_REVERSE

/** By default we only write pages that have changed.  Undefine this
    if you want less/faster code at the expense of more SPI operations. */
#undef ST7565_DIRTY_PAGES

unsigned char glcd_buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8];