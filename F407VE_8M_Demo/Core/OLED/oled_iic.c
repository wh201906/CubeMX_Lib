#include "OLED/oled_iic.h"
#include "OLED/codetab.h"

uint8_t RevState=1;
uint8_t brightness=0xff;

void I2C_WriteByte(uint8_t addr,uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte(OLED_ADDRESS);
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	IIC_Send_Byte(data);
	IIC_Wait_Ack();
	IIC_Stop();
}

void WriteCmd(uint8_t I2C_Command)//д����
{
	I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat(uint8_t I2C_Data, uint8_t reverse)//д����
{
    if(reverse==REVERSE_OFF)
        I2C_WriteByte(0x40, I2C_Data);
    else if(reverse==REVERSE_ON)
        I2C_WriteByte(0x40, ~I2C_Data);
}

void OLED_Init(void)
{
	IIC_Init();
	Delay_ms(100); //�������ʱ����Ҫ
	
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(brightness); //���ȵ��� 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	if(RevState)
		WriteCmd(0xa6);
	else
		WriteCmd(0xa7);
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
    OLED_CLS();
}

void OLED_SetPos(uint8_t x, uint8_t y) //������ʼ������
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(uint8_t fill_Data)//ȫ�����
{
	uint8_t m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
		{
			WriteDat(fill_Data,REVERSE_OFF);
		}
	}
}

void OLED_CLS(void)//����
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED�������л���
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X14);  //������ɱ�
	WriteCmd(0XAF);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X10);  //�رյ�ɱ�
	WriteCmd(0XAE);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); ch[] -- Ҫ��ʾ���ַ���; TextSize -- �ַ���С(1:6*8 ; 2:8*16)
// Description    : ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
//--------------------------------------------------------------
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize, uint8_t reverse)
{
	uint8_t c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 122)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i],reverse);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y+=2;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i],reverse);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8],reverse);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t index)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); index:������codetab.h�е�����
// Description    : ��ʾcodetab.h�еĺ���,16*16����
//--------------------------------------------------------------
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t index, uint8_t reverse)
{
	uint8_t wm=0;
	unsigned int  adder=32*index;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder],reverse);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder],reverse);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
// Description    : ��ʾBMPλͼ
//--------------------------------------------------------------
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[], uint8_t reverse)
{
	unsigned int j=0;
	uint8_t x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++],reverse);
		}
	}
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t TextSize, uint8_t reverse)
{
	uint8_t i = 0;
	switch(TextSize)
	{
		case 1:
		{
			ch -= 32;
			OLED_SetPos(x,y);
			for(i=0;i<6;i++)
				WriteDat(F6x8[ch][i],reverse);
		}break;
		case 2:
		{
			ch -= 32;
			OLED_SetPos(x,y);
			for(i=0;i<8;i++)
				WriteDat(F8X16[ch*16+i],reverse);
			OLED_SetPos(x,y+1);
			for(i=0;i<8;i++)
				WriteDat(F8X16[ch*16+i+8],reverse);
		}break;
	}
}

int OLED_ShowInt(uint8_t x, uint8_t y, int val, uint8_t TextSize, uint8_t reverse)
{
	uint8_t str[12], ch;
	int i, n;
	if (val == 0)
	{
		OLED_ShowChar(x, y, '0', TextSize,reverse);
		return 1;
	}
	else
	{
		if (val < 0)
		{
			str[0] = '-';
			val = -val;
		}
		else
			str[0] = ' ';
		for (n = 1; val > 0; val /= 10, n++)
			str[n] = (val % 10) + '0';
		str[n] = '\0';
		for (i = 1; i <= n / 2; i++)
		{
			ch = str[i];
			str[i] = str[n - i];
			str[n - i] = ch;
		}
		if (str[0] == '-')
		{
			OLED_ShowStr(x, y, str, TextSize,reverse);
			return n;
		}
		else
		{
			OLED_ShowStr(x, y, str + 1, TextSize,reverse);
			return (n-1);
		}
	}
}

int OLED_ShowFloat(uint8_t x, uint8_t y, double val, uint8_t TextSize, uint8_t reverse)
{
	char out[16];
	sprintf(out,"%.3lf",val);
	out[15]='\0';
	OLED_ShowStr(x,y,out,TextSize,reverse);
	return(strlen(out));
}
void OLED_SetBrightness(uint8_t val)
{
	WriteCmd(0xAE);
	WriteCmd(0x81);
	WriteCmd(val);
	WriteCmd(0xAf);
}

void OLED_Reverse(void)
{
	RevState=!RevState;
	if(RevState)
		WriteCmd(0xa6);
	else
		WriteCmd(0xa7);
}