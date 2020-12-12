#ifndef _KEY_H
#define _KEY_H
#include "main.h"
#include "DELAY/delay.h"

// return 255 if no key is pressed.

void Key_Init();
uint8_t Key_Scan();
uint8_t Key_ScanRaw();

#endif
