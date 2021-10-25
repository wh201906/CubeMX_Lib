#include "mpu6050.h"
#include "I2C/softi2c.h"

SoftI2C_Port MPU6050_Port;

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";

void get_ms(unsigned long *count)
{
  *count = HAL_GetTick();
}