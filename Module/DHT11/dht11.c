#include "DHT11/dht11.h"
#include "OLED/oled_iic.h"

uint8_t DHT11_rawData[40];
double DHT11_temp,DHT11_humid;

void DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_Initer;
	int i = 0;

	for (i = 0; i < 40; i++)
		DHT11_rawData[i] = 0xFFu;
	__HAL_RCC_GPIOE_CLK_ENABLE(); // Enable GPIOE

	GPIO_Initer.Pin = GPIO_PIN_2;
	GPIO_Initer.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initer.Pull = GPIO_PULLUP;
	GPIO_Initer.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_Initer);
}

uint8_t DHT11_Measure(uint8_t *tempd, uint8_t *tempf, uint8_t *humid, uint8_t *humif)
{
	uint8_t i, counter, res[5];

	DHT11_GPIO_OUT();
	// send start signal
	DHT11_GPIO_WRITE(0);
	Delay_ms(20);
	DHT11_GPIO_WRITE(1);
	// wait for DHT11
	DHT11_GPIO_IN();
	Delay_us(20); // wait first LOW appeared;
	if (DHT11_GPIO_READ())
		return 0;
	while (!DHT11_GPIO_READ()) // wait response1 finished
		;
	while (DHT11_GPIO_READ()) //wait response2 finished
		;
	// ready to read
	for (i = 0; i < 40; i++)
	{
		while (!DHT11_GPIO_READ()) // leading LOW for ~50us
			;
		for (counter = 0; DHT11_GPIO_READ(); counter += 5) // count the duration of HIGH
			Delay_us(5);
		if (counter < 50) // HIGH is less than 50us
			DHT11_rawData[i] = 0;
		else // HIGH is higher than 50us
			DHT11_rawData[i] = 1;
	}
	// wait for release
	Delay_us(150);
	// end
	// reset the DHT11_rawData wire
	DHT11_GPIO_OUT();
	DHT11_GPIO_WRITE(1);
	for (i = 0; i < 5; i++)
		res[i] = __DHT11_b2c(DHT11_rawData + 8 * i);
	if (res[4] != res[0] + res[1] + res[2] + res[3])
		return 0;
	if (humid != NULL)
		*humid = res[0];
	if (humif != NULL)
		*humif = res[1];
	if (tempd != NULL)
		*tempd = res[2];
	if (tempf != NULL)
		*tempf = res[3];
	return 1;
}

uint8_t DHT11_Refresh()
{
    uint8_t tempd=0,tempf=0,humid=0,humif=0;
    if(!DHT11_Measure(&tempd, &tempf, &humid, &humif))
        return 0;
    DHT11_temp=(double)tempd+tempf/10.0;
    DHT11_humid=(double)humid+humif/10.0;
    return 1;
}

double DHT11_GetTemp()
{
    return DHT11_temp;
}
double DHT11_GetHumid()
{
    return DHT11_humid;
}

uint8_t __DHT11_b2c(uint8_t *addr)
{
	uint8_t base = 1, res = 0;
	uint8_t *ptr;
	for (ptr = addr + 7; ptr >= addr; ptr--)
	{
		if (*ptr)
			res += base;
		base <<= 1;
	}
	return res;
}
