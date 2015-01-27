
/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           MAINTAIN.C
** Last modified Date:  2013-03-06
** Last Version:        No
** Descriptions:        Î¬»¤Á÷³Ì                     
**------------------------------------------------------------------------------------------------------
** Created by:          yanbo
** Created date:        2014-02-16
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"
#include "MAINTAIN.H"
#include "USERMAINTAIN_MENU.C"
#include "..\API\Public.h"
#include "SailControl.h"
 #include "..\API\ChannelHandle.h"

#define SystemPara SYSPara





/*********************************************************************************************************
** Function name:     	LogTransaction
** Descriptions:	    ÈÕ³£½»Ò×¼ÇÂ¼²éÑ¯
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      1½øÈë
*********************************************************************************************************/

static void LogTransaction(void)
{	
	uint8_t pageNum = 0,lineNum = 3;
	unsigned char Key;
	unsigned int value = 0;
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		LCDClrArea(1,4,238,14);
		lineNum = 3;
		switch(pageNum)
		{
			case 0:
				LCDPrintf(8,1,0,SYSPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);
					
				value = (stTotalTrade.BillMoneySum + stTotalTrade.CoinMoneySum);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.Income[SystemPara.Language],
					(value)/100,(value)%100);
				value = (stTotalTrade.BillMoneySum);
				
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.NoteIncome[SystemPara.Language],
					value/100,value%100);
				value = (stTotalTrade.CoinMoneySum);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.CoinsIncome[SystemPara.Language],
					value/100,value%100);

				value = (stTotalTrade.tradeAmount);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.TotalTrans[SystemPara.Language],
					(value)/100,(value)%100);
				
				value = (stTotalTrade.DispenceSum );
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.TotalChange[SystemPara.Language],
					value/100,value%100);
				
				

				break;

				
			case 1:
				LCDPrintf(8,1,0,SYSPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);
				value = (stTotalTrade.iouAmount);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.Iou[SystemPara.Language],
					value/100,value%100);

				break;
			
		}
		while(1)
		{
			Key = ReadKeyValue();
			if(Key == 'C')
			{				
				return;
			}
			else if(Key == '>')
			{				
				if(pageNum < 3)
					pageNum++;
				break;
			}
			else if(Key == '<')
			{				
				if(pageNum > 0)
					pageNum--;
				break;
			}
			OSTimeDly(10);	
		}			
	}
	
}
  

/*********************************************************************************************************
** Function name:     	ColumnTradeCountSum
** Descriptions:	    »õµÀÏúÊÛ²éÑ¯
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      1½øÈë
*********************************************************************************************************/
static void ColumnTradeCountSum()
{
	uint8_t pageNum = 0,lineNum = 3;
	unsigned char Key;
	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		LCDClrArea(1,4,238,14);
		lineNum = 3;
		switch(pageNum)
		{
			case 0:
				LCDPrintf(8,1,0,SystemPara.Language,user_column_trade_sum.ModeCapion[SystemPara.Language]);				
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",user_column_trade_sum.column1[SystemPara.Language],
					getColumnInfoBySelectNo(1,1,HUODAO_TYPE_TRADE_NUM));
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",user_column_trade_sum.column2[SystemPara.Language],
					getColumnInfoBySelectNo(1,2,HUODAO_TYPE_TRADE_NUM));
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",user_column_trade_sum.column3[SystemPara.Language],
					getColumnInfoBySelectNo(1,3,HUODAO_TYPE_TRADE_NUM));
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",user_column_trade_sum.column4[SystemPara.Language],
					getColumnInfoBySelectNo(1,4,HUODAO_TYPE_TRADE_NUM));								
				//LCDPrintf(231-8*strlen(LogTransMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageDown[SystemPara.Language]);//·­Ò³ÌáÊ¾
				break;
		}
		
		while(1)
		{
			Key = ReadKeyValue();
			if(Key == 'C')
			{				
				return;
			}
			else if(Key == '>')
			{				
				if(pageNum < 3)
					pageNum++;
				break;
			}
			else if(Key == '<')
			{				
				if(pageNum > 0)
					pageNum--;
				break;
			}
			OSTimeDly(10);	
		}
	}
		
}

/*********************************************************************************************************
** Function name:     	SelectOKCheck
** Descriptions:	    È·ÈÏÈ¡Ïû¶Ô»°¿òÒ³Ãæ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      1½øÈë
*********************************************************************************************************/
uint8_t SelectOKCheck(void)
{	
	unsigned char Key;
	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		LCDClrArea(1,4,238,14);
		LCDPrintf(8,1,0,SystemPara.Language,OKMenuList.ModeCapion[SystemPara.Language]);
		LCDPrintf(80,7,0,SystemPara.Language,OKMenuList.SureCapion[SystemPara.Language]);
		LCDPrintf(40,11,0,SystemPara.Language,OKMenuList.OKPin[SystemPara.Language]);
		LCDPrintf(140,11,0,SystemPara.Language,OKMenuList.CancelPin[SystemPara.Language]);		
		while(1)
		{
			Key = ReadKeyValue();
			if(Key == 'C')
			{				
				return 0;
			}
			else if(Key == 'E')
			{				
				return 1;
			}
			OSTimeDly(10);	
		}			
	}
}

/*********************************************************************************************************
** Function name:     	lcdSetVmcResultPrintf
** Descriptions:	    ÉèÖÃ½á¹ûº¯Êý
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      1½øÈë
*********************************************************************************************************/
static uint8_t lcdSetVmcResultPrintf(unsigned char flag)
{
	LCDClearLineDraw(0, 11, 1);
	
	if(flag)
	{
		LCDPrintf(50,11,0,SystemPara.Language,OKMenuList.setOK[SystemPara.Language]);
	}
	else
	{
		LCDPrintf(50,11,0,SystemPara.Language,OKMenuList.setFail[SystemPara.Language]);
	}

	return 1;
}


/*********************************************************************************************************
** Function name:     	PasswordCheck
** Descriptions:	    ÃÜÂëÑéÖ¤Ò³Ãæ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
static uint8_t PasswordCheck(void)
{
	unsigned char i;
	unsigned char Index = 0,Key;
	unsigned char Pin[6],PinOk = 0;
	unsigned char ADMINPASSWORD[8] 	 = {'8','3','7','1','8','5','5','7'};
	
	LCDClrScreen();
	Trace("EnterSelfCheckFuction\r\n");
	while(1)
	{
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDPrintf(8,1,0,SystemPara.Language,PasswordMenuList.ModeCapion[SystemPara.Language]);
		LCDPrintf(48,6,0,SystemPara.Language,PasswordMenuList.EnterPin[SystemPara.Language]);
		LCDClrArea(1,13,238,14);
		LCDPrintf(28,13,0,SystemPara.Language,PasswordMenuList.ReturnCapion[SystemPara.Language]);
		
		while(1)
		{
			PinOk = 0;
			for(i=0;i<8;i++)
			{
				LCDPutAscii(52+(i*16),9,"_",0);
			}
			while(1)//½øÈëÃÜÂëÑéÖ¤
			{
				Key = ReadKeyValue();
				if(Key != 0x00)
				{
					if(Key == 'C')
						return 0;	//°´È¡ÏûÖ±½Ó·µ»Ø
					else
					{
						Pin[Index] = Key;
						LCDPutAscii(52+(16*Index),9,"*",0);
						Index++;
						if(Index == 8)
						{
							Index = 0;
							for(i=0;i<8;i++)
							{
								if(Pin[i] == ADMINPASSWORD[i])//¶Ô±ÈÃÜÂë
									PinOk = 1;
								else
									PinOk = 0;	
							}
							if(PinOk == 1)
							{
								return 1;
							}
							else//ÃÜÂë´íÎó
							{
								LCDClrArea(1,13,238,14);
								LCDPrintf(48,13,0,SystemPara.Language,PasswordMenuList.PinError[SystemPara.Language]);
								OSTimeDly(400);
								LCDClrArea(1,13,238,14);
								LCDPrintf(48,13,0,SystemPara.Language,PasswordMenuList.ReturnCapion[SystemPara.Language]);
								for(i=0;i<8;i++)
								{
									LCDPutAscii(52+(i*16),9,"_",0);
								}	
							}
						}/**/
					}
				}//µÈ´ý°´¼ü
				OSTimeDly(5);
			}//ÃÜÂëÑéÖ¤Ñ­»·	
		}
	} 	
}


