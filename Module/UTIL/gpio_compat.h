#ifndef _GPIO_COMPAT_H
#define _GPIO_COMPAT_H

// for maximum GPIO speed in F0/F1/F3
#if !defined(GPIO_SPEED_FREQ_VERY_HIGH)
#define GPIO_SPEED_FREQ_VERY_HIGH GPIO_SPEED_FREQ_HIGH
#endif

#endif