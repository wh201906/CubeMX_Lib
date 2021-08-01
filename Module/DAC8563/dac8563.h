#ifndef _TLV56XX_H_
#define _TLV56XX_H_

#include <stdint.h>  
#include "sys.h"

#define ADC5863_DIN   PCout(8)
#define ADC5863_SCLK 	PCout(12)
#define ADC5863_SYNC	PCout(11)
#define ADC5863_CLR 	PCout(10)//
#define ADC5863_LDAC	  PCout(9)//


#define SCLK_H       ADC5863_SCLK = 1
#define SYNC_H       ADC5863_SYNC = 1
#define CLR_H        ADC5863_CLR = 1
#define LDAC_H       ADC5863_LDAC = 1
#define DIN_H        ADC5863_DIN = 1

#define SCLK_L       ADC5863_SCLK = 0
#define SYNC_L       ADC5863_SYNC = 0
#define CLR_L        ADC5863_CLR = 0
#define LDAC_L       ADC5863_LDAC = 0
#define DIN_L        ADC5863_DIN = 0

#define CMD_SETA_UPDATEA      		0x18	// д��DAC-A����Ĵ���������DAC-A +16λ����
#define CMD_SETB_UPDATEB      		0x19	// д��DAC-B����Ĵ���������DAC-B +16λ����
#define CMD_UPDATE_ALL_DACS	   		0x0F	// ��������DAC ��16λֻ��ʱ�Ӽ���

// Gain
#define CMD_GAIN	    					0x02	// default with internal Vref
#define DATA_GAIN_B2_A2    			0x00	// ���棺DAC-B����= 2��DAC-A����= 2��Ĭ��Ϊ�ڲ�VREF��
#define DATA_GAIN_B2_A1       		0x01	// ���棺DAC-B����= 2��DAC-A����= 1
#define DATA_GAIN_B1_A2         	0x02	// ���棺DAC-B����= 1��DAC-A����= 2
#define DATA_GAIN_B1_A1       		0x03	// ���棺DAC-B����= 1��DAC-A����= 1���ϵ�Ĭ��ֵ��






#define CMD_PWR_UP_A_B		      	0x20	// ����DAC�ϵ�����ģʽ
#define DATA_PWR_UP_A		      	  0x0001	// �ϵ�DAC-A 
#define DATA_PWR_UP_B		      	  0x0002	// �ϵ� DAC-B
#define DATA_PWR_UP_A_B		      	0x0003	// �ϵ�DAC-A��DAC-B
#define DATA_PWR_DOWN1_A		      0x0041	// �ض�DAC-A; 1k����GND
#define DATA_PWR_DOWN1_B		      0x0042	// �ض�DAC-B; 1k����GND
#define DATA_PWR_DOWN1_A_B		      0x0043	// �ض�DAC-A��DAC-B; 1k����GND
#define DATA_PWR_DOWN2_A		      	0x0081	// �ض�DAC-A; 100k����GND
#define DATA_PWR_DOWN2_B		      	0x0082	// �ض�DAC-B; 100k����GND
#define DATA_PWR_DOWN2_A_B		      0x0083	// �ض�DAC-A��DAC-B; 100k����GND
#define DATA_PWR_DOWN3_A		      	0x00C1	// �ض�DAC-A; ����
#define DATA_PWR_DOWN3_B		      	0x00C2	// �ض�DAC-B; ����
#define DATA_PWR_DOWN3_A_B		      0x00C3	// �ض�DAC-A��DAC-B;����


#define CMD_RESET_ALL_REG	      	0x28	// Software reset
#define DATA_RESET_ALL_REG     		0x0001	// ��λ���мĴ�������������DAC���ϵ縴λ���£�
#define DATA_RESET_ADCAB     		0x0000 //��λDAC-A��DAC-B����Ĵ�������������DAC


#define CMD_LDAC_DIS	        	0x30	// ����LDAC�Ĵ���
#define DATA_LDAC_AB       		0x0000 //LDAC������Ч����DAC-B��DAC-A
#define DATA_LDAC_B       		0x0001 //LDAC������Ч����DAC-B; ��DAC-A��Ч
#define DATA_LDAC_A       		0x0002  //LDAC���Ŷ�DAC-B��Ч; DAC-A��Ч
#define DATA_LDAC_NAB       		0x0003 //LDAC���Ŷ�DAC-B��DAC-A��Ч

#define CMD_INTERNAL_REF_DIS    	0x38	// Disable internal reference and reset DACs to gain = 1
#define DATA_INTERNAL_REF_DIS    	0x0000	// �����ڲ��ο�����DAC��λΪ����= 1

#define CMD_INTERNAL_REF_EN       0x38	// Enable Internal Reference & reset DACs to gain = 2
#define DATA_INTERNAL_REF_EN    	0x0001	//�����ڲ��ο��͸�λDAC�Ի������= 2

extern u16 SineWave_Value[];
extern u16 TriangleWave_Value[];
void DAC8563_Init(void);
void DAC8563_WRITE(uint8_t cmd,uint16_t data);
void DAC_OutAB(uint16_t data_a, uint16_t data_b);

#endif