/*********************************************************************************************************
** Function name:     	ClearMoneySum
** Descriptions:	    Çå³ý½»Ò×¼ÇÂ¼½çÃæ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
static void ClearMoneySum()
{	
	if(PasswordCheck())
	{
		LCDClrArea(1,4,238,14);
		if(SelectOKCheck())
		{
			clearTradeInfo();
			lcdSetVmcResultPrintf(1);
			OSTimeDly(300);		
		}
	}

}
	

/*********************************************************************************************************
** Function name:     	LogMaintainProcess
** Descriptions:	    ½øÈë½»Ò×ÈÕÖ¾Ö÷²Ëµ¥
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void LogMaintainProcess(void)
{
	unsigned char Key;
	
	void (*TestFunctonPtr)(void);
	while(1)
	{
		LCDClrScreen();
		LogMaintainMainMenu.PageNumb	  = 0x00;
		LogMaintainMainMenu.KeyCurrent  = 0x00;
		LogMaintainMainMenu.ExtSelfCheck= 0x00;
		LogMaintainMainMenu.TestOperate = (void *)0;
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		if(LogMaintainMainMenu.PageNumb == 0x00)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,0,LogMaintainMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,0,LogMaintainMenuList.Record[SystemPara.Language]);
			LCDPrintf(5,7,0,0,LogMaintainMenuList.ColumnRecord[SystemPara.Language]);
			LCDPrintf(5,9,0,0,LogMaintainMenuList.ClearRecord[SystemPara.Language]);
		}

		while(1)
		{
			Key = ReadKeyValue();
			//Trace("Key Select = %C\r\n",Key);
			switch(Key)
			{
				case 'C' :	LogMaintainMainMenu.ExtSelfCheck = 0x01;
							//Trace("\r\n c");
							break;
				case '<' :
							break;
				case '>' :	
							break;
				case '1' :	
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.KeyCurrent = 0x02;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LogMaintainMainMenu.TestOperate = LogTransaction;
							}
							//Trace("\r\n 1");
							break;
				case '2' :	
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.KeyCurrent = 0x02;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LogMaintainMainMenu.TestOperate = ColumnTradeCountSum;
							}
							
							break;
				case '3' :	
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.KeyCurrent = 0x02;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LogMaintainMainMenu.TestOperate = ClearMoneySum;
							}
							
							break;
						
				default  :	
							break;	
			}
			if(LogMaintainMainMenu.ExtSelfCheck == 0x01)
			{
				
				break;
			}
			if(LogMaintainMainMenu.KeyCurrent != 0x00)
			{
				TestFunctonPtr = LogMaintainMainMenu.TestOperate;
				(*TestFunctonPtr)();//ÓÉ´Ë½øÈë¸÷¸öÉè±¸µÄ×Ô¼ìº¯Êý
				break;	
			}
		}	
		if(LogMaintainMainMenu.ExtSelfCheck == 0x01)
		{
			
			break;
		}
	}
}

/*********************************************************************************************************
** Function name:     	ErrorCheck
** Descriptions:	    ¹ÊÕÏ²éÑ¯
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      1½øÈë
*********************************************************************************************************/
static void ErrorCheck(void)
{	
	uint8_t lineNum = 3,Key,isFault = 0;	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	
	
		LCDClrArea(1,4,238,14);
		LCDPrintf(5,1,0,SystemPara.Language,UserMaintainErrorMenuList.vmcFaultList[SystemPara.Language]);	
		//ÏÔÊ¾ERR.0¡ª¡ª±íÊ¾HopperÈ«¹ÊÕÏ»òÈ±±Ò				
		if(HardWareErr & SYS_ERR_NO_HOPPER)
		{
			lineNum += 2;
			LCDPrintf(5,lineNum,0,SystemPara.Language,UserMaintainErrorMenuList.hopperFault[SystemPara.Language]);	
			isFault = 1;
		}
		
		//ÏÔÊ¾ERR.1¡ª¡ª±íÊ¾Ö½±ÒÆ÷¹ÊÕÏ		
		if(HardWareErr & SYS_ERR_NO_BILL)
		{
			lineNum += 2;
			LCDPrintf(5,lineNum,0,SystemPara.Language,UserMaintainErrorMenuList.billFault[SystemPara.Language]);	
			isFault = 1;
		}
		
		//ÏÔÊ¾ERR.2¡ª¡ª±íÊ¾»õµÀÏµÍ³²ÎÊýÖÐµÄ´¢»õÁ¿È«Îª0
		if(HardWareErr & SYS_ERR_NO_HUODAO_EMPTY)
		{
			lineNum += 2;
			LCDPrintf(5,lineNum,0,SystemPara.Language,UserMaintainErrorMenuList.allColumnEmpty[SystemPara.Language]);	
			isFault = 1;
		}
		
		//ÏÔÊ¾ERR.3¡ª¡ª±íÊ¾»õµÀ×Ô¼ìÈ«²¿»õµÀ²»¿ÉÓÃ
		if(HardWareErr & SYS_ERR_NO_HUODAO_FAULT)
		{
			lineNum += 2;
			LCDPrintf(5,lineNum,0,SystemPara.Language,UserMaintainErrorMenuList.Emp_Gol[SystemPara.Language]);
			isFault = 1;
		}
		if(!isFault)
		{
			LCDPrintf(5,5,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);	
		}
		while(1)
		{
			Key = ReadKeyValue();
			if(Key == 'C')
			{				
				return                                                            ;
			}
			OSTimeDly(10);								
		}
	
}


