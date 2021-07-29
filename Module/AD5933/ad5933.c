#include "ad5933.h"
#include "DELAY/delay.h"
#include "I2C/softi2c.h"
#include <math.h>

#define Delay_IIC 1
#define AD5933 1
#define AD5933_MCLK 16.776 //=536870912/MCLK;
//#define	AD5933_MCLK_USE_OUT	1	//0内部时钟  1外部时钟
#define AD5933_Correction 100604823.421598073
//#define AD5933_Correction 990976.475417555122496  //小电阻校准系数

SoftI2C_Port AD5933_port;
void AD5933_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  SoftI2C_SetPort(&AD5933_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&AD5933_port, 400000, SI2C_ADDR_7b);
}

uint8_t AD5933_ReadReg(uint8_t reg, uint8_t *data)
{
  if(!SoftI2C_Write(&AD5933_port, AD5933_ADDR, AD5933_CMD_SETADDR, &reg, 1))
  {
    return 0;
  }
  SoftI2C_Start(&AD5933_port);
  if (!SoftI2C_SendAddr(&AD5933_port, AD5933_ADDR, SI2C_READ))
  {
    return 0;
  }
  *data = SoftI2C_ReadByte_ACK(&AD5933_port, SI2C_NACK);
  SoftI2C_Stop(&AD5933_port);
  return 1;
}

uint8_t AD5933_WriteReg(uint8_t reg, uint8_t data)
{
  return SoftI2C_Write(&AD5933_port, AD5933_ADDR, reg, &data, 1);
}

uint16_t AD5933_Tempter(void)
{
  // unsigned char Status;  //保存状态
  unsigned int Tm; //保存实部，虚部，温度
  uint8_t tmp;

  //   //复位AD5933
  //   AD5933_WriteReg(0x80,0XB1);
  //   AD5933_WriteReg(0x81,0X00);

  //启动温度测量
  AD5933_WriteReg(AD5933_REG_CTRL0, 0x93);

  //等待转换完成
  //   do
  //    {
  //
  //     Status=Rece_Byte(0x8F);
  //
  //}while(!(Status & 0x01));
  //读出温度，保存在Tm中
  AD5933_ReadReg(AD5933_REG_TEMP0, &tmp);
  Tm = tmp;
  Tm <<= 8;
  AD5933_ReadReg(AD5933_REG_TEMP1, &tmp);
  Tm += tmp;
  Tm <<= 2;

  return Tm;
}

float resistance[200];
float rads[200];
int AD5933_Dat_Re[200];
int AD5933_Dat_Im[200];

void Maopao_Paixu(float *dat, uint16_t leng)
{
  uint16_t i, j;
  float buf;
  for (j = 0; j < leng - 1; j++)
    for (i = 0; i < leng - j - 1; i++)
      if (dat[i] > dat[i + 1])
      {
        buf = dat[i];
        dat[i] = dat[i + 1];
        dat[i + 1] = buf;
      }
}

float Get_resistance(uint16_t num)
{
  uint16_t i;
  float navle;
  Maopao_Paixu(resistance, num);
  navle = resistance[0];
  for (i = num / 2 - num / 4; i < num / 2 + num / 4; i++)
  {
    navle = (navle + resistance[i]) / 2;
  };
  ;
  return navle * AD5933_Correction;
}
void Fre_To_Hex(float fre, uint8_t *buf)
{
  uint32_t dat;
  dat = (536870912 / (double)(AD5933_MCLK * 1000000)) * fre;
  buf[0] = dat >> 16;
  buf[1] = dat >> 8;
  buf[2] = dat;
}
/*Fre_Begin起始频率，Fre_UP频率增量，UP_Num增量数，OUTPUT_Vatage输出电压，Gain增益系数，SWeep_Rep扫频为1重复为0*/

/*
Fre_Begin 		开始频率 （HZ）
Fre_UP				步进频率（HZ）
UP_Num				步进次数
OUTPUT_Vatage	输出电压
								AD5933_OUTPUT_2V
								AD5933_OUTPUT_1V
								AD5933_OUTPUT_400mV
								AD5933_OUTPUT_200mV
								
Gain					PGA增益			
							AD5933_Gain_1
							AD5933_Gain_5
SWeep_Rep			扫描模式
							AD5933_Fre_UP 	递增频率
							AD5933_Fre_Rep	重复频率
*/

