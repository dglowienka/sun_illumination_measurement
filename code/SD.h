#include "SD/integer.h" // Declarations of Integer types
#include "SD/ff.h"		// Declarations of FatFs API
#include "SD/diskio.h" // High level SD control
#include "SD/ffconf.h" // High level SD control

UINT bw;
BYTE mount;

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */

void init_SD(void);