/*********************************************************************************************************
** Function name:     	BillTest
** Descriptions:	    Ö½±ÒÆ÷²âÊÔ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
static void BillTest()
{

	uint32_t InValue;
	uint32_t SumValue = 0;
	char    *pstr;
	char	strMoney[10];
	uint8_t err,type=0;
	
	LCDClrScreen();
	LCDPrintf(0,LINE1,0,0,"TestBillValidator=MDB");
	//LCDPrintf(0,LINE3,0,0,"Level=%d,Scale=%ld",stDevValue.BillLevel,stDevValue.BillScale);
	//LCDPrintf(0,LINE5,0,0,"Capacity=%ld,Escrow=%d",stDevValue.BillStkCapacity,stDevValue.BillEscrowFun);
	LCDPrintf(0,LINE7,0,0,"value=%ld,%ld,%ld,%ld",stDevValue.BillValue[0],stDevValue.BillValue[1],stDevValue.BillValue[2],stDevValue.BillValue[3]);
	LCDPrintf(0,LINE9,0,0,"     =%ld,%ld,%ld,%ld",stDevValue.BillValue[4],stDevValue.BillValue[5],stDevValue.BillValue[6],stDevValue.BillValue[7]);
	Trace("TestBillValidator\r\n");
	deviceEnable(TRUE);
	while(1)
	{
		err = BillDevIsBillIn(&type,&InValue);
		if(err == 1)
		{
			Trace("BillDevIsBillIn .bill=%ld\r\n",InValue);			
			SumValue += InValue;
			
			//pstr = PrintfMoney(SumValue);
			//strcpy(strMoney, pstr);
			
			LCDPrintf(0,LINE13,0,SystemPara.Language," %s%02d.%01d",SelfCheckText.Balance[SystemPara.Language],

			SumValue/100,SumValue%100);
		}
		
		if(ReadKeyValue() == 'C')
			break;
		OSTimeDly(10);		
	}
	deviceEnable(FALSE);
}

/*********************************************************************************************************
** Function name:     	TestCoinAcceptor
** Descriptions:	    Ó²±ÒÆ÷²âÊÔ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
static void TestCoinAcceptor(void)
{	
	uint32_t InValue;
	uint32_t SumValue = 0;
	char    *pstr;
	char	strMoney[10];
	
	LCDClrScreen();
	LCDPrintf(0,LINE1,0,0,"TestCoinAcceptor");
	Trace("TestCoinAcceptor\r\n");
	deviceEnable(TRUE);
	while(1)
	{
		InValue=getCoinMoney();
		if(InValue>0)
		{
			//Trace("2.coin=%ld\r\n",InValue);
			SumValue += InValue;
			pstr = PrintfMoney(SumValue);
			strcpy(strMoney, pstr);
			LCDPrintf(0,LINE15,0,SystemPara.Language," %s%s",SelfCheckText.Balance[SystemPara.Language],strMoney);
		}
		
		if(ReadKeyValue() == 'C')
			break;
		OSTimeDly(5);		
	}
	
	deviceEnable(FALSE);


}

/*********************************************************************************************************
** Function name:     	selectButtonTest
** Descriptions:	   Ñ¡»õ°´¼ü°æ²âÊÔ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
static void selectButtonTest()
{
	uint8_t nKey = 0xff,selectKey = 0;
	unsigned char curState[8] = {0},i,temp;
	unsigned int led[8] = {HUODAO_SELCET_LED1,HUODAO_SELCET_LED2,HUODAO_SELCET_LED3,HUODAO_SELCET_LED4};
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,SYSPara.Language,UserTradeMenuList.SelectButtonTest[SYSPara.Language]);
	LCDPrintf(8,5,0,SYSPara.Language,UserTradeMenuList.InputButton[SYSPara.Language]);
	for(i = 0;i < 8;i++)
		FIO1CLR |= led[i];
	OSQFlush(g_KeyMsg);
	while(1)
	{
		msleep(20);
		nKey = ReadKeyValue();
		if(nKey == 'C')
			return;
		selectKey = UserSelectKey();
		if(selectKey)
		{
			temp = curState[(selectKey - 1) % 8];
			if(temp == 1)
				FIO1CLR |= led[(selectKey - 1) % 8];
			else
				FIO1SET |= led[(selectKey - 1) % 8];
			curState[(selectKey - 1) % 8] = (temp == 1) ? 0 : 1;
			
			LCDPrintf(8,7,0,SYSPara.Language,"%s:%d",UserTradeMenuList.SelectButton[SYSPara.Language],selectKey);		
		}	
		
	}
	
}




/*********************************************************************************************************
** Function name:     	HopperTest
** Descriptions:	    ÕÒÁãÆ÷²âÊÔ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
static void HopperTest()
{
	uint8_t nKey = 0xff,hpoutcount=0;
	
	LCDClrScreen();
    LCDPrintf(0,LINE1,0,0,"TestChanger=HOPPER");
	LCDPrintf(0,LINE5,0,0,"value=%ld,%ld,%ld",stHopper[0].channelValue,stHopper[1].channelValue,stHopper[2].channelValue);
	
	while(1)
	{
		nKey = ReadKeyValue();
		if(nKey == '1')
		{
			if(stHopper[0].channelValue==0)
				continue;

			LCDPrintf(0,LINE15,0,SystemPara.Language,"%shopper1",SelfCheckText.Change[SystemPara.Language]);
			OSTimeDly(150);
			hpoutcount = 5;
			if(hopperOutput(hpoutcount,0x00))	
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%d",SelfCheckText.Change[SystemPara.Language],hpoutcount);
			OSTimeDly(150);		
		}
		else
		if(nKey == '2')
		{
			if(stHopper[1].channelValue==0)
				continue;
			LCDPrintf(0,LINE15,0,SystemPara.Language,"%shopper2",SelfCheckText.Change[SystemPara.Language]);
			OSTimeDly(150);
			hpoutcount = 5;
			if(hopperOutput(hpoutcount,0x01))	
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%d",SelfCheckText.Change[SystemPara.Language],hpoutcount);
			OSTimeDly(150);
		}
		else
		if(nKey == '3')
		{
			if(stHopper[2].channelValue==0)
				continue;
			LCDPrintf(0,LINE15,0,SystemPara.Language,"%shopper2",SelfCheckText.Change[SystemPara.Language]);
			OSTimeDly(150);
			hpoutcount = 5;
			if(hopperOutput(hpoutcount,0x02))	
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%d",SelfCheckText.Change[SystemPara.Language],hpoutcount);
			OSTimeDly(150);	
		}
		else
		if(nKey == 'C')
			break;
	}	
}



/*********************************************************************************************************
** Function name:     	deviceMange
** Descriptions:	    Éè±¸¹ÜÀí
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void deviceMange(void)
{
	void (*TestFunctonPtr)(void);
	unsigned char Key;
	UserTradeMainMenu.PageNumb	  = 0x00;
	UserTradeMainMenu.KeyCurrent  = 0x00;
	UserTradeMainMenu.ExtSelfCheck= 0x00;
	
	while(1)
	{
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDClrArea(1,4,238,14);
		LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
		LCDPrintf(5,5,0,SystemPara.Language,UserTradeMenuList.BillValidatorTest[SystemPara.Language]);
		LCDPrintf(5,7,0,SystemPara.Language,UserTradeMenuList.CoinAcceptorTest[SystemPara.Language]);
		LCDPrintf(5,9,0,SystemPara.Language,UserTradeMenuList.CoinChangerTest[SystemPara.Language]);
		LCDPrintf(5,11,0,SystemPara.Language,UserTradeMenuList.SelectButtonTest[SystemPara.Language]);
		while(1)
		{
			Key = ReadKeyValue();
			//Trace("Key Select = %C\r\n",Key);
			switch(Key)
			{
				case 'C' :	UserTradeMainMenu.ExtSelfCheck = 0x01;
							
							//Trace("\r\n c");
							return;
				
				case '1' :	
							UserTradeMainMenu.KeyCurrent = 0x02;
							UserTradeMainMenu.ExtSelfCheck = 0x00;
							UserTradeMainMenu.TestOperate = BillTest;
							//Trace("\r\n 1");
							break;
						
				case '2' :	
							
							UserTradeMainMenu.KeyCurrent = 0x02;
							UserTradeMainMenu.ExtSelfCheck = 0x00;
							UserTradeMainMenu.TestOperate = TestCoinAcceptor;
							
							break;
				case '3' :	
							
							UserTradeMainMenu.KeyCurrent = 0x02;
							UserTradeMainMenu.ExtSelfCheck = 0x00;
							UserTradeMainMenu.TestOperate = HopperTest;
							
							break;
				case '4' :
							UserTradeMainMenu.KeyCurrent = 0x04;
							UserTradeMainMenu.ExtSelfCheck = 0x00;
							UserTradeMainMenu.TestOperate = selectButtonTest;
							break;
				default  :	
					
							break;	
			}
	
			if(UserTradeMainMenu.KeyCurrent != 0x00)
			{
				
				TestFunctonPtr = UserTradeMainMenu.TestOperate;
				(*TestFunctonPtr)();//ÓÉ´Ë½øÈë¸÷¸öÉè±¸µÄ×Ô¼ìº¯Êý
				UserTradeMainMenu.KeyCurrent = 0;
				break;	
			}
			
		}	
	}

}





/*****************************************************************************
** Function name:	ChannelParamSet	
**
** Descriptions:	Î¬»¤Ä£Ê½ÏÂ»õµÀ½»Ò×²ÎÊýÉèÖÃ	
**					
**														 			
** parameters:		ÎÞ
					
** Returned value:	ÎÞ
** 
*******************************************************************************/
static void ChannelParamSet()
{
	uint8_t key;	
	unsigned char enterSubMenu = 0,editMenu = 0,editIndex = 0,editColumn = 0,state;
	char *cloumnState;
	unsigned int value;
	//ÏÔÊ¾¡°Ïä¹ñÊôÐÔÉèÖÃ¡±
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,SystemPara.Language,LogChannelUi.ChannelParamSet[SystemPara.Language]);
	LCDPrintf(8,5,0,SystemPara.Language,LogChannelUi.ChannelEnterChnlNum[SystemPara.Language]);
	while(1)//¶¥¼¶²Ëµ¥
	{
		key = ReadKeyValue();
		switch(key)
		{
			case 'C':
				return;
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':
				key = key - '0';
				editColumn = key;
				//ÏÔÊ¾»õµÀºÅÊôÐÔ				
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.EnterColumnNum[SystemPara.Language],key);									
				state = getColumnInfoBySelectNo(1, key,HUODAO_TYPE_ISOPEN);
				if(state)
				{
					
					//»õµÀµ¥¼Û
					value = getColumnInfoBySelectNo(1, key,HUODAO_TYPE_PRICE);
					LCDPrintf(8,7,0,SystemPara.Language,"%s%d.%02d",LogChannelUiDisp.ColumnPrice[SystemPara.Language],value/100,value%100);

					//»õµÀÊ£ÓàÁ¿
					value = getColumnInfoBySelectNo(1, key,HUODAO_TYPE_REMAIN);
					LCDPrintf(8,9,0,SystemPara.Language,"%s%d",LogChannelUiDisp.ColumnNum[SystemPara.Language],value);

					value = getColumnInfoBySelectNo(1, key,HUODAO_TYPE_STATE);
					switch(value)
					{
						case HUODAO_STATE_NORMAL:
							cloumnState = (char *)LogChannelUiDisp.columnOK[SystemPara.Language];
							break;
						case HUODAO_STATE_EMPTY:
							cloumnState = (char *)LogChannelUiDisp.columnEmpty[SystemPara.Language];
							break;
						case HUODAO_STATE_FAULT:
							cloumnState = (char *)LogChannelUiDisp.columnFault[SystemPara.Language];
							break;
						default:
							cloumnState = NULL;
							break;
					}
				    //»õµ½×´Ì¬
					LCDPrintf(8,11,0,SystemPara.Language,"%s%s",LogChannelUiDisp.ColumnStatus[SystemPara.Language],cloumnState);
									
					//»õµÀ³É¹¦½»Ò×´ÎÊý
					value = getColumnInfoBySelectNo(1, key,HUODAO_TYPE_TRADE_NUM);
					LCDPrintf(8,13,0,SystemPara.Language,"%s%d",LogChannelUiDisp.ColumnSuccess[SystemPara.Language],value);
					enterSubMenu = 1;
				}
				else
				{
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.NoColumn[SystemPara.Language]);

					OSTimeDly(500/5);
					LCDClrScreen();
					LCDDrawRectangle(0,0,239,15);
					LCDDrawRectangle(0,0,239,3);
					LCDClrArea(1,4,238,14);
					LCDPrintf(8,1,0,SystemPara.Language,LogChannelUi.ChannelParamSet[SystemPara.Language]);
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelUi.ChannelEnterChnlNum[SystemPara.Language]);
					enterSubMenu = 0;
					editColumn = 0;
				}
				
				break;
			default:
				break;
				
		}


		
		while(enterSubMenu)
		{	
			key = ReadKeyValue();			
			switch(key)
			{
				case 'C':
					LCDClrScreen();
					LCDDrawRectangle(0,0,239,15);
					LCDDrawRectangle(0,0,239,3);
					LCDClrArea(1,4,238,14);
					LCDPrintf(8,1,0,SystemPara.Language,LogChannelUi.ChannelParamSet[SystemPara.Language]);
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelUi.ChannelEnterChnlNum[SystemPara.Language]);
					enterSubMenu = 0;
					editMenu = 0;
					break;	
				case '1'://±à¼­µ¥¼Û
					value = 0;
					editMenu = 1;
					editIndex = 1;
					LCDPrintf(8,7,1,SystemPara.Language,"%s%d.%02d",LogChannelUiDisp.ColumnPrice[SystemPara.Language],value/100,value%100);
					break;
				case '2'://±à¼­´¢»õÁ¿
					value = 0;
					editMenu = 1;
					editIndex = 2;
					LCDPrintf(8,9,1,SystemPara.Language,"%s%d",LogChannelUiDisp.ColumnNum[SystemPara.Language],value);

					break;
				default:
					break;	
			}

			OSTimeDly(5);
			//½øÈë±à¼­²Ëµ¥ Èý¼¶
			while(editMenu)
			{
				key = ReadKeyValue();
				switch(key)
				{
					case 'C':
						editMenu = 0;
						value = getColumnInfoBySelectNo(1, editColumn,HUODAO_TYPE_PRICE);
						LCDPrintf(8,7,0,SystemPara.Language,"%s%d.%02d",LogChannelUiDisp.ColumnPrice[SystemPara.Language],value/100,value%100);			

						value = getColumnInfoBySelectNo(1, editColumn,HUODAO_TYPE_REMAIN);
						LCDPrintf(8,9,0,SystemPara.Language,"%s%d",LogChannelUiDisp.ColumnNum[SystemPara.Language],value);
	
						break;
					case 'E':
						
						if(editIndex == 1)
						{
							LCDPrintf(8,7,0,SystemPara.Language,"%s%d.%02d",LogChannelUiDisp.ColumnPrice[SystemPara.Language],value/100,value%100);												
							setColumnInfoBySelectNo(1,editColumn,value,HUODAO_TYPE_PRICE);
							saveHuodaoInfo(TYPE_HUODAO_PRICE);
						}
						else if(editIndex == 2)
						{
							LCDPrintf(8,9,0,SystemPara.Language,"%s%d",LogChannelUiDisp.ColumnNum[SystemPara.Language],value);
							setColumnInfoBySelectNo(1,editColumn,value,HUODAO_TYPE_REMAIN);
							
							hd_check_state(fromSelectNoGetLogicNo(1,editColumn));
							saveHuodaoInfo(TYPE_HUODAO_REMAIN);
							
						}
						editMenu = 0;
						break;
						
					case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':		
						value = value * 10 + key - '0';
						if(editIndex == 1)
						{
							LCDPrintf(8,7,1,SystemPara.Language,"%s%d.%02d",LogChannelUiDisp.ColumnPrice[SystemPara.Language],value/100,value%100);			
						
						}
						else if(editIndex == 2)
						{
							LCDPrintf(8,9,1,SystemPara.Language,"%s%d",LogChannelUiDisp.ColumnNum[SystemPara.Language],value);
						}
						break;
						
					default:
						break;
				}
				
				
				OSTimeDly(5);
			}
			
			
			
		}
		
	}

}

