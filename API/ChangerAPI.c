#include "..\config.h"


/*********************************************************************************************************
** Function name:     	Hopper_Init
** Descriptions:	    Hopper设备初始化函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Hopper_Init()
{
	stEvbHp[0].Price = SystemPara.HopperValue[0];
	stEvbHp[1].Price = SystemPara.HopperValue[1];
	stEvbHp[2].Price = 0;
	stEvbHp[0].MemoryAdd = ADD_HP1;
	stEvbHp[1].MemoryAdd = ADD_HP2;
	stEvbHp[2].MemoryAdd = ADD_HP3;
	//对结构链表中的三个Hp做基本配置
	stEvbHp[0].State = 0;
	stEvbHp[0].Add = 0x00;
	stEvbHp[0].Index = 1;
	stEvbHp[1].State = 0;
	stEvbHp[1].Add = 0x01;
	stEvbHp[1].Index = 2;
	stEvbHp[2].State = 0;
	stEvbHp[2].Add = 0x02;
	stEvbHp[2].Index = 3;
	if((stEvbHp[0].Price == stEvbHp[1].Price)&&(stEvbHp[0].Price == stEvbHp[2].Price))
		HpValueFlag = 5;
	else
	if((stEvbHp[1].Price == stEvbHp[2].Price)&&(stEvbHp[0].Price != stEvbHp[2].Price))
		HpValueFlag = 4;
	else
	if((stEvbHp[0].Price == stEvbHp[2].Price)&&(stEvbHp[0].Price != stEvbHp[1].Price))
		HpValueFlag = 3;		
	else
	if((stEvbHp[0].Price == stEvbHp[1].Price)&&(stEvbHp[0].Price != stEvbHp[2].Price))
		HpValueFlag = 2;	
	else
	if(((stEvbHp[0].Price != stEvbHp[1].Price)&&(stEvbHp[0].Price != stEvbHp[2].Price))&&(stEvbHp[2].Price != stEvbHp[1].Price))
		HpValueFlag = 1;	
	//1.根据价格的大小不同，对三个Hp进行逻辑高低排序
	Hopper_ValueListInit();
	ChangeInitHopperState();
}

/*********************************************************************************************************
** Function name:     	Hopper_Distribute
** Descriptions:	    Hopper设备配币函数
** input parameters:    nMoney:需要找币的金额
						tHp1:000地址Hopper的找币枚数
						tHp2:010地址Hopper的找币枚数
						tHp3:100地址Hopper的找币枚数
** output parameters:   无
** Returned value:      1：配币成功；0：配备失败
*********************************************************************************************************/
uint8_t Hopper_Distribute(uint32_t nMoney,uint8_t *tHp1,uint8_t *tHp2,uint8_t *tHp3)
{
	uint8_t Count = 0;
	uint32_t i,nMoney1=0,nMoney2=0;
	uint8_t ln1=0,ln2=0,ln3=0;

	for(i=0;i<3;i++)
	{
		if(stEvbHp[i].Num == 1)
		{
			ln1 = i;
		}
		else
		if(stEvbHp[i].Num == 2)
		{
			ln2 = i;
		}
		else
		if(stEvbHp[i].Num == 3)
		{
			ln3 = i;
		}
	}
	Trace("\r\nMidddistribute.1..%d..%d..%d..%d\r\n",stEvbHp[0].Price,stEvbHp[0].State,stEvbHp[0].DevBadFlag,stEvbHp[0].Num);
	Trace("\r\nMidddistribute.2..%d..%d..%d..%d\r\n",stEvbHp[1].Price,stEvbHp[1].State,stEvbHp[1].DevBadFlag,stEvbHp[1].Num);
	Trace("\r\nMidddistribute.3..%d..%d..%d..%d\r\n",stEvbHp[2].Price,stEvbHp[2].State,stEvbHp[2].DevBadFlag,stEvbHp[2].Num);
	Trace("\r\nMidddistribute.1.1.%d..%d..%d..%d\r\n",stEvbHp[ln1].Price,stEvbHp[ln1].State,stEvbHp[ln1].DevBadFlag,stEvbHp[ln1].Num);
	Trace("\r\nMidddistribute.2.2.%d..%d..%d..%d\r\n",stEvbHp[ln2].Price,stEvbHp[ln2].State,stEvbHp[ln2].DevBadFlag,stEvbHp[ln2].Num);
	Trace("\r\nMidddistribute.3.3.%d..%d..%d..%d\r\n",stEvbHp[ln3].Price,stEvbHp[ln3].State,stEvbHp[ln3].DevBadFlag,stEvbHp[ln3].Num);
	*tHp1 = 0;
	*tHp2 = 0;
	*tHp3 = 0;
	//如果状态不为故障或者不可服务状态则进行配币
	if((stEvbHp[ln3].State != 3)&&(stEvbHp[ln3].State != 2)/*&&(stEvbHp[ln3].DevBadFlag < 2)*/)
	{
		Count = nMoney/stEvbHp[ln3].Price;
		nMoney1 = nMoney - Count*stEvbHp[ln3].Price;
		if(nMoney1 > nMoney)
			return 0;
		*tHp1 = Count;
	}
	else
	{
		*tHp1 = 0;
		nMoney1 = nMoney;
	}
	//如果状态不为故障或者不可服务状态则进行配币
	if((stEvbHp[ln2].State != 3)&&(stEvbHp[ln2].State != 2)/*&&(stEvbHp[ln2].DevBadFlag < 2)*/)
	{
		Count = nMoney1/stEvbHp[ln2].Price;
		nMoney2 = nMoney1 - Count*stEvbHp[ln2].Price;
		if(nMoney2 > nMoney1)
			return 0;
		*tHp2 = Count;
	}
	else
	{
		*tHp2 = 0;
		nMoney2 = nMoney1;
	}
	//如果状态不为故障或者不可服务状态则进行配币
	if((stEvbHp[ln1].State != 3&&(stEvbHp[ln1].State != 2))/*&&(stEvbHp[ln1].DevBadFlag < 2)*/)
	{
		Count = nMoney2/stEvbHp[ln1].Price;
		nMoney1 = nMoney2 - Count*stEvbHp[ln1].Price;
		if(nMoney1 > nMoney2)
			return 0;
		*tHp3 = Count;
	}
	else
	{
		*tHp3 = 0;
	}
	if((*tHp1==0)&&(*tHp2==0)&&(*tHp3==0))
		return 0;
	Trace("\r\nMidddistribute.11..%d..%d..%d\r\n",stEvbHp[ln1].Price,stEvbHp[ln1].State,stEvbHp[ln1].DevBadFlag);
	Trace("\r\nMidddistribute.22..%d..%d..%d\r\n",stEvbHp[ln2].Price,stEvbHp[ln2].State,stEvbHp[ln2].DevBadFlag);
	Trace("\r\nMidddistribute.33..%d..%d..%d\r\n",stEvbHp[ln3].Price,stEvbHp[ln3].State,stEvbHp[ln3].DevBadFlag);
	return 1;
}