//AD5933_Sweep(30000,200,200,AD5933_OUTPUT_2V,AD5933_Gain_1,AD5933_Fre_UP);
float AD5933_Sweep(float Fre_Begin, float Fre_UP, uint16_t UP_Num, uint16_t OUTPUT_Vatage, uint16_t Gain, uint16_t SWeep_Rep)
{
  uint8_t SValue[3], IValue[3], NValue[2], CValue[2];
  uint16_t buf = 0;
  Fre_To_Hex(Fre_Begin, SValue);
  Fre_To_Hex(Fre_UP, IValue);
  NValue[0] = UP_Num >> 8;
  NValue[1] = UP_Num;
#ifdef AD5933_MCLK_USE_OUT
  buf = OUTPUT_Vatage | Gain | SWeep_Rep | AD5933_OUT_MCLK;
#else
  buf = OUTPUT_Vatage | Gain | SWeep_Rep | AD5933_IN_MCLK;
#endif
  CValue[0] = buf >> 8;
  CValue[1] = buf;

  Scale_imp(SValue, IValue, NValue, CValue);
  return 0;
}
/*SValue[3]起始频率，IValue[3]频率增量，NValue[2]增量数，CValue[2]控制字，ki增益系数，Ps扫频为1重复为0*/

float Scale_imp(uint8_t *SValue, uint8_t *IValue, uint8_t *NValue, uint8_t *CValue)
{
  int i, j, AddrTemp;
  uint8_t Gain = ((~CValue[0]) & 0x01) ? 5 : 1;
  uint8_t SWeep_Rep = ((CValue[0] & 0xF0) == (AD5933_Fre_UP >> 8)) ? 1 : 0;
  uint8_t Mode = CValue[0] & 0x0f;
  uint8_t ReadTemp, realArr[2], imageArr[2];
  int32_t real, image;
  float magnitude;
  //                uint start_f[3]={0X33,0X26,0X17};
  //                uint inc_f[3]={0,0,0X21};
  //                uint num_f[2]={0,0XC8};
  //                uint control[2]={0XB1,0X00};
  //								CValue[0]=Mode|AD5933_Standby;
  j = 0;

  AddrTemp = AD5933_REG_STARTFREQ0; //初始化起始频率寄存器
  for (i = 0; i < 3; i++)
  {
    AD5933_WriteReg(AddrTemp, SValue[i]);
    AddrTemp++;
  }
  AddrTemp = AD5933_REG_STEPFREQ0; //初始化频率增量寄存器
  for (i = 0; i < 3; i++)
  {
    AD5933_WriteReg(AddrTemp, IValue[i]);
    AddrTemp++;
  }
  AddrTemp = AD5933_REG_STEPNUM0; //初始化频率点数寄存器
  for (i = 0; i < 2; i++)
  {
    AD5933_WriteReg(AddrTemp, NValue[i]);
    AddrTemp++;
  }
  //初始化控制寄存器，1011 0001 0000 0000待机模式，2V，一倍放大，内部时钟
  AddrTemp = AD5933_REG_CTRL0;
  //                for(i = 0;i <2;i++)
  {
    AD5933_WriteReg(AddrTemp, Mode | (AD5933_Standby >> 8));
    AddrTemp++;
    AD5933_WriteReg(AddrTemp, CValue[1]);
    AddrTemp++;
  }

  AD5933_WriteReg(AD5933_REG_CTRL0, Mode | (AD5933_SYS_Init >> 8)); //控制寄存器写入初始化频率扫描命令
  Delay_ms(10);
  AD5933_WriteReg(AD5933_REG_CTRL0, Mode | (AD5933_Begin_Fre_Scan >> 8)); //控制寄存器写入开始频率扫描命令
  while (1)
  {
    while (1)
    {
      AD5933_ReadReg(AD5933_REG_STATUS, &ReadTemp);
                                  //							ReadTemp=ReadTemp&0x07;
      if (ReadTemp & 0x02)
        break;
    }
    AD5933_ReadReg(AD5933_REG_REAL0, &realArr[0]);
    AD5933_ReadReg(AD5933_REG_REAL1, &realArr[1]);
    real = realArr[0] * 0x100 + realArr[1];
    
    AD5933_ReadReg(AD5933_REG_IMAG0, &imageArr[0]);
    AD5933_ReadReg(AD5933_REG_IMAG1, &imageArr[1]);
    image = imageArr[0] * 0x100 + imageArr[1];

    rads[j] = atan2(image, real) - 0.00143485062;

    if (real >= 0x8000) //计算实部的原码(除符号位外，取反加一)
    {
      real ^= 0xFFFF;
      real ^= 0x8000;
      real += 1;
      real ^= 0x8000;
    }
    if (image >= 0x8000) //计算虚部的原码(除符号位外，取反加一)
    {
      image ^= 0xFFFF;
      image ^= 0x8000;
      image += 1;
      image ^= 0x8000;
    }
    AD5933_Dat_Re[j] = real;
    AD5933_Dat_Im[j] = image;
    magnitude = sqrt(real * real + image * image); //模值计算
    resistance[j++] = 1 / (magnitude * Gain);                              //阻抗计算
    
    AD5933_ReadReg(AD5933_REG_STATUS, &ReadTemp);
    if (ReadTemp & 0x04)
      break;
    if (SWeep_Rep == 1)
      AD5933_WriteReg(AD5933_REG_CTRL0, CValue[0]); //控制寄存器写入增加频率（跳到下一个频率点)的命令
    else
      AD5933_WriteReg(AD5933_REG_CTRL0, CValue[0]); //控制寄存器写入重复当前频率点扫描
  }
  AD5933_WriteReg(AD5933_REG_CTRL0, 0XA1); //进入掉电模式
  return magnitude;
}