/*****************************************************************************
** Function name:	ChannelAddGoods	
**
** Descriptions:	»õµÀÌí»õ²Ù×÷	
**					
**														 			
** parameters:		ÎÞ
					
** Returned value:	ÎÞ
** 
*******************************************************************************/
void ChannelAddGoods()
{
	uint8_t key;
	unsigned char i,j;
	unsigned int addNum = 0;//Ìí»õÊýÁ¿
	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	//ÏÔÊ¾¡°»õµÀÌí»õ¡±
	LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelAddGoods[SystemPara.Language]);
	//LCDPrintf(8,5,0,SystemPara.Language,LogChannelAddGoods.ChannelAllAdd[SystemPara.Language]);
	LCDPrintf(8,5,0,SystemPara.Language,LogChannelAddGoods.inputAddGoodsNum[SystemPara.Language]);
	
	LCDPrintf(40,11,0,SystemPara.Language,OKMenuList.OKPin[SystemPara.Language]);
	LCDPrintf(140,11,0,SystemPara.Language,OKMenuList.CancelPin[SystemPara.Language]);	
	while(1)
	{		
		key = ReadKeyValue();
		
		switch(key)
		{
			case 'C':
				return;
			case '0':
				if(addNum)
				{
					addNum *= 10;
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelAddGoods.inputAddGoodsNumAndValue[SystemPara.Language],addNum);
				}
				break;
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
				if(addNum)
				{
					addNum = addNum*10 + (key - '0');
				}
				else
					addNum = (key - '0');
				
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelAddGoods.inputAddGoodsNumAndValue[SystemPara.Language],addNum);
				break;
			case 'E'://È·¶¨Ôò±£´æÊý¾Ý
				LCDClearLineDraw(0,11,1);				
				LCDPrintf(80,11,0,SystemPara.Language,LogChannelAddGoods.isSaveHuodao[SystemPara.Language]);
				if(!addNum)
					addNum = 10;//Ä¬ÈÏÌí»õ10¸ö

				for(i = 0;i < COLUMN_LEVEL_NUM;i++)
				{
					for(j =0;j < COLUMN_ONE_LEVEL_NUM;j++)
					{
						if(stHuodao[i].stColumn[j].state == HUODAO_STATE_NORMAL|| stHuodao[i].stColumn[j].state == HUODAO_STATE_EMPTY)
						{
							stHuodao[i].stColumn[j].remainCount = addNum;
							hd_check_state(stHuodao[i].stColumn[j].logicNo);
						}
					}
				}
				saveHuodaoInfo(TYPE_HUODAO_REMAIN);
				
				OSTimeDly(500/5);
				return;
			default:
				break;
		}			
		OSTimeDly(5);	
		
	}

}



/*****************************************************************************
** Function name:	Channel_Test	
**
** Descriptions:	»õµÀ²âÊÔ	
**					
**														 			
** parameters:		ÎÞ
					
** Returned value:	ÎÞ
** 
*******************************************************************************/
void ChannelTest(void)
{

	uint8_t nkey,Cstate=0x00,column=0x01,columnNo,selectKey = 0;
	
	unsigned char enterTesttingMenu = 0;
	Trace("ChannelTest...\r\n");
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	//ÏÔÊ¾¡°»õµÀ²âÊÔ¡±
	LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelTest[SystemPara.Language]);
	LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.inputTestColumn[SystemPara.Language]);
	OSQFlush(g_KeyMsg);
	while(1)
	{
		selectKey = UserSelectKey();
		if(selectKey)
			nkey = (selectKey % 8) + '0';
		else
			nkey = ReadKeyValue();
		
		switch(nkey)
		{
			case 'E':
				break;
			
			case 'C':
				return;

			case '0'://È«»õµÀ×Ô¼ì
				enterTesttingMenu = 1;
				break;
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':
				columnNo = nkey - '0'; 
				if(!fromSelectNoGetLogicNo(1,columnNo))
					break;
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.isTesttingChannel[SystemPara.Language],columnNo);
				OSTimeDly(250);
				
				Cstate = hd_outgoods_test(fromSelectNoGetLogicNo(1,columnNo));
				if(Cstate == COLUMN_RST_OK)
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.TestColumnNormal[SystemPara.Language]);
				else if(Cstate == COLUMN_RST_EMPTY)
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.TestColumnEmpty[SystemPara.Language]);	
				else 
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.TestColumnFault[SystemPara.Language]);	
			
				OSTimeDly(300);	
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				//ÏÔÊ¾¡°»õµÀ²âÊÔ¡±
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelTest[SystemPara.Language]);
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.inputTestColumn[SystemPara.Language]);
							
				break;	
			default:
				break;
				
		}
		while(enterTesttingMenu)
		{
			nkey = ReadKeyValue();
			if(nkey=='C')
			{
				enterTesttingMenu = 0;
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				//ÏÔÊ¾¡°»õµÀ²âÊÔ¡±
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelTest[SystemPara.Language]);
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.inputTestColumn[SystemPara.Language]);
				break;
			}										
			if(fromSelectNoGetLogicNo(1,columnNo))
			{
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.isTesttingChannel[SystemPara.Language],column);
				OSTimeDly(250);

				Cstate = hd_outgoods_test(fromSelectNoGetLogicNo(1,columnNo));
				if(Cstate == COLUMN_RST_OK)
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.TestColumnNormal[SystemPara.Language],column);
				else if(Cstate == COLUMN_RST_EMPTY)
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.TestColumnEmpty[SystemPara.Language],column);	
				else 
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.TestColumnFault[SystemPara.Language],column);	
				
				column++;
			}
			else
				column = 1;
			Trace("column=%d\r\n",column);
			OSTimeDly(300);
			
			
			
		}
	}
	
	
}

