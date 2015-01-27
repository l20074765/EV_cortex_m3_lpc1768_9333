#include "..\config.h"
#include "GoodsAndKey.h"
#include "ChannelHandle.h"


/*********************************************************************************************
**函   数   名: delay
**函数功能说明：us延时函数
**函数传入参数：max：延时的时间
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
void delay(uint32_t max)
{
	while(max)
		max--;
}


/*********************************************************************************************
**函   数   名:	SelectKeyInit
**函数功能说明：选货按键初始化
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
void SelectKeyInit(void)
{
	//将引脚设置成输入
	FIO1DIR &= ~(1ul<<22);
	FIO1DIR &= ~(1ul<<27);
	FIO1DIR &= ~(1ul<<21);
	FIO1DIR &= ~(1ul<<23);
	FIO1DIR |= (1ul<<31);
	FIO1DIR |= (1ul<<19);
	FIO1DIR |= (1ul<<18);
	FIO1DIR |= (1ul<<20);
	FIO1SET |= (1ul<<18)|(1ul<<19)|(1ul<<20)|(1ul<<31);
	FIO1CLR |= (1ul<<21)|(1ul<<22)|(1ul<<23)|(1ul<<27);
}

/*********************************************************************************************
**函   数   名: IsSelectKeyIn
**函数功能说明：检测是否有选货按键进入
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：根据不同的键返回相应的键值，没有返回0
**********************************************************************************************/
uint8_t IsSelectKeyIn(void)
{
	static uint8_t keyInFlag[4]={0};

	if((FIO1PIN&(1ul<<27))==0x00000000)
	{
		keyInFlag[0]=1;
	}
	else
	{
		if(keyInFlag[0]==1)
		{
			keyInFlag[0]=0;
			Buzzer();
			return 0x04;	
		}
	}
	if((FIO1PIN&(1ul<<23))==0x00000000)
	{
		keyInFlag[1]=1;
	}
	else
	{
		if(keyInFlag[1]==1)
		{
			keyInFlag[1]=0;
			Buzzer();
			return 0x03;	
		}
	}
	if((FIO1PIN&(1ul<<22))==0x00000000)
	{
		keyInFlag[2]=1;
	}
	else
	{
		if(keyInFlag[2]==1)
		{
			keyInFlag[2]=0;
			Buzzer();
			return 0x02;	
		}
	}
	if((FIO1PIN&(1ul<<21))==0x00000000)
	{
		keyInFlag[3]=1;
	}
	else
	{
		if(keyInFlag[3]==1)
		{
			keyInFlag[3]=0;
			Buzzer();
			return 0x01;	
		}
	}
	return 0;
}


unsigned char UserSelectKey()
{
	unsigned char *Ukey,err;

	Ukey = (unsigned char *)OSQPend(g_KeyMsg,15,&err);
	//Trace("Ukey==%d\r\n",*Ukey);
	if(err==OS_NO_ERR)
	{
		return *Ukey;
	}
	else
		return 0;
}


unsigned char ReturnKey()
{
	static uint8_t ReKeyFlag = 0;
	FIO4DIR &= ~(1ul<<28);
	//FIO4SET |= (1ul<<28);

	if(FIO4PIN&(1ul<<28))
	{
		ReKeyFlag = 1;
	}
	else
	{
		if(ReKeyFlag==1)
		{
			ReKeyFlag=0;
			Buzzer();
			return 0x01;	
		}
	}
	return 0;
}