float AD5933_Get_Rs(void)
{
  float Rs, re, im;

  //	AD5933_Sweep(30000,200,2,AD5933_OUTPUT_2V,AD5933_Gain_1,AD5933_Fre_Rep);
  AD5933_Sweep(10000, 1, 40, AD5933_OUTPUT_2V, AD5933_Gain_1, AD5933_Fre_UP);
  Rs = Get_resistance(40);
  re = Rs * cos(rads[0]);
  im = Rs * sin(rads[0]);
  return Rs;
}
float AD5933_Dat_Cap(float Fre)
{
  float pp;
  //	float dat=3.1415926*2*Fre;
  float dat = 1;
  pp = 1000000 * 23.9999992 / (AD5933_Dat_Re[0] * dat);
  return pp;
}

float AD5933_Get_Cap(float Fre)
{
  //	float Cap,dat;
  //
  //	AD5933_Sweep(30000,1,40,AD5933_OUTPUT_2V,AD5933_Gain_1,AD5933_Fre_UP);
  //	Cap=DA5933_Get_Rs();
  //	dat=1/(Cap*30000*2*3.1415926/100000000000000);
  //	DA5933_Dat_Cap(30000);
  //
  //	return Cap;

  float Rs, re, im, cap;

  //	AD5933_Sweep(30000,200,2,AD5933_OUTPUT_2V,AD5933_Gain_1,AD5933_Fre_Rep);
  AD5933_Sweep(100000, 1, 20, AD5933_OUTPUT_2V, AD5933_Gain_1, AD5933_Fre_UP);
  Rs = Get_resistance(20);
  //	re=Rs*cos(rads[0]);
  im = Rs * sin(rads[0]);
  cap = 0.9442 / (2 * 3.1415926 * im / 10000000);
  return cap;
}
float AD5933_Get_L(float Fre)
{
  float L;
  float Rs, re, im;

  AD5933_Sweep(100000, 1, 20, AD5933_OUTPUT_2V, AD5933_Gain_1, AD5933_Fre_UP);
  Rs = Get_resistance(20);
  //	re=Rs*cos(rads[0]);
  im = Rs * sin(rads[0]);
  L = im * 1000 / (2 * 3.1415926);
  return L;
}
//---------------------------------------------------------------------------------------------------------------------
// 函数原形：void display(unsigned int re,unsigned int im)
// 功能描述：显示函数。
// 参数说明：unsigned int re,unsigned int im ,实部和虚部
// 返回值：无
//---------------------------------------------------------------------------------------------------------------------