/*****************************************************************************
** Function name:	ChannelStateLookUp	
**
** Descriptions:	²éÑ¯»õµÀµ±Ç°×´Ì¬	
**					
**														 			
** parameters:							
** Returned value:	ÎÞ
** 
*******************************************************************************/
static void ChannelStateLookUp()
{
	unsigned char pageIndex = 0,maxPage = 4,i,j,index = 0;//Ò³ºÅ
	unsigned char key = 0;
	char columnList[50] = {0};
	
			
	while(1)
	{
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDClrArea(1,4,238,14);
		switch(pageIndex)
		{
			case 0:
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelOK[SystemPara.Language]);

				memset(columnList,'0',50);
				index = 0;
				for(i = 0;i < COLUMN_LEVEL_NUM;i++)
				{
					if(!stHuodao[i].isOpen)
						continue;
					for(j =0;j < COLUMN_ONE_LEVEL_NUM;j++)
					{
						if(!stHuodao[i].stColumn[j].isOpen )
								continue;
						if(stHuodao[i].stColumn[j].state == HUODAO_STATE_NORMAL && stHuodao[i].stColumn[j].selectNo)
						{
							sprintf(&columnList[index],"%2d",stHuodao[i].stColumn[j].selectNo);
							index += 2;
						}
					}
				}
				
				
				if(index)
					LCDPrintf(8,5,0,SystemPara.Language,"%s",columnList);
				else
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.listIsEmpty[SystemPara.Language]);
				
				break;
			case 1:
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelEmpty[SystemPara.Language]);
				memset(columnList,'0',50);
				index = 0;
				for(i = 0;i < COLUMN_LEVEL_NUM;i++)
				{
					if(!stHuodao[i].isOpen)
						continue;
					for(j =0;j < COLUMN_ONE_LEVEL_NUM;j++)
					{
						if(!stHuodao[i].stColumn[j].isOpen)
								continue;
						if(stHuodao[i].stColumn[j].state == HUODAO_STATE_EMPTY && stHuodao[i].stColumn[j].selectNo)
						{
							sprintf(&columnList[index],"%2d",stHuodao[i].stColumn[j].selectNo);
							index += 2;
						}
					}
				}
				
				if(index)
					LCDPrintf(8,5,0,SystemPara.Language,"%s",columnList);
				else
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.listIsEmpty[SystemPara.Language]);
				
				break;
			case 2:	
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelBad[SystemPara.Language]);
				memset(columnList,'0',50);
				index = 0;
				for(i = 0;i < COLUMN_LEVEL_NUM;i++)
				{
					if(!stHuodao[i].isOpen)
						continue;
					for(j =0;j < COLUMN_ONE_LEVEL_NUM;j++)
					{
						if(!stHuodao[i].stColumn[j].isOpen)
								continue;
						if(stHuodao[i].stColumn[j].state == HUODAO_STATE_FAULT && stHuodao[i].stColumn[j].selectNo)
						{
							sprintf(&columnList[index],"%2d",stHuodao[i].stColumn[j].selectNo);
							index += 2;
						}
					}
				}
				if(index)
					LCDPrintf(8,5,0,SystemPara.Language,"%s",columnList);
				else
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.listIsEmpty[SystemPara.Language]);
				
				break;
			case 3:
				
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelPriceZero[SystemPara.Language]);			
				memset(columnList,'0',50);
				index = 0;
				for(i = 0;i < COLUMN_LEVEL_NUM;i++)
				{
					if(!stHuodao[i].isOpen)
						continue;
					for(j =0;j < COLUMN_ONE_LEVEL_NUM;j++)
					{
						if(!stHuodao[i].stColumn[j].isOpen)
								continue;
						if(!stHuodao[i].stColumn[j].price && stHuodao[i].stColumn[j].selectNo)
						{
							sprintf(&columnList[index],"%2d",stHuodao[i].stColumn[j].selectNo);
							index += 2;
						}
					}
				}
				
				if(index)
					LCDPrintf(8,5,0,SystemPara.Language,"%s",columnList);
				else
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.listIsEmpty[SystemPara.Language]);

				
				break;
				
			default:
				break;
		}
		
		while(1)//µÈ´ý¶ÁÈ¡°´¼ü
		{
			key = ReadKeyValue();
			if(key)
				break;
			OSTimeDly(5);
		}
		switch(key)
		{
			case 'C':
				return;
			case '>':							
				pageIndex = (pageIndex + 1) % maxPage;
				break;				
			case '<':
				pageIndex = (pageIndex) ? (pageIndex - 1): 0;
				break;	
			default:
				break;
		}
		
	}
	
	
}



