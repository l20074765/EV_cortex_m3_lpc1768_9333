#include "..\config.h"
#include "..\API\Public.h"
#include "Menu.h"
#include "MAINTAIN.h"

#define LANGUAGECOUNT			2
#define HPMENU				(1)				//Hopper面值设置菜单
#define BILLMENU			(2)				//纸币器面值设置菜单
#define COINMENU			(3)				//硬币器面值设置菜单
uint8_t stopKey[3]={0};

typedef struct 
{
	//密码输入
	char *EnterPassword[LANGUAGECOUNT];
	//货道管理
	char *ColumnManage[LANGUAGECOUNT];
	//系统参数设置
	char *SysParaSet[LANGUAGECOUNT];
	//设备管理
	char *DeiveManage[LANGUAGECOUNT];
	//交易记录
	char *TradeRecord[LANGUAGECOUNT];
}UI_MENUPRINTF;

const UI_MENUPRINTF UI_MenuPinrtf=
{
	{
		"请输入密码:",
		"Pls Enter Password:"
	},
	{
		"1.货道管理",
		"1.Column Manage"
	},	
	{
		"2.系统参数设置",
		"2.System Param Set"
	},	
	{
		"3.设备管理",
		"3.Device Manage"
	},	
	{
		"4.交易记录",
		"4.Trade Record"
	}	
};

//输入密码
unsigned char MenuInputSecritCode()
{
	unsigned char Secrit[6]={0x00},Index=0;
	unsigned char i=0,key=0xff;
	unsigned char SecritCode[6]={0x01,0x02,0x03,0x04,0x05,0x06};
	
	while(1)
	{
		key = GetKeyInValue();
		switch(key)
		{
			case 0x01:
				if(Index==0)
				{
					Secrit[Index] = 0x01;
					Index++;
				}
				break;
			case 0x02:
				if(Index==1)
				{
					Secrit[Index] = 0x02;
					Index++;
				}
				break;
			case 0x03:
				if(Index==2)
				{
					Secrit[Index] = 0x03;
					Index++;
				}
				break;
			case 0x04:
				if(Index==3)
				{
					Secrit[Index] = 0x04;
					Index++;
				}
				break;
			case 0x05:
				if(Index==4)
				{
					Secrit[Index] = 0x05;
					Index++;
				}
				break;
			case 0x06:
				if(Index==5)
				{
					Secrit[Index] = 0x06;
					Index++;
				}
				break;
			case 0x0e:
				return 0;
			case 0x0f:
				if(Index == 6)
				{
					for(i=0;i<6;i++)
					{
						if(Secrit[i]!=SecritCode[i])
							break;
					}
					if(i==6)
						return 1;
					else
						return 0;
				}
				else
				{
					Index = 0;
					memset(Secrit,0x00,sizeof(Secrit));
				}
				break;
			default:
				break;
		}
	}	
}

//
void UI_EnterCode()
{
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,7,0,0,UI_MenuPinrtf.EnterPassword[0]);
	OSTimeDly(300);
	LCDPrintf(8,5,0,0,UI_MenuPinrtf.ColumnManage[0]);
	LCDPrintf(8,7,0,0,UI_MenuPinrtf.SysParaSet[0]);
	LCDPrintf(8,9,0,0,UI_MenuPinrtf.DeiveManage[0]);
	LCDPrintf(8,11,0,0,UI_MenuPinrtf.TradeRecord[0]);
}



