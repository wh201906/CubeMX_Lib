#ifndef _PE4302_H
#define _PE4302_H

#include "sys.h"
#include "delay.h"

#define PE4302_LE   PGout(15)
#define PE4302_CLK  PGout(14)
#define PE4302_DATA PGout(13)

#define PE4302b_LE   PGout(12)
#define PE4302b_CLK  PGout(11)
#define PE4302b_DATA PGout(10)

void PE4302_Init(void);
void PE4302_Set(float val);
void PE4302b_Init(void);
void PE4302b_Set(float val);
void PE4302_DoubleSet(float val);
#endif