/*********************************************************************************************************
** Function name:     	columnManage
** Descriptions:	    »õµ½¹ÜÀí
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
static void columnManage(void)
{
	uint8_t key=0xff;

	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelManage[SystemPara.Language]);
	LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.ChannelAttribute[SystemPara.Language]);
	LCDPrintf(8,7,0,SystemPara.Language,LogChannelMenuList.ChannelAddGoods[SystemPara.Language]);
	LCDPrintf(8,9,0,SystemPara.Language,LogChannelMenuList.ChannelStateLookUp[SystemPara.Language]);
	LCDPrintf(8,11,0,SystemPara.Language,LogChannelMenuList.ChannelCheck[SystemPara.Language]);	
	while(1)
	{
		key = GetKeyInValue();
		switch(key)
		{
			case 0x01:
				
				ChannelParamSet();
				ChannelSaveParam();	
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelManage[SystemPara.Language]);
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.ChannelAttribute[SystemPara.Language]);
				LCDPrintf(8,7,0,SystemPara.Language,LogChannelMenuList.ChannelAddGoods[SystemPara.Language]);
				LCDPrintf(8,9,0,SystemPara.Language,LogChannelMenuList.ChannelStateLookUp[SystemPara.Language]);
				LCDPrintf(8,11,0,SystemPara.Language,LogChannelMenuList.ChannelCheck[SystemPara.Language]);	
				break;
			case 0x02:
				ChannelAddGoods();
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelManage[SystemPara.Language]);
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.ChannelAttribute[SystemPara.Language]);
				LCDPrintf(8,7,0,SystemPara.Language,LogChannelMenuList.ChannelAddGoods[SystemPara.Language]);
				LCDPrintf(8,9,0,SystemPara.Language,LogChannelMenuList.ChannelStateLookUp[SystemPara.Language]);
				LCDPrintf(8,11,0,SystemPara.Language,LogChannelMenuList.ChannelCheck[SystemPara.Language]);
				break;
			case 0x03:
				ChannelStateLookUp();
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelManage[SystemPara.Language]);
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.ChannelAttribute[SystemPara.Language]);
				LCDPrintf(8,7,0,SystemPara.Language,LogChannelMenuList.ChannelAddGoods[SystemPara.Language]);
				LCDPrintf(8,9,0,SystemPara.Language,LogChannelMenuList.ChannelStateLookUp[SystemPara.Language]);
				LCDPrintf(8,11,0,SystemPara.Language,LogChannelMenuList.ChannelCheck[SystemPara.Language]);
				break;
			case 0x04:
				ChannelTest();
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelMenuList.ChannelManage[SystemPara.Language]);
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelMenuList.ChannelAttribute[SystemPara.Language]);
				LCDPrintf(8,7,0,SystemPara.Language,LogChannelMenuList.ChannelAddGoods[SystemPara.Language]);
				LCDPrintf(8,9,0,SystemPara.Language,LogChannelMenuList.ChannelStateLookUp[SystemPara.Language]);
				LCDPrintf(8,11,0,SystemPara.Language,LogChannelMenuList.ChannelCheck[SystemPara.Language]);
				break;
			case 0x0e:
				ChannelSaveParam();
				return;
		}
	}
}
/*********************************************************************************************************
** Function name:     	MaintainUserProcess
** Descriptions:	    ½øÈëÓÃ»§Ä£Ê½²Ëµ¥
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void MaintainUserProcess(void)
{
	unsigned char Key;
	
	void (*TestFunctonPtr)(void);
	//SYSPara.Language = 0;//Ä¬ÈÏÖÐÎÄ×ÖÌå	
	while(1)
	{
		LCDClrScreen();
		Trace("\r\nEnterUserSelfCheckFuction");	
		UserMaintainMainMenu.PageNumb	  = 0x00;
		UserMaintainMainMenu.KeyCurrent  = 0x00;
		UserMaintainMainMenu.ExtSelfCheck= 0x00;
		UserMaintainMainMenu.TestOperate = (void *)0;
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		if(UserMaintainMainMenu.PageNumb == 0x00)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserMaintainMenuList.ModeCapion[SystemPara.Language]);//±êÌâ			
			LCDPrintf(5,5,0,SystemPara.Language,UserMaintainMenuList.Record[SystemPara.Language]);//½»Ò×¼ÇÂ¼
			LCDPrintf(5,7,0,SystemPara.Language,UserMaintainMenuList.Error[SystemPara.Language]);//¹ÊÕÏ²éÑ¯
			LCDPrintf(5,9,0,SystemPara.Language,UserMaintainMenuList.Column[SystemPara.Language]);//»õµ½¹ÜÀí
			LCDPrintf(5,11,0,SystemPara.Language,UserMaintainMenuList.Device[SystemPara.Language]);//Éè±¸¹Üí		
		}
		while(1)
		{
			Key = ReadKeyValue();
			switch(Key)
			{
				case 'C' :
							LCDClrScreen();//ÍË³öÎ¬»¤²Ëµ¥ÇåÆÁ
							return;
				case '<' :	
							break;
				case '>' :	
					    	break;							
				case '1' :							
							if(UserMaintainMainMenu.PageNumb == 0x00)
							{
								UserMaintainMainMenu.KeyCurrent = 0x01;
								UserMaintainMainMenu.ExtSelfCheck = 0x00;
								UserMaintainMainMenu.TestOperate = LogMaintainProcess;
							}						
							break;
				case '2' :
							if(UserMaintainMainMenu.PageNumb == 0x00)
							{
								UserMaintainMainMenu.KeyCurrent = 0x02;
								UserMaintainMainMenu.ExtSelfCheck = 0x00;
								UserMaintainMainMenu.TestOperate = ErrorCheck;
							}
							
							break;
				case '3' :	
						if(UserMaintainMainMenu.PageNumb == 0x00)
						{
							UserMaintainMainMenu.KeyCurrent = 0x03;
							UserMaintainMainMenu.ExtSelfCheck = 0x00;
							UserMaintainMainMenu.TestOperate = deviceMange;
						}
						break;
				case '4' :													
							if(UserMaintainMainMenu.PageNumb == 0x00)
							{
								UserMaintainMainMenu.KeyCurrent = 0x04;
								UserMaintainMainMenu.ExtSelfCheck = 0x00;
								UserMaintainMainMenu.TestOperate = columnManage;
							}
						
							break;
										
				default:	
							break;	

			}

			if(UserMaintainMainMenu.ExtSelfCheck == 0x01)
			{
				Trace("\r\nSwitch exit User");
				break;
			}
			if(UserMaintainMainMenu.KeyCurrent != 0x00)
			{
				Trace("\r\nEnter  TestFunctonPtr");
				TestFunctonPtr = UserMaintainMainMenu.TestOperate;
				(*TestFunctonPtr)();//ÓÉ´Ë½øÈë¸÷¸öÉè±¸µÄ×Ô¼ìº¯Êý
				break;	
			}
		}	
		if(UserMaintainMainMenu.ExtSelfCheck == 0x01)
		{
			Trace("\r\nSwitch exit User");
			break;
		}
	}



}





/*****************************************************************************
** Function name:	DefaultSystemParaMenu	
**
** Descriptions:	ÅäÖÃ¹¤³Ì²ÎÊý	
**					
**														 			
** parameters:		
					
** Returned value:	0¡ª¡ªÍê³É
** 
*******************************************************************************/
void RstSystemPara(uint8_t pageNum,uint8_t editIndex,uint32_t keyValue)
{
	switch(pageNum)
	{
		case 0:
			//Trace("\r\n show=%ld",keyValue);	
			switch(editIndex)
			{
				case 1: 
					SYSPara.Language = keyValue;											
					break;
				case 2: 	
					SYSPara.CoinType= keyValue;						
					break;
				case 3: 
					SYSPara.DispenceType= keyValue;								
					break;
				case 4:
					SYSPara.BillType= keyValue;											
					break;	
				case 5:
					SYSPara.nPoinValue = keyValue;								
					break;	
				default:
					break;
			}
			break;
		case 1://Ó²±ÒÆ÷Í¨µÀÃæÖµ
			stDevValue.CoinValue[editIndex - 1]= keyValue;	
			break;
		case 2://ÕÒÁãÆ÷Í¨µÀÃæÖµ
			stDevValue.HpValue[editIndex - 1]= keyValue;	
			break;
		case 3://Ö½±ÒÆ÷Í¨µÀÃæÖµ
			stDevValue.BillValue[editIndex - 1]= keyValue;	
			break;
		default:
			break;
			
	}
	
}

