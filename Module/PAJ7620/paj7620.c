#include "paj7620.h"
#include "paj7620_init.h"
#include "DELAY/delay.h"
#include "I2C/softi2c.h"

SoftI2C_Port PAJ7620_port;

uint8_t PAJ7620_WriteReg(uint8_t reg, uint8_t data)
{
	return SoftI2C_Write(&PAJ7620_port, PAJ7620_ID, reg, &data, 1);
}

uint8_t PAJ7620_ReadReg(uint8_t reg, uint8_t *data)
{
	return SoftI2C_Read(&PAJ7620_port, PAJ7620_ID, reg, data, 1);
}

uint8_t PAJ7620_Read16(uint8_t reg, uint16_t *data)
{
	uint8_t tmp[2];
  if (!SoftI2C_Read(&PAJ7620_port, PAJ7620_ID, reg, tmp, 2))
    return 0;
  *data = ((uint16_t)tmp[1] << 8) | tmp[0];
  return 1;
}

void PAJ7620_SelectBank(uint8_t bank)
{
	PAJ7620_WriteReg(PAJ7620_REGITER_BANK_SEL, bank);
}

/****************************************************************
    Function Name: PAJ7620_Init
    Description:  PAJ7620 REG INIT
    Parameters: none
    Return: error code; success: return 0
****************************************************************/
uint8_t PAJ7620_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
	//Near_normal_mode_V5_6.15mm_121017 for 940nm
	int i = 0;
	uint8_t error;
	uint8_t data0 = 0, data1 = 0;
	//wakeup the sensor
	Delay_us(700); //Wait 700us for PAJ7620U2 to stabilize

	SoftI2C_SetPort(&PAJ7620_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
	SoftI2C_Init(&PAJ7620_port, 400000, SI2C_ADDR_7b);

	PAJ7620_SelectBank(PAJ7620_BANK0);
	PAJ7620_SelectBank(PAJ7620_BANK0);

	PAJ7620_ReadReg(0, &data0);
	PAJ7620_ReadReg(1, &data1);

	if ((data0 != 0x20) || (data1 != 0x76))
	{
		return 0;
	}
	if (data0 == 0x20)
	{
		// Serial.println("wake-up finish.");
	}

	for (i = 0; i < (sizeof(PAJ7620_initArray) / sizeof(PAJ7620_initArray[0])); i++)
	{
		uint16_t word = PAJ7620_initArray[i];
		uint8_t high, low;
		high = (word & 0xFF00) >> 8;
		low = (word & 0x00FF);
		PAJ7620_WriteReg(high, low);
	}

	/**
        Setting normal mode or gaming mode at BANK1 register 0x65/0x66 R_IDLE_TIME[15:0]
        T = 256/System CLK = 32us,
        Ex:
        Far Mode: 1 report time = (77+R_IDLE_TIME)T
        Report rate 120 fps:
        R_IDLE_TIME=1/(120*T)-77=183

        Report rate 240 fps:
        R_IDLE_TIME=1/(240*T)-77=53

        Near Mode: 1 report time = (112+R_IDLE_TIME)T

        Report rate 120 fps:
        R_IDLE_TIME=1/(120*T)-120=148

        Report rate 240 fps:
        R_IDLE_TIME=1/(240*T)-112=18

    */
	PAJ7620_SelectBank(PAJ7620_BANK1); //gesture flage reg in Bank1
	// PAJ7620_WriteReg(0x65, 0xB7); // far mode 120 fps
	PAJ7620_WriteReg(0x65, 0x12); // near mode 240 fps

	PAJ7620_SelectBank(PAJ7620_BANK0); //gesture flage reg in Bank0

	return 1;
}

uint16_t PAJ7620_Gesture9(uint16_t entryTime, uint16_t quitTime)
{
	uint16_t gesture1, gesture2;
	if (!PAJ7620_Read16(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &gesture1)) // first read
		return 0;

	if (gesture1 == GES_CLOCKWISE_FLAG || gesture1 == GES_ANTI_CLOCKWISE_FLAG || gesture1 == GES_WAVE_FLAG)
		return gesture1;
	else if (gesture1 == GES_FORWARD_FLAG || gesture1 == GES_BACKWARD_FLAG)
	{
		Delay_ms(quitTime);
		return gesture1;
	}
	else if (gesture1 == GES_UP_FLAG || gesture1 == GES_DOWN_FLAG || gesture1 == GES_LEFT_FLAG || gesture1 == GES_RIGHT_FLAG)
	{
		Delay_ms(entryTime);
		if (!PAJ7620_Read16(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &gesture2)) // second read
			return 0;
		if (gesture2 == GES_FORWARD_FLAG || gesture2 == GES_BACKWARD_FLAG)
		{
			Delay_ms(quitTime);
			return gesture2;
		}
		else
			return gesture1;
	}
	else
	{
		return 0;
	}
	
}