/*
void display(signed int re,signed int im)
{
   unsigned char a[11],y=0,x=0,i=0;
   signed int  revalu=0,imvalu=0;
   float  valu=0;
   double xishu=0,regist;
   long  int ll;

   revalu=re;
   imvalu=im;  
   if(revalu&0x8000)           //把带符号的换算成无符号整数
   {
     revalu=(revalu-0x10000)*(-1);  
     x=1;
   }
   if(imvalu&0x8000)           //把带符号的换算成无符号整数
   {
     imvalu=(imvalu-0x10000)*(-1);
     y=1;
   }
   valu=sqrt(imvalu*imvalu+revalu*revalu);  //计算幅值
   switch(j)
   {
     case 0:
            if((valu<13000)&&(valu>8000))
            {
                xishu=(1/(96.86810));            //计算系数
                xishu=(1/(xishu*valu))*100000+0.5;   //计算阻值
                ll=xishu-250;
                i=1;
            }
            break;
     case 1:
            if((valu<13000)&&(valu>1350))
            {
                xishu=(1/(13.33726));            //计算系数
                xishu=(1/(xishu*valu))*1000000+0.5;   //计算阻值
                ll=xishu-100;
                i=1;
            }
            break;
     case 3:
            if((valu<12000)&&(valu>1050))
            {
                xishu=(1/(10.398994));            //计算系数
                xishu=(1/(xishu*valu))*10000000+0.5;   //计算阻值
                ll=xishu;
                i=1;
            }
            break;
     case 4:
            if((valu<12000)&&(valu>1000))
            {
                xishu=(1/(96.736596));            //计算系数
                xishu=(1/(xishu*valu))*10000000+0.5;   //计算阻值
                ll=xishu;
                i=1;
            }
            break;
     case 5:
            if((valu<2130)&&(valu>1060))
            {
                xishu=(1/(21.006));            //计算系数
               xishu=(1/(xishu*valu))*100000000+0.5;   //计算阻值
               ll=xishu;
                i=1;
            }
            break;
     case 6:
            if((valu<4800)&&(valu>800))
            {
                xishu=(1/(96.527914));            //计算系数
                xishu=(1/(xishu*valu))*100000000+0.5;   //计算阻值
                ll=xishu;
                i=1;
            }
            break;
    default: 
            break;
   }
   if(i==1)                //满足条件则显示阻值，相位
   {
     a[0]=ll/10000000+48;
     a[1]=ll%10000000/1000000+48;
     a[2]=ll%1000000/100000+48;
     a[3]=ll%100000/10000+48;
     a[4]=ll%10000/1000+48;
     a[5]='.';
     a[6]=ll%1000/100+48;
     a[7]=ll%100/10+48;
     a[8]=ll%100%10+48;
     a[9]='K';
     a[10]=0;
     LCD_WriteStr(4,2,a);
   
     regist=ll;
     if((y==0)&&(x==0))
     {
//计算并显示  atan2(float y, float x);  求y/x（弧度表示）的反正切值
       xishu=atan2(imvalu,revalu)*180/3.14+0.5;          
     }
     if((y==1)&&(x==0))
     {
        //计算并显示  atan2(float y, float x);  求y/x（弧度表示）的反正切值
       xishu=atan2((imvalu*(-1)),revalu)*180/3.14-0.5;         
       xishu=xishu+180;
     }
     if((y==1)&&(x==1))
     {
        //计算并显示  atan2(float y, float x);  求y/x（弧度表示）的反正切值
       xishu=atan2(imvalu,revalu)*180/3.14+0.5;          
       xishu=xishu+180;
     }
     if((y==0)&&(x==1)) 
     {
       xishu=atan2(imvalu,(revalu*(-1)))*180/3.14-0.5;          //计算并显示  atan2(float y, float x);  求y/x（弧度表示）的反正切值
       xishu=xishu+360;
     }
     if((ll<1000)&&(ll>400))                //相位误差修正
       ll=xishu-89.5;
     if((ll<10000)&&(ll>=1000))
       ll=xishu-(-0.0012237*regist+91.46)+0.5;
     if((ll<100000)&&(ll>=10000))
       ll=xishu-(-0.0001515*regist+91.016)+0.5;
     if((ll<1000000)&&(ll>=100000))
       ll=xishu-(-0.0000179*regist+91.806)+0.5;
     if((ll<=2000000)&&(ll>=1000000))
       ll=xishu-(-0.000009675*regist+93.789)+0.5;
     if((ll<12000000)&&(ll>2000000))
       ll=xishu-(-0.00000256*regist+95.444)+0.5;
     if(ll<0)                     //判断正负
     {
       ll=ll*(-1);
       a[0]='-';
       a[1]=ll/100+48;
       a[2]=ll%100/10+48;
       a[3]=ll%100%10+48;
       a[4]=0;
       LCD_WriteStr(4,3,a);
     }
     else
     {
     a[0]=' ';
     a[1]=ll/100+48;
     a[2]=ll%100/10+48;
     a[3]=ll%100%10+48;
     a[4]=0;
     LCD_WriteStr(4,3,a);
     }
   }
}

*/