/*********************************************************************************************************
** Function name:     	columnInitMenu
** Descriptions:	    »õµÀ³õÊ¼»¯²Ëµ¥
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/

void columnInitMenu(void)
{
	unsigned char key = 0;
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,SystemPara.Language,LogChannelUiDisp.Columnmanage[SystemPara.Language]);
	LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.ColumnRest[SystemPara.Language]);
	LCDPrintf(30,9,0,SystemPara.Language,LogChannelUiDisp.Yes[SystemPara.Language])	;
	LCDPrintf(90,9,0,SystemPara.Language,LogChannelUiDisp.Cancel[SystemPara.Language]) ;
	while(1)
	{
		key = ReadKeyValue();
		if(key == 'C')
		{
			 return;
		}
		else if(key == 'E')
		{
			columnInit(0);
			LCDClearLineDraw(0, 9,1);
			LCDPrintf(50,9,0,SystemPara.Language,LogChannelUiDisp.columnInitOK[SystemPara.Language]) ;
			OSTimeDly(300);
			return ;
		}	
		OSTimeDly(3);
	}
		
	
	
}

/*********************************************************************************************************
** Function name:     	columnLevelMenu
** Descriptions:	    ²ã¼Ü¿ª¹Ø
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/

void columnLevelMenu(void)
{
	unsigned char key = 0,levelNo = 0,editMenu = 0,enterSubMenu = 0;
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,SystemPara.Language,LogChannelUiDisp.Columnmanage[SystemPara.Language]);
	LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.EnterTrayNum[SystemPara.Language]);
	
	while(1)
	{
		key = ReadKeyValue();

		switch(key)
		{
			case '1':case '2':
				enterSubMenu = 1;
				while(enterSubMenu)
				{
					levelNo = key - '0';
					if(stHuodao[levelNo - 1].isOpen)
						LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.TrayOpen[SystemPara.Language],levelNo);
					else
						LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.TrayClose[SystemPara.Language],levelNo);
					LCDPrintf(8,7,0,SystemPara.Language,LogChannelUiDisp.Open[SystemPara.Language]);
					LCDPrintf(8,9,0,SystemPara.Language,LogChannelUiDisp.Close[SystemPara.Language]);	
					editMenu = 1;
					while(editMenu)
					{
						key = ReadKeyValue();
						switch(key)
						{
							case '1'://¿ªÆô
								hd_open_handle(1,levelNo,2,1);
								editMenu = 0;
								break;
							case '2'://¹Ø±Õ
								hd_open_handle(1,levelNo,2,0);
								editMenu = 0;
								break;
								
							case 'C':
								editMenu = 0;
								enterSubMenu = 0;
								break;
							case 'E':
								editMenu = 0;
								enterSubMenu = 0;
								break;
							default:
								break;	
						}
						OSTimeDly(3);
					}
				}
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,SystemPara.Language,LogChannelUiDisp.Columnmanage[SystemPara.Language]);
				LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.EnterTrayNum[SystemPara.Language]);
	
				break;

			case 'C':
				return;
			default:
				break;
		}
		OSTimeDly(3);
	}
		
	
	
}

/*********************************************************************************************************
** Function name:     	columnOpenMenu
** Descriptions:	    »õµ½¿ª¹Ø
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/

void columnOpenMenu(void)
{
	unsigned char key = 0,levelNo = 0,editMenu = 0,enterSubMenu = 0,levelSelected = 0,columnNo = 0;
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,SystemPara.Language,LogChannelUiDisp.Columnmanage[SystemPara.Language]);
	LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.InputColumnNo[SystemPara.Language]);
	
	while(1)
	{
		key = ReadKeyValue();

		switch(key)
		{
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':			
				if(levelSelected)
				{
					columnNo = levelNo * 10 + key - '0';
					levelSelected = 0;
					enterSubMenu = 1;
					LCDPrintf(8,5,0,SystemPara.Language,"%s%d",LogChannelUiDisp.InputColumnNo[SystemPara.Language],columnNo);		

				}
				else
				{		
					levelNo =  key - '0';
					if(levelNo <= 2)
					{
						levelSelected = 1;
						columnNo = levelNo;
						LCDPrintf(8,5,0,SystemPara.Language,"%s%d",LogChannelUiDisp.InputColumnNo[SystemPara.Language],columnNo);		

					}
					
					
				}
					
				while(enterSubMenu)
				{
					
					if(getColumnInfoByPhysic(1,columnNo,HUODAO_TYPE_ISOPEN) && stHuodao[levelNo - 1].isOpen)
						LCDPrintf(8,7,0,SystemPara.Language,LogChannelUiDisp.ColumnOpen[SystemPara.Language],levelNo);
					else
						LCDPrintf(8,7,0,SystemPara.Language,LogChannelUiDisp.ColumnClose[SystemPara.Language],levelNo);
					LCDPrintf(8,9,0,SystemPara.Language,LogChannelUiDisp.Open[SystemPara.Language]);
					LCDPrintf(8,11,0,SystemPara.Language,LogChannelUiDisp.Close[SystemPara.Language]);	
					editMenu = 1;
					while(editMenu)
					{
						key = ReadKeyValue();
						switch(key)
						{
							case '1'://¿ªÆô
								hd_open_handle(1,levelNo,1,1);
								
								editMenu = 0;
								break;
							case '2'://¹Ø±Õ
								hd_open_handle(1,levelNo,1,0);
							
								editMenu = 0;
								break;
								
							case 'C':
								editMenu = 0;
								enterSubMenu = 0;
								break;
							case 'E':
								editMenu = 0;
								enterSubMenu = 0;
								break;
							default:
								break;	
						}
						OSTimeDly(3);
					}
				}
				if(!levelSelected)
				{
					LCDClrScreen();
					LCDDrawRectangle(0,0,239,15);
					LCDDrawRectangle(0,0,239,3);
					LCDClrArea(1,4,238,14);
					LCDPrintf(8,1,0,SystemPara.Language,LogChannelUiDisp.Columnmanage[SystemPara.Language]);
					LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.InputColumnNo[SystemPara.Language]);
						
				}
				break;

			case 'C':
				
				return;
			default:
				break;
		}
		OSTimeDly(3);
	}
		
	
	
}




/*********************************************************************************************************
** Function name:     	columnSetMenu
** Descriptions:	    ÉèÖÃ»õµÀ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void columnSetMenu(void)
{
	uint8_t key= 0,flag = 0;
	void (*menuFunction)(void);
	
	while(1)
	{
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDClrArea(1,4,238,14);
		LCDPrintf(8,1,0,SystemPara.Language,LogChannelUiDisp.Columnmanage[SystemPara.Language]);
		LCDPrintf(8,5,0,SystemPara.Language,LogChannelUiDisp.ColumnRest[SystemPara.Language]);//³õÊ¼»¯
		LCDPrintf(8,7,0,SystemPara.Language,LogChannelUiDisp.columnLevelOpenOrClose[SystemPara.Language]);//²ã¿ª¹Ø
		LCDPrintf(8,9,0,SystemPara.Language,LogChannelUiDisp.ColumnOpenORClose[SystemPara.Language]);//»õµÀ¿ª¹Ø

		while(1)
		{
			key = ReadKeyValue();
			switch(key)
			{
				case '1'://³õÊ¼»¯
					flag = 1;
					menuFunction = columnInitMenu;					
					break;
				case '2'://²ãºÅÅäÖÃ
					menuFunction = columnLevelMenu;
					flag = 1;
					break;
				case '3':
					menuFunction = columnOpenMenu;
					flag = 1;
					break;
				case 'C':
					flag = 0;
					ChannelSaveParam();
					return;
				default:
					break;
			}
			
			if(flag)
			{
				(*menuFunction)();
				flag = 0;
				break;
			}
			OSTimeDly(3);
			
		}
		
		
	}

}


/*********************************************************************************************************
** Function name:     	SystemParaMenu
** Descriptions:	    ÉèÖÃÏµÍ³²ÎÊý
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void SystemParaMenu(void)
{
	uint8_t pageNum = 0;
	unsigned char Key;
	uint32_t keyValue = 0;
	unsigned char max_page = 4;	
	unsigned char editIndex = 0,isEditMenu = 0;
	unsigned int value = 0;
	LCDClrScreen();
	while(1)
	{
		
		LCDClrScreen();
		//1.ÏÔÊ¾Ò³Ãæ
		switch(pageNum)
		{
			case 0: 
				LCDPrintf(0,0,0,SYSPara.Language,"%s%d",SysMenuList.Language[SYSPara.Language],SYSPara.Language);
				LCDPrintf(0,2,0,SYSPara.Language,"%s%d",SysMenuList.CoinAcceptorType[SYSPara.Language],SYSPara.CoinType);
				LCDPrintf(0,4,0,SYSPara.Language,"%s%d",SysMenuList.CoinChangerType[SYSPara.Language],SYSPara.DispenceType);
				LCDPrintf(0,6,0,SYSPara.Language,"%s%d",SysMenuList.BillValidatorType[SYSPara.Language],SYSPara.BillType);
				LCDPrintf(0,8,0,SYSPara.Language,"%s%d",SysMenuList.DecimalNum[SYSPara.Language],SYSPara.nPoinValue);					
				break;
			case 1: //Ó²±ÒÆ÷Í¨µÀÃæÖµ
					value = (stDevValue.CoinValue[0]);
					LCDPrintf(0,0,0,SYSPara.Language,"%s%d.%02d",SysMenuList.coin1[SYSPara.Language],value/100,value%100);
					value = (stDevValue.CoinValue[1]);
					LCDPrintf(0,2,0,SYSPara.Language,"%s%d.%02d",SysMenuList.coin2[SYSPara.Language],value/100,value%100);
					value = (stDevValue.CoinValue[2]);
					LCDPrintf(0,4,0,SYSPara.Language,"%s%d.%02d",SysMenuList.coin3[SYSPara.Language],value/100,value%100);
					value = (stDevValue.CoinValue[3]);
					LCDPrintf(0,6,0,SYSPara.Language,"%s%d.%02d",SysMenuList.coin4[SYSPara.Language],value/100,value%100);
					value = (stDevValue.CoinValue[4]);

					LCDPrintf(0,8,0,SYSPara.Language,"%s%d.%02d",SysMenuList.coin5[SYSPara.Language],value/100,value%100);
					value = (stDevValue.CoinValue[5]);

					LCDPrintf(0,10,0,SYSPara.Language,"%s%d.%02d",SysMenuList.coin6[SYSPara.Language],value/100,value%100);
					value = (stDevValue.CoinValue[6]);

					LCDPrintf(0,12,0,SYSPara.Language,"%s%d.%02d",SysMenuList.coin7[SYSPara.Language],value/100,value%100);
					value = (stDevValue.CoinValue[7]);

					LCDPrintf(0,14,0,SYSPara.Language,"%s%d.%02d",SysMenuList.coin8[SYSPara.Language],value/100,value%100);
					break;	
			case 2:
					value = (stDevValue.HpValue[0]);
					LCDPrintf(0,0,0,SYSPara.Language,"%s%d.%02d",SysMenuList.hopper1[SYSPara.Language],value/100,value%100);
					value = (stDevValue.HpValue[1]);

					LCDPrintf(0,2,0,SYSPara.Language,"%s%d.%02d",SysMenuList.hopper2[SYSPara.Language],value/100,value%100);
					value = (stDevValue.HpValue[2]);

					LCDPrintf(0,4,0,SYSPara.Language,"%s%d.%02d",SysMenuList.hopper3[SYSPara.Language],value/100,value%100);				
					break;	
			case 3://Ö½±ÒÆ÷Í¨µÀÃæÖµ
			
					value = (stDevValue.BillValue[0]);
					
					LCDPrintf(0,0,0,SYSPara.Language,"%s%d.%02d",SysMenuList.bill1[SYSPara.Language],value/100,value%100);
					value = (stDevValue.BillValue[1]);

					LCDPrintf(0,2,0,SYSPara.Language,"%s%d.%02d",SysMenuList.bill2[SYSPara.Language],value/100,value%100);
					value = (stDevValue.BillValue[2]);
					LCDPrintf(0,4,0,SYSPara.Language,"%s%d.%02d",SysMenuList.bill3[SYSPara.Language],value/100,value%100);
					value = (stDevValue.BillValue[3]);
					LCDPrintf(0,6,0,SYSPara.Language,"%s%d.%02d",SysMenuList.bill4[SYSPara.Language],value/100,value%100);
					value = (stDevValue.BillValue[4]);
					LCDPrintf(0,8,0,SYSPara.Language,"%s%d.%02d",SysMenuList.bill5[SYSPara.Language],value/100,value%100);
					value = (stDevValue.BillValue[5]);
					LCDPrintf(0,10,0,SYSPara.Language,"%s%d.%02d",SysMenuList.bill6[SYSPara.Language],value/100,value%100);
					value = (stDevValue.BillValue[6]);
					LCDPrintf(0,12,0,SYSPara.Language,"%s%d.%02d",SysMenuList.bill7[SYSPara.Language],value/100,value%100);
					value = (stDevValue.BillValue[7]);
					LCDPrintf(0,14,0,SYSPara.Language,"%s%d.%02d",SysMenuList.bill8[SYSPara.Language],value/100,value%100);
					break;

			default:
				break;
		}
		
		//2.²¶×½°´¼ü
		while(1)
		{			
			Key = ReadKeyValue();
			if(Key != 0x00)
				break;			
			OSTimeDly(3);
		}
		
		switch(Key)
		{
			case 'C':	
				//Trace("\r\n 1==%d,%d,%d,%d",SystemPara.Language,SystemPara.CoinAcceptorType,SystemPara.CoinChangerType,SystemPara.BillValidatorType);
				//±£´æÏµÍ³²ÎÊý
				saveSystemParaFromFlash();			
				//Trace("\r\n ==%d,%d,%d,%d",SystemPara.Language,SystemPara.CoinAcceptorType,SystemPara.CoinChangerType,SystemPara.BillValidatorType);
			
				return;
				
			case '>':							
				if(pageNum < max_page)
					pageNum++;
				editIndex = 0;
				break;				
			case '<':							
				if(pageNum > 0)
					pageNum--;
				editIndex = 0;
				break;
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':
				editIndex = Key - '0';
				break;
			
			default:
				editIndex = 0;
				break;					
		}
		
		while(editIndex)
		{			
			//3.·´°×ÏÔÊ¾²Ëµ¥,ÒÔ¼°ÏÔÊ¾ÉÔºòµÄÊäÈëÊý×Ö
			//Trace("editIndex = %d\r\n",editIndex);			
			switch(pageNum)
			{
				case 0: 
					switch(editIndex)
					{
						case 1: 
							LCDPrintf(0,0,1,SYSPara.Language,"%s%d",SysMenuList.Language[SystemPara.Language],keyValue);
							
							break;
						case 2: 
							LCDPrintf(0,2,1,SYSPara.Language,"%s%d",SysMenuList.CoinAcceptorType[SYSPara.Language],keyValue);										
							break;
						case 3: 
							LCDPrintf(0,4,1,SYSPara.Language,"%s%d",SysMenuList.CoinChangerType[SYSPara.Language],keyValue);										
							break;
						case 4: 
							LCDPrintf(0,6,1,SYSPara.Language,"%s%d",SysMenuList.BillValidatorType[SYSPara.Language],keyValue);											
							break;
						case 5: 
							LCDPrintf(0,8,1,SYSPara.Language,"%s%d",SysMenuList.DecimalNum[SYSPara.Language],keyValue);										
							break;													
				
						default:
							break;
					}									
					break;
					
				case 1: 
					switch(editIndex)
					{							
						case 1: 
							LCDPrintf(0,0,1,SYSPara.Language,"%s%d.%02d",SysMenuList.coin1[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 2: 
							LCDPrintf(0,2,1,SYSPara.Language,"%s%d.%02d",SysMenuList.coin2[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 3: 
							LCDPrintf(0,4,1,SYSPara.Language,"%s%d.%02d",SysMenuList.coin3[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 4: 
							LCDPrintf(0,6,1,SYSPara.Language,"%s%d.%02d",SysMenuList.coin4[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 5: 
							LCDPrintf(0,8,1,SYSPara.Language,"%s%d.%02d",SysMenuList.coin5[SYSPara.Language],keyValue/100,keyValue%100);
							break;													
						case 6:
							LCDPrintf(0,10,1,SYSPara.Language,"%s%d.%02d",SysMenuList.coin6[SYSPara.Language],keyValue/100,keyValue%100);
							
							break;
						case 7:
							LCDPrintf(0,12,1,SYSPara.Language,"%s%d.%02d",SysMenuList.coin7[SYSPara.Language],keyValue/100,keyValue%100);					
							break;
						case 8:
							LCDPrintf(0,14,1,SYSPara.Language,"%s%d.%02d",SysMenuList.coin8[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						default:
							break;
					}																		
					break;
				case 2: 
					switch(editIndex)
					{	
					
						case 1: 
							LCDPrintf(0,0,1,SYSPara.Language,"%s%d.%02d",SysMenuList.hopper1[SYSPara.Language],keyValue/100,keyValue%100);					
							break;
						case 2: 
							LCDPrintf(0,2,1,SYSPara.Language,"%s%d.%02d",SysMenuList.hopper2[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 3: 
							LCDPrintf(0,4,1,SYSPara.Language,"%s%d.%02d",SysMenuList.hopper3[SYSPara.Language],keyValue/100,keyValue%100);				
							break;
						
						default:
							break;
					}	
					break;
				case 3: 
					switch(editIndex)
					{	
					
						case 1: 
							LCDPrintf(0,0,1,SYSPara.Language,"%s%d.%02d",SysMenuList.bill1[SYSPara.Language],keyValue/100,keyValue%100);
					
							break;
						case 2: 
							LCDPrintf(0,2,1,SYSPara.Language,"%s%d.%02d",SysMenuList.bill2[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 3: 
							LCDPrintf(0,4,1,SYSPara.Language,"%s%d.%02d",SysMenuList.bill3[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 4: 
							LCDPrintf(0,6,1,SYSPara.Language,"%s%d.%02d",SysMenuList.bill4[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 5: 
							LCDPrintf(0,8,1,SYSPara.Language,"%s%d.%02d",SysMenuList.bill5[SYSPara.Language],keyValue/100,keyValue%100);
							break;													
						case 6:
							LCDPrintf(0,10,1,SYSPara.Language,"%s%d.%02d",SysMenuList.bill6[SYSPara.Language],keyValue/100,keyValue%100);
					
							break;
						case 7:
							LCDPrintf(0,12,1,SYSPara.Language,"%s%d.%02d",SysMenuList.bill7[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						case 8:
							LCDPrintf(0,14,1,SYSPara.Language,"%s%d.%02d",SysMenuList.bill8[SYSPara.Language],keyValue/100,keyValue%100);
							break;
						default:
							break;
					}																		
					break;
				default:
					break;
			}

			isEditMenu = 1;
			
			//ÊäÈëÊý×Ö
			while(isEditMenu)
			{			
				Key = ReadKeyValue();				
				switch(Key)
				{
					case '0': case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
						keyValue = keyValue*10 + Key - '0';	
						//Trace("keyValue1 = %d\r\n",keyValue);
						isEditMenu = 0;
						break;
						
					case 'C':
						if(!keyValue)//Ã»ÓÐ±à¼­ Á½´ÎÈ¡ÏûÔòÌø³ö·­°×
						{
							editIndex = 0;
							isEditMenu = 0;
						}
						else
						{
							keyValue = 0;								
							isEditMenu = 0;
							//Trace("keyValue2 = %d\r\n",keyValue);
						}
							
						
						break;
					case 'E':

						RstSystemPara(pageNum,editIndex,keyValue);
						keyValue = 0;						
						//Trace("keyValue3 = %d\r\n",keyValue);
						
						isEditMenu = 0;
						editIndex = 0;
						break;
					default:					
						
						break;
				}
				
				OSTimeDly(3);					
				
			}

			
		}
		
					
		OSTimeDly(3);
		
	}  

	
}


/*********************************************************************************************************
** Function name:     	MaitainProcess
** Descriptions:	    ¹ÜÀí¼¶Î¬»¤²Ëµ¥
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void MaintainProcess(void)
{
	unsigned char Key;		
	void (*KeyFunctonPtr)(void);
	
	MaintainMainMenu.KeyEnterState  = 0x00;
	while(1)
	{
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDPrintf(8,1,0,SystemPara.Language,MaintainMenuList.ModeCapion[SYSPara.Language]);
		LCDPrintf(8,5,0,SystemPara.Language,MaintainMenuList.SystemSetCapion[SYSPara.Language]);
		LCDPrintf(8,7,0,SystemPara.Language,MaintainMenuList.columnSetCapion[SYSPara.Language]);
		while(1)
		{
			Key = ReadKeyValue();
			//Trace("Key Select = %C\r\n",Key);
			switch(Key)
			{
				case 'C' :
					LCDClrScreen();//ÍË³öÎ¬»¤²Ëµ¥ÇåÆÁ
					return;
				case '1':
					MaintainMainMenu.KeyEnterState  = 0x01;
					MaintainMainMenu.CurrentOperate = SystemParaMenu;
					break;
				case '2':
					MaintainMainMenu.KeyEnterState  = 0x01;
					MaintainMainMenu.CurrentOperate = columnSetMenu;
					break;
				case '7':
					
					readDetailTradeInfo(&stDetailTradeInfo,0);
					saveDetailTradeInfo();
					saveTradeInfo();
					break;
				default:
					break;
			}
			if(MaintainMainMenu.KeyEnterState)
			{
				MaintainMainMenu.KeyEnterState = 0x00;
				KeyFunctonPtr = MaintainMainMenu.CurrentOperate;
				(*KeyFunctonPtr)();//ÓÉ´Ë½øÈë¸÷¸ö×Ó²Ëµ¥
				
				break;
			}
			
		}
	}



}