/*********************************************************************************************************
** Function name:       ColumnParamSet
** Descriptions:        货道参数设置
** input parameters:    num：第几个货道
						type:1——单价设置    2——存货余量设置     3——状态更改
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ColumnParamSet(unsigned char type,unsigned char num)
{
 #if 0
	uint32_t ColumnAdd = 0x00000000;
	uint32_t value = 0;

	if(type == 1)
	{
		ColumnAdd = ADD_CHANNELPRICE+(num-1)*4;		
	}
	else
	if(type == 2)
	{
		ColumnAdd = ADD_CHANNELCOUNT+(num-1)*4;
	}
	else
	if(type == 3)
	{
		ColumnAdd = ADD_CHANNELSTATE+(num-1)*4;
	}
	value = GetMemoryData(ColumnAdd);
	if(type == 1)
	{
		if(value<10000)
		{
			DisplayDecimalNumberWithDot(value,SYSPara.nPoinValue);
		}
		else
		{
			DisplayDecimalNumberWithDot(value/10000,0);
			OSTimeDly(150);
			DisplayDecimalNumberWithDot(value%10000,SYSPara.nPoinValue+3);	
		}
	}
	else
		DisplayDecimalNumberWithDot(value,0);
	SetValue(ColumnAdd);
#endif
}


//货道参数设置菜单
void ColumnParaSetMenu(unsigned char type)
{
	uint8_t iKey = 0xff,flag = 0;

	DosplaySettingMemu(0,type);													 
	while(1)
	{
		iKey = GetKeyInValue();
		switch(iKey)
		{
			case 0x01:
				DosplaySettingMemu(11,type);
				flag = 1;
				break;
			case 0x02:
				DosplaySettingMemu(12,type);
				flag = 2;
				break;
			case 0x03:
				DosplaySettingMemu(13,type);
				flag = 3;
				break;
			case 0x04:
				DosplaySettingMemu(14,type);
				flag = 4;
				break;
			case 0x0f:
				ColumnParamSet(type,flag);
				DosplaySettingMemu(0,type);
				flag = 0;
				break;
			case 0x0e:
				return;
			default:
				break;
		}
	}	
}


//货道测试
void ColumnTest()
{
	uint8_t nkey,Cstate=0x00,column=0x01;
	while(1)
	{
		nkey = GetKeyInValue();
		if(nkey==0x0f)
			break;
		else
		if(nkey==0x0e)
			return;
	}
	DisplayDecimalNumberWithDot(0,0);
	while(1)
	{
		nkey = GetKeyInValue();
		if(nkey==0xff)
		{
			nkey = UserSelectKey();
			Trace("nkey==%d\r\n",nkey);
			if(nkey==0x00)
				nkey = 0xff;
		}
		switch(nkey)
		{
			case 0x00:
				while(1)
				{
					nkey = GetKeyInValue();
					if(nkey==0x0e)
						break;
					if(fromSelectNoGetLogicNo(1,column))
					{
						DisplayDecimalNumberWithDot(column,0);
						OSTimeDly(250);
						Cstate = hd_outgoods_test(column);
						//ChannelhandleProcess(COLUMN_OUTGOODS,column,&Cstate);
						OSTimeDly(50);
						//Trace("Check..IsEmpty..\r\n");
						//ChannelhandleProcess(COLUMN_CHECK_RST,column,&Cstate);
						
						if(Cstate != 1)
							DispLayExceptionInfomation(22);
						else
							DispLayExceptionInfomation(21);	
						column++;
					}
					else
						column = 1;
					Trace("column=%d\r\n",column);
					OSTimeDly(300);
					DisplayDecimalNumberWithDot(0,0);
				}
				break;
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
			case 0x08:
				if(!fromSelectNoGetLogicNo(1,nkey))
					break;
				DisplayDecimalNumberWithDot(nkey,0);
				OSTimeDly(250);
				Cstate = hd_outgoods_test(nkey);
				//ChannelhandleProcess(COLUMN_OUTGOODS,nkey,&Cstate);
				//OSTimeDly(50);
				//Trace("Check..IsEmpty..\r\n");
				//ChannelhandleProcess(COLUMN_CHECK_RST,nkey,&Cstate);
				if(Cstate != 1)
					DispLayExceptionInfomation(22);
				else
					DispLayExceptionInfomation(21);	
				OSTimeDly(300);
				DisplayDecimalNumberWithDot(0,0);
				break;
			case 0x0e:
				return;
		}
	}	
}

//硬币器测试
void CoinDevTest()
{
	uint8_t nkey;
	uint32_t moeny=0,showAmount;


	DisplayDecimalNumberWithDot(0,0);
	deviceEnable(TRUE);
	while(1)
	{
		nkey = GetKeyInValue();
		if(nkey==0x0e)
			break;
		moeny = getCoinMoney();
		if(moeny)
		{
			showAmount += moeny;
			DisplayDecimalNumberWithDot(amountTurnByPoint(showAmount),SYSPara.nPoinValue);
		}	
		OSTimeDly(10);
	}
	deviceEnable(FALSE);
}


/*********************************************************************************************************
** Function name:       DispHpCoinOutSum
** Descriptions:        查看Hp的出币记录
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispHpCoinOutSum()
{
	uint8_t nkey,enterSubMenu = 0;
	uint16_t mData = 0;
	DisplayDecimalNumberWithDot(0,0);
	while(1)
	{
		nkey = GetKeyInValue();
		switch(nkey)
		{
			case 0x01:
				mData = amountTurnByPoint(stTotalTrade.Hopper1DispSum);
				DisplayDecimalNumberWithDot(mData,0);
				enterSubMenu = 1;
				break;
			case 0x02:	
			 	mData = amountTurnByPoint(stTotalTrade.Hopper2DispSum);
				DisplayDecimalNumberWithDot(mData,0);
				enterSubMenu = 1;
				break;
			case 0x03:
				mData = amountTurnByPoint(stTotalTrade.Hopper3DispSum);
				DisplayDecimalNumberWithDot(mData,0);
				enterSubMenu = 1;
				break;
			case 0x0e:
				return;
			default:
				enterSubMenu = 0;
				break;
		}
		while(enterSubMenu)
		{
			nkey = GetKeyInValue();
			if(nkey == 0x0e)
			{
				DisplayDecimalNumberWithDot(0,0);
				enterSubMenu = 0;
				break;
			}	
			OSTimeDly(4);
		}	
		OSTimeDly(4);
	}
	
}


/*********************************************************************************************************
** Function name:       DispBillRecvMoney
** Descriptions:        查看纸币器的收币记录
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispBillRecvMoney()
{
	uint8_t nkey=0xff;
	uint32_t mData = 0;
	mData =  amountTurnByPoint(stTotalTrade.BillMoneySum);
	if(mData>10000)
	{
		DisplayDecimalNumberWithDot(mData/10000,0);
		OSTimeDly(500);
		DisplayDecimalNumberWithDot(mData%10000,SYSPara.nPoinValue+3);
	}
	else
		DisplayDecimalNumberWithDot(mData,SYSPara.nPoinValue);	
	while(1)
	{
		nkey = GetKeyInValue();
		if(nkey == 0x0e)
			return;
		OSTimeDly(4);
	}
}


/*********************************************************************************************************
** Function name:       hopperTest
** Descriptions:        Hp测试
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void hopperTest(unsigned char menu)
{
	uint8_t nKey = 0xff,returnFlag = 0,enterSubMenu = 0,hopperNo = 0;
	uint8_t outcout = 0,state=0;
	displayUserMemuLED(menu, 0);
	while(1)
	{
		nKey = GetKeyInValue();
		switch(nKey)
		{
			case 1:case 2:case 3:
				hopperNo = nKey;
				displayUserMemuLED(menu, hopperNo);
				break;
			case 0x0e:
				if(hopperNo)
					hopperNo = 0;
				else
					returnFlag = 1;
				displayUserMemuLED(menu, hopperNo);
				break;
			case 0x0f:
				if(hopperNo)
					enterSubMenu = 1;
				break;
			default:
				break;
		}
		if(enterSubMenu)
		{
			// HP01
			led_display(ledTab[LED_H],
						ledTab[LED_P],
						ledTab[hopperNo],
						ledTab[0]);
			OSTimeDly(150);
			if(hopperTestOutput(5,hopperNo - 1))
			{
				outcout = 5;
				DisplayDecimalNumberWithDot(outcout,0);
				OSTimeDly(250);
				
			}
			state = stHopper[hopperNo - 1].state;
			DisplayDecimalNumberWithDot(state,0);
			if(state != 0)
			{
				led_display(ledTab[LED_H],
							ledTab[LED_P],
							ledTab[hopperNo],
							ledTab[LED_E]);
			}	
			else
			{
				led_display(ledTab[LED_H],
						ledTab[LED_P],
						ledTab[hopperNo],
						ledTab[0]);
			}
				
			OSTimeDly(250);	
			displayUserMemuLED(menu, 0);
			hopperNo = 0;
			enterSubMenu = 0;

			
		}
		
		if(returnFlag)
		{
			return;
		}
		OSTimeDly(5);
		
	}

	
}


/*********************************************************************************************************
** Function name:       hopperClear
** Descriptions:          Hp清斗
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void hopperClear(unsigned char menu)
{
	uint8_t nKey = 0xff,returnFlag = 0,enterSubMenu = 0,hopperNo = 0;
	unsigned short outCount;
	displayUserMemuLED(menu, 0);
	while(1)
	{
		nKey = GetKeyInValue();
		switch(nKey)
		{
			case 1:case 2:case 3:
				hopperNo = nKey;
				displayUserMemuLED(menu, hopperNo);
				break;
			case 0x0e:
				if(hopperNo)
					hopperNo = 0;
				else
					returnFlag = 1;
				displayUserMemuLED(menu, hopperNo);
				break;
			case 0x0f:
				if(hopperNo)
					enterSubMenu = 1;
				break;
			default:
				break;
		}
		if(enterSubMenu)
		{
			led_display(ledTab[LED_H],
						ledTab[LED_P],
						ledTab[hopperNo],
						ledTab[0]);
			OSTimeDly(150);
			while(hopperTestOutput(10,hopperNo - 1))
			{
				outCount += 10;
				DisplayDecimalNumberWithDot(outCount,0);
				nKey = GetKeyInValue();
				if(nKey == 0x0e)
					break;
			}
			outCount += stHopper[hopperNo - 1].LastOutPutCount;
			DisplayDecimalNumberWithDot(outCount,0);			
			OSTimeDly(200);	
			displayUserMemuLED(menu, 0);
			outCount = 0;
			hopperNo = 0;
			enterSubMenu = 0;

			
		}
		
		if(returnFlag)
		{
			return;
		}
		OSTimeDly(5);
		
	}

	
}




/*********************************************************************************************************
** Function name:       HpTest
** Descriptions:        Hp测试
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void HpTest()
{
	uint8_t nKey = 0xff;
	uint8_t outcout = 0,state=0;
	

	DisplayDecimalNumberWithDot(0,0);
	while(1)
	{
		nKey = GetKeyInValue();
		if(nKey == 0x01)
		{
			DispLayExceptionInfomation(10);
			OSTimeDly(150);
			if(hopperTestOutput(5,0x00))
			{
				outcout = 5;
				DisplayDecimalNumberWithDot(outcout,0);
				OSTimeDly(250);
				state = stHopper[0].state;
				DisplayDecimalNumberWithDot(state,0);
				if(state != 0)
					DispLayExceptionInfomation(0);
				else
					DispLayExceptionInfomation(10);
			}
			
			OSTimeDly(250);	
			DisplayDecimalNumberWithDot(0,0);
					
		}
		else
		if(nKey == 0x02)
		{
			DispLayExceptionInfomation(11);
			OSTimeDly(150);
			if(hopperTestOutput(5,0x01))
			{
				outcout = 5;
				DisplayDecimalNumberWithDot(outcout,0);
				OSTimeDly(250);
				state = stHopper[1].state;
				DisplayDecimalNumberWithDot(state,0);
				if(state != 0)
					DispLayExceptionInfomation(1);
				else
					DispLayExceptionInfomation(11);
			}
			
			OSTimeDly(250);	
			DisplayDecimalNumberWithDot(0,0);
		}
		else
		if(nKey == 0x03)
		{
			DispLayExceptionInfomation(12);
			OSTimeDly(150);
			if(hopperTestOutput(5,0x02))
			{
				outcout = 5;
				DisplayDecimalNumberWithDot(outcout,0);
				OSTimeDly(250);
				state = stHopper[2].state;
				DisplayDecimalNumberWithDot(state,0);
				if(state != 0)
					DispLayExceptionInfomation(2);
				else
					DispLayExceptionInfomation(12);
			}
			
			OSTimeDly(250);
			DisplayDecimalNumberWithDot(0,0);
		}
		else
		if(nKey == 0x0e)
			break;
	}
}



/*********************************************************************************************************
** Function name:       ClearHp
** Descriptions:        清除Hp中的余币
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ClearHp()
{
	uint8_t nKey = 0xff;
	uint8_t outcout = 0;
	unsigned char i;
	DisplayDecimalNumberWithDot(0,0);
	while(1)
	{
		nKey = GetKeyInValue();
		if(nKey == 0x01)
		{
			outcout = 0;
			DisplayDecimalNumberWithDot(1,0);
			OSTimeDly(150);
			DisplayDecimalNumberWithDot(0,0);
			while(hopperOutput(5,stHopper[0].addr))
			{
				outcout += 5;
				DisplayDecimalNumberWithDot(outcout,0);
			}
			outcout += stHopper[i].LastOutPutCount;
			DisplayDecimalNumberWithDot(outcout,0);
			OSTimeDly(500);
			DisplayDecimalNumberWithDot(0,0);
				
		}
		else if(nKey == 0x02)
		{
			outcout = 0;
			DisplayDecimalNumberWithDot(2,0);
			OSTimeDly(150);
			DisplayDecimalNumberWithDot(0,0);
			while(hopperOutput(5,stHopper[1].addr))
			{
				outcout += 5;
				DisplayDecimalNumberWithDot(outcout,0);
			}
			outcout += stHopper[i].LastOutPutCount;
			DisplayDecimalNumberWithDot(outcout,0);
			OSTimeDly(500);
			DisplayDecimalNumberWithDot(0,0);
				
		}
		else if(nKey == 0x03)
		{
			outcout = 0;
			DisplayDecimalNumberWithDot(3,0);
			OSTimeDly(150);
			DisplayDecimalNumberWithDot(0,0);
			while(hopperOutput(5,stHopper[2].addr))
			{
				outcout += 5;
				DisplayDecimalNumberWithDot(outcout,0);
			}
			outcout += stHopper[i].LastOutPutCount;
			DisplayDecimalNumberWithDot(outcout,0);
			OSTimeDly(500);
			DisplayDecimalNumberWithDot(0,0);
		}
		else
		if(nKey == 0x0e)
			break;
	}	
}


/*********************************************************************************************************
** Function name:       SeeDebtMoney
** Descriptions:        查看欠款金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SeeDebtMoney()
{
	uint8_t nkey;
	uint32_t setValue=0;


	setValue = amountTurnByPoint(stTotalTrade.iouAmount);
	if(setValue>10000)
	{
		DisplayDecimalNumberWithDot(setValue/10000,0);
		OSTimeDly(300);
		DisplayDecimalNumberWithDot(setValue%10000,SYSPara.nPoinValue +3);
	}
	else
		DisplayDecimalNumberWithDot(setValue,SYSPara.nPoinValue);
	while(1)
	{	
		nkey = GetKeyInValue();
		if(nkey==0x0e)
			break;
		OSTimeDly(5);
	}
}



/*********************************************************************************************************
** Function name:       dispCoinRecvMony
** Descriptions:        硬币器收币金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void dispCoinRecvMony(void)
{
	uint8_t nkey=0xff;
	uint32_t mData = 0;
	mData =  amountTurnByPoint(stTotalTrade.CoinMoneySum);
	if(mData>10000)
	{
		DisplayDecimalNumberWithDot(mData/10000,0);
		OSTimeDly(500);
		DisplayDecimalNumberWithDot(mData%10000,SYSPara.nPoinValue+3);
	}
	else
		DisplayDecimalNumberWithDot(mData,SYSPara.nPoinValue);	
	while(1)
	{
		nkey = GetKeyInValue();
		if(nkey == 0x0e)
			return;
		OSTimeDly(4);
	}
}




/*********************************************************************************************************
** Function name:       dispChangerMoney
** Descriptions:        找零总金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void dispChangerMoney(void)
{
	uint8_t nkey=0xff;
	uint32_t mData = 0;
	mData =  amountTurnByPoint(stTotalTrade.DispenceSum);
	if(mData>10000)
	{
		DisplayDecimalNumberWithDot(mData/10000,0);
		OSTimeDly(500);
		DisplayDecimalNumberWithDot(mData%10000,SYSPara.nPoinValue+3);
	}
	else
		DisplayDecimalNumberWithDot(mData,SYSPara.nPoinValue);	
	while(1)
	{
		nkey = GetKeyInValue();
		if(nkey == 0x0e)
			return;
		OSTimeDly(4);
	}
}




/*********************************************************************************************************
** Function name:       dispHopperChangeSum
** Descriptions:       hopper找零数量
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void dispHopperChangeSum(unsigned char hopperNo)
{
	uint8_t nkey=0xff;
	uint32_t mData = 0;
	if(hopperNo == 1)
		mData = stTotalTrade.Hopper1DispSum;
	else if(hopperNo == 2)
		mData = stTotalTrade.Hopper2DispSum;
	else if(hopperNo == 3)
		mData = stTotalTrade.Hopper3DispSum;
	else 
		return;
	
	DisplayDecimalNumberWithDot(mData,0);	
	
	while(1)
	{
		nkey = GetKeyInValue();
		if(nkey == 0x0e)
			return;
		OSTimeDly(4);
	}
}



/*********************************************************************************************************
** Function name:       ledClearTrade
** Descriptions:         清除交易记录
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ledClearTrade(void)
{
	unsigned char num[4] = {0},key,returnFlag = 0,enterSub = 0,index =0;
	led_dispaly_mima(0,NULL);
	while(1)
	{
		key = GetKeyInValue();
		switch(key)
		{
			case 1:case 2:case 3:case 4://密码 1234
				num[index++] = key;
				led_dispaly_mima(index,num);
				break;
			case 0x0e:
				if(index)
					index = 0;
				else
					returnFlag = 1;
				break;
			case 0x0f:
				enterSub = 1;
				index= 0;
				break;
		}
		if(enterSub)
		{
			if(num[0] == 1 && num[1] == 2 && num[2] == 3 && num[3] == 4)
			{
				//led_dispaly_mima(10,NULL);
				led_display(ledTab[LED_C],
							ledTab[LED_L],
							ledTab[LED_E],
							ledTab[LED_A]);
				clearTradeInfo();
				OSTimeDly(200);
				led_dispaly_off();
				OSTimeDly(200);
				led_dispaly_mima(10,NULL);
				OSTimeDly(200);
				enterSub = 0;
				return;
				
			}
			else
			{
				led_dispaly_off();
				OSTimeDly(200);
				led_dispaly_mima(0,NULL);
				enterSub = 0;
			}
		}

		if(returnFlag)
			return;
		
		OSTimeDly(10);
		
	}
}


/*********************************************************************************************************
** Function name:       vmcChangerUserMenu
** Descriptions:        兑零机用户菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char vmcChangerUserMenu(void)
{
	uint8_t nkey,subMenu = 0,returnFlag = 0,enterSubMenu = 0;

	//进入维护模式，发送禁能纸币器命令
	deviceEnable(FALSE);
	OSTimeDly(200);
	print_log("vmcChangerUserMenu...\r\n");
	displayUserMemuLED(0,0);
	while(1)
	{
		nkey = GetKeyInValue();
		switch(nkey)
		{
			case 1: case 2: case 3: case 4: case 5:case 6:case 7:case 8:case 9:case 0:
				subMenu = subMenu* 10 + nkey;			
				displayUserMemuLED(0,subMenu);
				break;
			case 0x0e:
				if(subMenu)
					subMenu = 0;
				else
					returnFlag = 1;
				displayUserMemuLED(0,subMenu);
				break;
			case 0x0f:
				if(subMenu)
				{
					enterSubMenu = 1;					
				}		
				break;
			default:
				returnFlag = 0;
				break;
		}
		
		if(enterSubMenu)
		{
			switch(subMenu)
			{
//==============交易记录1- 19========================================
				case 1://纸币器收币
					DispBillRecvMoney();
					break;
				case 2://硬币器收币
					dispCoinRecvMony();
					break;
				case 3://找零总金额
					dispChangerMoney();	
					break;
				case 4://成功交易次数
					break;
					
				case 5://欠费金额
					SeeDebtMoney();
					break;		

				case 6://reserved
					
					break;	

				case 7://hopper1出币数
					dispHopperChangeSum(1);
					break;
				case 8://hopper2出币数
					dispHopperChangeSum(2);
					break;
				case 9://hopper3出币数
					dispHopperChangeSum(3);
					break;
				
//================================================================

//==============设备管理20 -39========================================
				case 20://测试纸币器收币
					
					BillTest();
					break;
				case 21://测试硬币器收币
					CoinDevTest();
					break;
				case 22://测试找零总金额	
					hopperTest(22);	
					break;
				case 23://清零hopper斗			
					hopperClear(23);	
					break;	
//================================================================
				

//==============货道管理40 -59========================================




					
//================================================================



				case 99:// 清除交易记录
					ledClearTrade();					
					break;
					
					
				default:
					break;
			}
			enterSubMenu = 0;
			subMenu = 0;
			displayUserMemuLED(0,0);
			
		}

		if(returnFlag)
		{
			deviceEnable(TRUE);
			DisplayDecimalNumberWithDot(0,SYSPara.nPoinValue);
			
			return 1;
		}
		
		OSTimeDly(5);
		
	}
	
	
}

/*********************************************************************************************************
** Function name:       UserMenu
** Descriptions:        用户菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t UserMenu()
{
	
#if VMC_TYPE == VMC_TYPE_CHANGER
	return vmcChangerUserMenu();
#else
	uint8_t nkey;
	//进入维护模式，发送禁能纸币器命令
	deviceEnable(FALSE);
	OSTimeDly(200);
	DosplaySettingMemu(0,0);
	while(1)
	{
		nkey = GetKeyInValue();
		switch(nkey)
		{
			case 0x01:
			case 0x02:
				ColumnParaSetMenu(nkey);
				DosplaySettingMemu(0,0);
				break;
			case 0x03:
				DosplaySettingMemu(0,3);
				ColumnTest();
				DosplaySettingMemu(0,0);
				break;
			case 0x04:
				DosplaySettingMemu(0,4);
				KeyTest();
				DosplaySettingMemu(0,0);
				break;
			case 0x05:
				DosplaySettingMemu(0,5);
				CoinDevTest();
				DosplaySettingMemu(0,0);
				break;
			case 0x06:
				DosplaySettingMemu(0,6);
				HopperTest();
				DosplaySettingMemu(0,0);
				break;
			case 0x07:
				DosplaySettingMemu(0,7);
				BillTest();
				DosplaySettingMemu(0,0);
				break;
			case 0x08:
				DosplaySettingMemu(0,8);
				ColumnTradeCountSum();
				DosplaySettingMemu(0,0);
				break;
			case 0x09:
				DosplaySettingMemu(0,9);
				RecvMoneySum();
				DosplaySettingMemu(0,0);
				break;
			case 0x0a:
				DosplaySettingMemu(0,10);
				ClearMoneySum();
				DosplaySettingMemu(0,0);
				break;
			case 0x0e:
				DisplayDecimalNumberWithDot(0,0);
				//ChannelInit();
				deviceEnable(TRUE);
				return 0;
			default:
				break;
		}
	}	
#endif
	
}


/*********************************************************************************************************
** Function name:       setDeviceChannelValue
** Descriptions:        设置交易设备通道值菜单
** input parameters:    nmenu:父菜单；mmenu：子菜单
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void setDeviceChannelValue(unsigned char menu,unsigned char type)
{
	
	uint8_t nkey,enterEdit = 0,subKey,index = 0,topReturnFlag = 0,subMenu = 0,topIndex = 0;
	uint32_t value;
	print_log("admin menu =%d, type = %d\r\n",menu,type);
	displayAdminMemuLED(menu,0);//显示一级菜单
	while(1)
	{
		
		nkey = GetKeyInValue();
		switch(nkey)//
		{
			case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:
				if(type == 3 && nkey > 3)
				{
					break;
				}
				topIndex = nkey;
				displayAdminMemuLED(menu,nkey);
				break;
			case 0x0e:
				if(topIndex)
					topIndex = 0;
				else
					topReturnFlag = 1;

				print_log("topReturnFlag = %d\r\n",topReturnFlag);
				displayAdminMemuLED(menu,topIndex);
				break;
			case 0x0f://进入下一级菜单
				if(topIndex)
				{
					enterEdit = 0;
					if(type == 3 && topIndex > 3)
					{
						subMenu = 0;
						displayAdminMemuLED(menu,topIndex);
					}
					else
					{
						index = topIndex;
						if(type == 1)
							value = amountTurnByPoint(stDevValue.BillValue[index - 1]);
						else if(type == 2)
							value = amountTurnByPoint(stDevValue.CoinValue[index - 1]);
						else if(type == 3)
							value = amountTurnByPoint(stDevValue.HpValue[(index - 1) % 3]);
						else 
							value = 0;
						
						subMenu = 1;
						DisplayDecimalNumberWithDot(value,SYSPara.nPoinValue);
					}

					
				}
				print_log("subMenu = %d, index = %d\r\n",subMenu,index);	
				break;
			default:
				
				break;
		}
		
		while(subMenu)
		{
			
			subKey = GetKeyInValue();
			if(subKey == 0x0f)
			{
				enterEdit = 1;
				value = 0;
				DisplayDecimalNumberWithDot(0,0);	
			}
			else if(subKey == 0x0e)
			{	
				subMenu = 0;//退出
				displayAdminMemuLED(menu,topIndex);	
			}
			
				
			while(enterEdit)//进入编辑模式
			{
				subKey = GetKeyInValue();
				switch(subKey)
				{
					case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:case 0x06:case 0x07:case 0x08:case 0x09:case 0x00:
						value = value * 10 + subKey;
						DisplayDecimalNumberWithDot(value,SYSPara.nPoinValue);
						break;
					case 0x0e://取消修改
						if(value)
						{
							DisplayDecimalNumberWithDot(0,0);
							value = 0;
						}
						else
						{
							enterEdit = 0;
							if(type == 1)
								value = amountTurnByPoint(stDevValue.BillValue[index - 1]);
							else if(type == 2)
								value = amountTurnByPoint(stDevValue.CoinValue[index - 1]);
							else if(type == 3)
								value = amountTurnByPoint(stDevValue.HpValue[(index - 1) % 3]);
							else
								value = 0;
							DisplayDecimalNumberWithDot(value,SYSPara.nPoinValue);
						}
							
						
						break;
					case 0x0f://确定修改
						led_dispaly_off();
						OSTimeDly(600/5);
						if(type == 1)
							stDevValue.BillValue[index - 1] = amountTurnTocents(value);
						else if(type == 2)
							stDevValue.CoinValue[index - 1] = amountTurnTocents(value);
						else if(type == 3)
							stDevValue.HpValue[(index - 1) % 3] = amountTurnTocents(value);
						
						enterEdit = 0;
						saveSystemParaFromFlash();
						DisplayDecimalNumberWithDot(value,SYSPara.nPoinValue);
						break;
					default:
						break;
				}
				
				OSTimeDly(10);	
			}
			
			OSTimeDly(10);	

		}

		if(topReturnFlag)
			return;
	
		OSTimeDly(10);
	
	}
		
	
		
}




/*********************************************************************************************************
** Function name:       setPointValue
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void setPointValue(unsigned char menu)
{
	unsigned char key,enterEdit = 0,returnFlag = 0;

	unsigned char point;
	point = SYSPara.nPoinValue;
	DisplayDecimalNumberWithDot(point,0);
	while(1)
	{
		key = GetKeyInValue();
		switch(key)
		{
			case 0x0f:
				point = 0;
				DisplayDecimalNumberWithDot(point,0);
				enterEdit = 1;
				break;
			case 0x0e:				
				returnFlag = 1;
				break;
			default:
				break;
		}

		while(enterEdit)
		{
			key = GetKeyInValue();
			switch(key)
			{
				case 0x01: case 0x02: case 0x03:
					point = key;
					DisplayDecimalNumberWithDot(point,0);
					break;
				case 0x0e:
					if(point)
					{
						point = 0;
						DisplayDecimalNumberWithDot(point,0);
					}	
					else
					{
						enterEdit = 0;
						DisplayDecimalNumberWithDot(SYSPara.nPoinValue,0);
					}	
					
					break;
				case 0x0f:
					led_dispaly_off();
					OSTimeDly(600/5);
					SYSPara.nPoinValue = point;
					saveSystemParaFromFlash();
					DisplayDecimalNumberWithDot(point,0);
					enterEdit = 0;
					break;
				default:
					break;
					
			}

			OSTimeDly(5);
			
		}

		if(returnFlag)
			return;
		
		OSTimeDly(5);
	}
}

/*********************************************************************************************************
** Function name:       AdminMenu
** Descriptions:        管理员菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t vmcChangerAdminMenu(void)
{

	uint8_t enterSubMenu = 0,returnFlag = 0,key;
	unsigned short menuKey = 0;

	//进入管理员模式，发送禁能纸币器命令
	deviceEnable(FALSE);
	OSTimeDly(20);
	displayAdminMemuLED(0,0);//跟目录-- A0.00
	while(1)
	{
		key = GetKeyInValue();
		switch(key)//一级菜单选择
		{
			case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:case 0x06:case 0x07:case 0x08:case 0x09:case 0x00:
				menuKey = menuKey * 10 + key;
				displayAdminMemuLED(0,menuKey);// A0.menu
				returnFlag = 0;
				break;
			case 0x0e:
				if(menuKey)
					menuKey = 0;
				else
					returnFlag = 1;
				displayAdminMemuLED(menuKey,0);
				print_log("menuKey = %d ,returnFlag =%d\r\n",menuKey,returnFlag);
				break;
			case 0x0f:
				if(menuKey)
				{
					enterSubMenu = 1;
				}
				print_log("menuKey = %d ,enterSubMenu =%d\r\n",menuKey,enterSubMenu);	
				break;
			default:
				break;

		}
		
		if(enterSubMenu)//进入二级菜单选择
		{
			displayAdminMemuLED(menuKey,0);//显示一级菜单
			switch(menuKey)
			{
				
				case 1://纸币器通道面值设置
					setDeviceChannelValue(1,1);
					break;
				case 2://硬币币器通道面值设置
					setDeviceChannelValue(2,2);
					break;
				case 3://找零通道面值设置
					setDeviceChannelValue(3,3);
					break;
				case 4://小数点位数
					setPointValue(4);
					break;
					
				default:
					break;
			}
			enterSubMenu = 0;
			menuKey = 0;
			displayAdminMemuLED(0,0);//返回根目录
			
		}

		
		if(returnFlag)
		{
			deviceEnable(TRUE);
			DisplayDecimalNumberWithDot(0,SYSPara.nPoinValue);
			return 1;
		}
			
		OSTimeDly(5);

	}
	
}





/*********************************************************************************************************
** Function name:       AdminMenu
** Descriptions:        管理员菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t AdminMenu()
{

#if VMC_TYPE == VMC_TYPE_CHANGER
		return vmcChangerAdminMenu();
#else

#if 0
	uint8_t nkey,i;
	uint8_t flagl=0;

	//进入维护模式，发送禁能纸币器命令
	deviceEnable(FALSE);
	Trace("AdminMenu...\r\n");
	DosplaySettingMemu(0,10);  	
	while(1)
	{
		nkey = GetKeyInValue();
		switch(nkey)
		{
			case 0x01:
				DosplaySettingMemu(0,11);
				SetDevValueMenu(HPMENU,11);
				stDevValue.HpValue[0] = GetMemoryData(ADD_HP1VALUE);
				stDevValue.HpValue[1] = GetMemoryData(ADD_HP2VALUE);
				stDevValue.HpValue[2] = GetMemoryData(ADD_HP3VALUE);
//				stEvbHp[0].Price = stDevValue.HpValue[0];
//				stEvbHp[1].Price = stDevValue.HpValue[1];
//				stEvbHp[2].Price = stDevValue.HpValue[2];
//				EvbDevice_HpValueListInit();
				DosplaySettingMemu(0,10);
				break;
			case 0x02:
				DosplaySettingMemu(0,12);
				SetDevValueMenu(BILLMENU,12);
				for(i=0;i<8;i++)
					stDevValue.BillValue[i] = GetMemoryData(ADD_BILLVALUE1+i*4);	
				DosplaySettingMemu(0,10);
				break;
			case 0x03:
				DosplaySettingMemu(0,13);
				SetDevValueMenu(COINMENU,13);
				for(i=0;i<8;i++)
					stDevValue.CoinValue[i] = GetMemoryData(ADD_COINVALUE1+i*4);
				DosplaySettingMemu(0,10);
				break;
			case 0x04:
				DosplaySettingMemu(0,14);
				SetPoinValue();
				DosplaySettingMemu(0,10);
				break;
			case 0x05:
				DosplaySettingMemu(0,15);
				KeyTest();
				DosplaySettingMemu(0,10);
				break;
			case 0x06:
				DosplaySettingMemu(0,16);
				HopperTest();
				DosplaySettingMemu(0,10);
				break;
			case 0x07:
				DosplaySettingMemu(0,17);
				BillTest();
				DosplaySettingMemu(0,10);
				break;
			case 0x08:
				DosplaySettingMemu(0,18);
				SetDispMinMoney();
				DosplaySettingMemu(0,10);
				break;
			case 0x0e:
				flagl = 1;
				//ResetDevValue();
				break;
		}
		if(flagl == 1)
			break;
	}
	//DisplayDecimalNumberWithDot(0,nPoinValue);
	DisplayDecimalNumberWithDot(0,0);
	deviceEnable(TRUE);
 #endif
	return 0;

#endif
}


/*********************************************************************************************************
** Function name:       SetValue
** Descriptions:        设置参数值函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t SetValue(unsigned int value)
{
	uint8_t nkey;
	uint8_t index=0,flag = 0;
	uint32_t setValue=0;
	
	while(1)
	{  
		nkey = GetKeyInValue();
		switch(nkey)
		{
			case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07: case 0x08: case 0x09:
				if(index>=6)
					break;
				setValue = setValue * 10 + nkey ;
				index++;
				if(setValue<10000)
				{
					DisplayDecimalNumberWithDot(setValue,SYSPara.nPoinValue);
				}
				else
				{
					DisplayDecimalNumberWithDot(setValue/10000,0);
					OSTimeDly(300);
					DisplayDecimalNumberWithDot(setValue%10000,SYSPara.nPoinValue+3);	
				}
				break;
			case 0x0e:
				setValue = 0;
				break;
			case 0x0f:
				flag = 1;
				break;
		}

		if(flag)
		{
			return  (setValue) ? setValue : value;
		}
			
	}	
}




/*********************************************************************************************************
** Function name:       SetDevValueMenu
** Descriptions:        设置交易设备通道值菜单
** input parameters:    nmenu:父菜单；mmenu：子菜单
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SetDevValueMenu(uint8_t nmenu,uint8_t mmenu)
{
	//mmenu = 11
	uint8_t nkey;
	uint8_t lmenu = 0;
	uint32_t lMenuType=0,mData=0;
	
	while(1)
	{  
		nkey = GetKeyInValue();
		switch(nkey)
		{
			case 0x01:
				lmenu = 1;
				DosplaySettingMemu(1,mmenu);//F11.1
				break;
			case 0x02:
				lmenu = 2;
				DosplaySettingMemu(2,mmenu);
				break;
			case 0x03:
				lmenu = 3;
				DosplaySettingMemu(3,mmenu);
				break;
			case 0x04:				
				if(nmenu==HPMENU)
					break;
				lmenu = 4;
				DosplaySettingMemu(4,mmenu);
				break;
			case 0x05:				
				if(nmenu==HPMENU)
					break;
				lmenu = 5;
				DosplaySettingMemu(5,mmenu);
				break;
			case 0x06:
				if(nmenu==HPMENU)
					break;
				lmenu = 6;
				DosplaySettingMemu(6,mmenu);
				break;
			case 0x07:
				if(nmenu==HPMENU)
					break;
				lmenu = 7;
				DosplaySettingMemu(7,mmenu);
				break;
			case 0x08:
				if(nmenu==HPMENU)
					break;
				lmenu = 8;
				DosplaySettingMemu(8,mmenu);
				break;
			case 0x0e:
				DosplaySettingMemu(0,mmenu);
				return;
			case 0x0f://lmenu 通道号
				if(lmenu==0)
					break;
				if((nmenu == HPMENU)&&(lmenu == 1))
					lMenuType = stDevValue.HpValue[0];
				if((nmenu == HPMENU)&&(lmenu == 2))
					lMenuType = stDevValue.HpValue[1];
				if((nmenu == HPMENU)&&(lmenu == 3))
					lMenuType = stDevValue.HpValue[2];
				if((nmenu == BILLMENU)&&(lmenu == 1))
					lMenuType = stDevValue.BillValue[0];
				if((nmenu == BILLMENU)&&(lmenu == 2))
					lMenuType = stDevValue.BillValue[1];
				if((nmenu == BILLMENU)&&(lmenu == 3))
					lMenuType = stDevValue.BillValue[2];
				if((nmenu == BILLMENU)&&(lmenu == 4))
					lMenuType = stDevValue.BillValue[3];
				if((nmenu == BILLMENU)&&(lmenu == 5))
					lMenuType = stDevValue.BillValue[4];
				if((nmenu == BILLMENU)&&(lmenu == 6))
					lMenuType = stDevValue.BillValue[5];
				if((nmenu == BILLMENU)&&(lmenu == 7))
					lMenuType = stDevValue.BillValue[6];
				if((nmenu == BILLMENU)&&(lmenu == 8))
					lMenuType = stDevValue.BillValue[7];
				if((nmenu == COINMENU)&&(lmenu == 1))
					lMenuType = stDevValue.CoinValue[0];
				if((nmenu == COINMENU)&&(lmenu == 2))
					lMenuType = stDevValue.CoinValue[1];
				if((nmenu == COINMENU)&&(lmenu == 3))
					lMenuType = stDevValue.CoinValue[2];
				if((nmenu == COINMENU)&&(lmenu == 4))
					lMenuType = stDevValue.CoinValue[3];
				if((nmenu == COINMENU)&&(lmenu == 5))
					lMenuType = stDevValue.CoinValue[4];
				if((nmenu == COINMENU)&&(lmenu == 6))
					lMenuType = stDevValue.CoinValue[5];
				if((nmenu == COINMENU)&&(lmenu == 7))
					lMenuType = stDevValue.CoinValue[6];
				if((nmenu == COINMENU)&&(lmenu == 8))
					lMenuType = stDevValue.CoinValue[7];
				mData = amountTurnByPoint(lMenuType);
				if(mData>=10000)
				{
					DisplayDecimalNumberWithDot(mData/10000,0);
					OSTimeDly(500);
					DisplayDecimalNumberWithDot(mData%10000,SYSPara.nPoinValue+3);
				}
				else
					DisplayDecimalNumberWithDot(mData,SYSPara.nPoinValue);

				
				lMenuType =  amountTurnTocents(SetValue(mData));
				
				if((nmenu == HPMENU)&&(lmenu == 1))
					stDevValue.HpValue[0] = lMenuType;
				if((nmenu == HPMENU)&&(lmenu == 2))
					stDevValue.HpValue[1] = lMenuType;
				if((nmenu == HPMENU)&&(lmenu == 3))
					stDevValue.HpValue[2] = lMenuType;
				if((nmenu == BILLMENU)&&(lmenu == 1))
					stDevValue.BillValue[0] = lMenuType;
				if((nmenu == BILLMENU)&&(lmenu == 2))
					stDevValue.BillValue[1] = lMenuType;
				if((nmenu == BILLMENU)&&(lmenu == 3))
					stDevValue.BillValue[2] = lMenuType;
				if((nmenu == BILLMENU)&&(lmenu == 4))
					stDevValue.BillValue[3] = lMenuType;
				if((nmenu == BILLMENU)&&(lmenu == 5))
					stDevValue.BillValue[4] = lMenuType;
				if((nmenu == BILLMENU)&&(lmenu == 6))
					stDevValue.BillValue[5] = lMenuType;
				if((nmenu == BILLMENU)&&(lmenu == 7))
					stDevValue.BillValue[6] = lMenuType;
				if((nmenu == BILLMENU)&&(lmenu == 8))
					stDevValue.BillValue[7] = lMenuType;
				if((nmenu == COINMENU)&&(lmenu == 1))
					stDevValue.CoinValue[0] = lMenuType;
				if((nmenu == COINMENU)&&(lmenu == 2))
					stDevValue.CoinValue[1] = lMenuType;
				if((nmenu == COINMENU)&&(lmenu == 3))
					stDevValue.CoinValue[2] = lMenuType;
				if((nmenu == COINMENU)&&(lmenu == 4))
					stDevValue.CoinValue[3] = lMenuType;
				if((nmenu == COINMENU)&&(lmenu == 5))
					stDevValue.CoinValue[4] = lMenuType;
				if((nmenu == COINMENU)&&(lmenu == 6))
					stDevValue.CoinValue[5] = lMenuType;
				if((nmenu == COINMENU)&&(lmenu == 7))
					stDevValue.CoinValue[6] = lMenuType;
				if((nmenu == COINMENU)&&(lmenu == 8))
					stDevValue.CoinValue[7] = lMenuType;
				
				saveSystemParaFromFlash();
				DosplaySettingMemu(0,mmenu);	
				break;	
		}	
	}
}





/*********************************************************************************************************
** Function name:       ResetDevValue
** Descriptions:        重新配置设备各通道面值
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ResetDevValue()
{
  #if 0
	uint8_t i;

	for(i=0;i<8;i++)
	{
		//读取Hopper金额
		if(i<3)
			stDevValue.HpValue[i] = GetMemoryData(ADD_HP1VALUE+i*4);
		//读取纸币器金额
		//if(NowBillDev == BILLDEV_RS232)	
	//		stDevValue.BillValue[i] = GetMemoryData(ADD_BILLVALUE1+i*4);
		//读取硬币金额
		stDevValue.CoinValue[i] = GetMemoryData(ADD_COINVALUE1+i*4);
		//读取货道商品余量
//		stChannel[i].GoodsCount = GetMemoryData(ADD_CHANNELCOUNT+i*4);
		//读取货道状态
//		stChannel[i].State = GetMemoryData(ADD_CHANNELSTATE+i*4);
		//读取货道金额
//		stChannel[i].Price = GetMemoryData(ADD_CHANNELPRICE+i*4);	
	}
	#endif
}


/*********************************************************************************************************
** Function name:       SetPoinValue
** Descriptions:        设置小数点位数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SetPoinValue()
{
	uint8_t nkey,falg=0;
	uint32_t setValue=0;
		
	while(1)
	{	
		nkey = GetKeyInValue();
		if(nkey==0x0f)
		{
			setValue = SYSPara.nPoinValue;
			DisplayDecimalNumberWithDot(setValue,0);
			while(1)
			{  
				nkey = GetKeyInValue();
				switch(nkey)
				{
					case 0x00: case 0x01: case 0x02:
						setValue = (uint32_t)nkey;
						DisplayDecimalNumberWithDot(setValue,0);
						break;
					case 0x0e:
						falg = 1;
						break;
					case 0x0f:
						falg = 1;
						SYSPara.nPoinValue = setValue;
						saveSystemParaFromFlash();
						break;
					default:
						break;
				}
				if(falg == 1)
					break;
			}
		}
		else
		if(nkey==0x0e)
		{
			break;
		}
		if(falg == 1)
			break;
	}	
}

void KeyTest()
{
	uint8_t nkey;
	uint8_t select = 0;
	
	while(1)
	{	
		nkey = GetKeyInValue();
		if(nkey == 0x0f)
		{
			DisplayDecimalNumberWithDot(0,0);
			while(1)
			{
				nkey = GetKeyInValue();
				if(nkey==0x0e)
					break;
				select = UserSelectKey();
				if(select!=0)
				{
					DisplayDecimalNumberWithDot(select,0);	
				}
				OSTimeDly(10);	
			}
		}
		else
		if(nkey == 0x0e)
			break;
	}	
}

/*********************************************************************************************************
** Function name:       GetInMenuType
** Descriptions:        判断是否有进入维护菜单的组合键输入
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t GetInMenuType()
{

	uint8_t iKey = 0xff,rst = 0;
	static unsigned char adminFlag = 0,userFalg = 0;	
	iKey = GetKeyInValue();
	if(iKey != 0xFF)
	{
		if(iKey == 0x0b)
		{
			if(adminFlag)
				adminFlag = 0;
			userFalg++;
		}	
		else if(iKey == 0x0c)
		{
			if(userFalg)
				userFalg = 0;
			adminFlag++;
		}
		else
		{
			userFalg = 0;
			adminFlag = 0;
		}
			
	}
	
	if(adminFlag >= 3)
		rst = 1;
	else if(userFalg >= 3)
		rst = 2;
	if(rst)
	{
		userFalg = 0;
		adminFlag = 0;
	}
	return rst;		

}





//查询收入金额总记录
void RecvMoneySum()
{

}

void ColumnTradeCountSum()
{

}


//清除交易记录
void ClearMoneySum()
{
	uint8_t iKey = 0xff;
	
	while(1)
	{
		iKey = GetKeyInValue();
		if(iKey==0x0f)
		{
			clearTradeInfo();
			OSTimeDly(300);
			break;
		}
		else
		if(iKey==0x0e)
			break;
	}	
}


void HopperTest()
{

	
}


void BillTest()
{
	uint8_t nKey = 0xff;
	uint8_t type=0,err;
	uint32_t InValue=0,CurrentMoney=0;

	DisplayDecimalNumberWithDot(0,0);
	deviceEnable(TRUE);
	while(1)
	{
		nKey = GetKeyInValue();
		if(nKey == 0x0e)
		{	  	
			break;
		}
		err = BillDevIsBillIn(&type,&InValue);
		if(err == 1)
		{
			CurrentMoney += amountTurnByPoint(InValue);
			if(CurrentMoney<10000)
			{
				DisplayDecimalNumberWithDot(CurrentMoney,SYSPara.nPoinValue);
			}
			else
			{
				DisplayDecimalNumberWithDot(CurrentMoney/10000,0);
				OSTimeDly(150);
				DisplayDecimalNumberWithDot(CurrentMoney%10000,SYSPara.nPoinValue+3);	
			}	
		}
		OSTimeDly(15);
	}
	deviceEnable(FALSE);	
	OSTimeDly(200);
}

//设置允许退币金额
void SetDispMinMoney()
{
 
}



