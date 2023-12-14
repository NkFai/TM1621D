#include"user.h"

const uint8_t Sum_LM2[10]={0x0A,0x00,0x06,0x04,0x0C,0x0C,0x0E,0x00,0x0E,0x0C};//0-9---������0-99�����ַ11������[0]
const uint8_t Sum_LM0[10]={0x0B,0x01,0x07,0x05,0x0D,0x0D,0x0F,0x01,0x0F,0x0D};//0-9---������100�����ַ11������[0]

const uint8_t Sum_LM1[10]={0x0F,0x06,0x0D,0x0F,0x06,0x0B,0x0B,0x0E,0x0F,0x0F};

const uint8_t Sum_SW0[10]={0x0F,0x06,0x0B,0x0F,0x06,0x0D,0x0D,0x07,0x0F,0x0F};
const uint8_t Sum_SW1[10]={0x0B,0x01,0x0D,0x05,0x07,0x07,0x0F,0x01,0x0F,0x07};
// uint8_t Write_bit;
// uint8_t Write_Data;
uint8_t Cmd;
uint8_t LM_DATA1_RAM,LM_DATA2_RAM;
uint8_t SW_Addr;
uint8_t T100ms_cnt = 0;


void DelayUs(unsigned char Time)
{
	unsigned char a;
	Time>>=1;		//8M--2T
//	Time>>=2;		//4M--2T
//	Time>>=3;		//2M--2T
//	Time>>=4;		//1M--2T
	for(a=0;a<Time;a++)
	{
		//NOP();
	}
} 
void DelayMs(u16 Time)
{
	u16 a,b;
	for(a=0;a<Time;a++)
	{
	 	//CLRWDT(); 		
		for(b=0;b<5;b++)
		{
		 	DelayUs(198); 	
		}
	}
}                                                                                                                            

void Delay5us_TM()
{
	unsigned char i;
	i = 6;
	while (--i);
}

/********************�Ӹ�λд������*************************/
void Write_Data_H(unsigned char Data, unsigned char Cnt)
{
    uchar i;
    for(i=0; i<Cnt; i++)
    {
        WR_IO = 0;
        NOP();
        NOP();
        NOP();
        NOP();
        
        if(Data & 0x80)
        {
            DATA_IO = 1;		//��1
        }	
        else
        {
            DATA_IO = 0;		//��0
        }	
        NOP();
        NOP();
        NOP();
        NOP();

        WR_IO = 1;
        Data <<= 1;
    }
    WR_IO = 0;
    DATA_IO = 0;
}
/********************�ӵ�λд������*************************/
void Write_Data_L(unsigned char Data, unsigned char Cnt)
{
    uchar i;
    for(i=0; i<Cnt; i++)
    {
        WR_IO = 0;

        if(Data & 0x01)		//
        {
            DATA_IO = 1;		//��1
        }	
        else
        {
            DATA_IO = 0;		//��0
        }	
        NOP();
        NOP();
        NOP();
        NOP();

        WR_IO = 1;
        Data >>= 1;
    }

    WR_IO = 0;
    DATA_IO = 0;
}

/********************д���������*************************/
void WriteCmd(unsigned char Cmd)
{
    CS_IO = 0;
    NOP();
    NOP();
    NOP();
    NOP();

    Write_Data_H(0x80,4);   //д�������־100---0B1000 00000000 (��4bit�����룬ǰ8bitָ�����)
    Write_Data_H(Cmd,8);    //д����������

    CS_IO = 1;

    DelayUs(1);
}

/*********ָ����ַд�����ݣ�ʵ��д���4λ************/
void WriteOneData(unsigned char Addr, unsigned char Data)  
{
    CS_IO = 0;

    Write_Data_H(0xa0,3);   //д�����ݱ�־101
    Write_Data_H(Addr<<2,6);    //д���ַ����
    Write_Data_L(Data,4);

    CS_IO = 1;
    NOP();
    NOP();
    NOP();
    NOP();
}

/*********����д�뷽ʽ��ÿ������Ϊ8λ��д������************/
void WriteAllData(unsigned char Addr, unsigned char *p, unsigned char cnt)
{
    uchar i;
    CS_IO = 0;

    Write_Data_H(0xa0,3);       //д���־101
    Write_Data_H(Addr<<2,6);

    for(i=0; i<cnt; i++)
    {
        Write_Data_L(*p,8);
        p++;
    }

    CS_IO = 1;
    NOP();
    NOP();
    NOP();
    NOP();
}

/*******************TM1621��ʼ��**********************/
void TM1621D_Init(void)
{
    CS_IO = 1;
    WR_IO = 1;
    DATA_IO = 1;
    

    NOP();
    DelayUs(100);
    WriteCmd(BIAS);         //1/3ƫѹ 4������  0x52
    WriteCmd(RC);           //�ڲ�RC��        0x30
    WriteCmd(SYSDIS);        //��ϵͳ������LCDƫѹ������  0x00
    WriteCmd(SYSEN);         //��ϵͳ����  0x02
    WriteCmd(LCDON);         //��LCDƫѹ    0x06
}

// void Close_LCD(void)
// {
//     WriteCmd(SYSDIS);         //��ϵͳ������LCDƫѹ������
//     WriteCmd(LCDOFF);         //�ر�LCDƫѹ
// }

void TM1621D_Display()
{
    uint8_t Temp;
    if(F_DisUpdate)
    {
        F_DisUpdate = 0;

        if(bONOFF)
        {
            /*�����3---�̶�Ϊ0*/
            WriteOneData(15,Sum_SW0[0]);  //0x0F Sum_SW0[0]
            WriteOneData(16,Sum_SW1[0]);  //0x0B Sum_SW1[0]

            /*�����4---�̶�Ϊ0*/
            WriteOneData(17,Sum_SW0[0]);  //0x0F Sum_SW0[0]
            WriteOneData(18,Sum_SW1[0]);  //0x0B Sum_SW1[0]

            /*�����6*/
            Temp = (SwLevel+SW_Base)/10%10;
            WriteOneData(21,Sum_SW0[Temp]);  //0x0F Sum_SW0[0]
            WriteOneData(22,Sum_SW1[Temp]);  //0x0B Sum_SW1[0]

            /*�����5*/
            Temp = (SwLevel+SW_Base)%10;
            WriteOneData(19,Sum_SW0[Temp]);  //0x0F Sum_SW0[0]
            WriteOneData(20,Sum_SW1[Temp]);  //0x0B Sum_SW1[0]

            if(LmLevel >= 100)
            {
                /*�����1*/
                WriteOneData(11,Sum_LM0[0]);  //0x0B Sum_LM0[0]
                WriteOneData(12,Sum_LM1[0]);  //0x0F Sum_LM1[0]

                /*�����2*/
                WriteOneData(13,Sum_LM0[0]);  //0x0B Sum_LM0[0]
                WriteOneData(14,Sum_LM1[0]);  //0x0F Sum_LM1[0]
            } 
            else
            {
                /*�����1*/
                Temp = LmLevel/10%10;
                WriteOneData(11,Sum_LM2[Temp]);  //0x0B Sum_LM0[0]
                WriteOneData(12,Sum_LM1[Temp]);  //0x0F Sum_LM1[0]

                /*�����2*/
                Temp = LmLevel%10;
                WriteOneData(13,Sum_LM0[Temp]);  //0x0B Sum_LM0[0]
                WriteOneData(14,Sum_LM1[Temp]);  //0x0F Sum_LM1[0]
            }
        }
        else
        {
            LCD_IO = 0;
            OFF_Light();
        }
    }
}
