
/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.h
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        系统参数定义及通用函数部分                     
**------------------------------------------------------------------------------------------------------
** Created by:          yanbo
** Created date:        20134-02-18
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/


#include "..\config.h"
#include "ChannelHandle.h"
#include "MemoryHandle.h"

DETAIL_TRADE_INFO stDetailTradeInfo;


/*********************************************************************************************************
** Function name:       amountTurnByPoint
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned int amountTurnByPoint(unsigned int value)
{
	unsigned int cValue = 0;
	if(SYSPara.nPoinValue == 1)
	{
		cValue = value / 10;
	}
	else if(SYSPara.nPoinValue == 0)
	{
		cValue = value / 100;
	}
	else
		cValue = value;
	
	return cValue;
}




/*********************************************************************************************************
** Function name:       amountTurnTocents
** Descriptions:        根据小数点转换分
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned int amountTurnTocents(unsigned int value)
{
	unsigned int cValue = 0;
	if(SYSPara.nPoinValue == 1)
	{
		cValue = value * 10;
	}
	else if(SYSPara.nPoinValue == 0)
	{
		cValue = value*100;
	}
	else
		cValue = value;
	
	return cValue;
	
}

/*********************************************************************************************************
** Function name:       save_data
** Descriptions:        保存信息
** input parameters:    ptr：写入flash的数据指针，
						len 写入长度 最大长度不超过240字节
						addr写入的起始地址
** output parameters:   无
** Returned value:     0 失败   1成功
*********************************************************************************************************/
static unsigned char  save_data(void *ptr,unsigned short len,unsigned short addr)
{
	unsigned short crc;
	unsigned char buf[256] = {0};//256字节 分8次存储
	if(!ptr || !len || (len > 240))
		return 0;
	
	memcpy(buf,ptr,len);
	crc = CrcCheck(buf,len);
	buf[len + 0] = HUINT16(crc);
	buf[len + 1] = LUINT16(crc);
	len += 2;
	saveFlash(addr,buf,256);	
	return 1;

}

/*********************************************************************************************************
** Function name:       read_data
** Descriptions:        读取信息
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
static unsigned char read_data(void *ptr,unsigned short len,unsigned short addr)
{
	unsigned short crc;
	unsigned char buf[256] = {0};
	if(!ptr || !len || (len > 240))
			return 0;
	
	readFlash(addr,buf,256);	
	crc = CrcCheck(buf,len);
	if(crc == INTEG16(buf[len],buf[len + 1]))
	{
		memcpy(ptr,buf,len);
		return 1;
	}	

	return 0;
	


}



/*********************************************************************************************************
** Function name:       saveTradeInfo
** Descriptions:        保存交易记录
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

void saveTradeInfo(void)
{
	save_data((void *)&stTotalTrade,sizeof(stTotalTrade),ADD_USER_DATA);		
}

/*********************************************************************************************************
** Function name:       readTradeInfo
** Descriptions:        读取总交易记录
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

unsigned char readTradeInfo(void)
{
	return read_data((void *)&stTotalTrade,sizeof(stTotalTrade),ADD_USER_DATA);

}


/*********************************************************************************************************
** Function name:       clearTradeInfo
** Descriptions:        清交易记录
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void clearTradeInfo(void)
{
	unsigned char i,j;
	memset((void *)&stTotalTrade,0,sizeof(stTotalTrade));
	saveTradeInfo();
	for(i = 0;i < COLUMN_LEVEL_NUM;i++)
	{
		for(j = 0; j < COLUMN_ONE_LEVEL_NUM;j++)
		{
			stHuodao[i].stColumn[j].tradeCount  = 0;
		}
	}
	saveHuodaoInfo(TYPE_HUODAO_TRADE_COUNT);
	
}

/*********************************************************************************************************
** Function name:       saveDetailTradeInfo
** Descriptions:        保存详细交易记录
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void saveDetailTradeInfo(void)
{
	unsigned short addr = 0;
	unsigned short len,crc;
	unsigned char buf[ADD_TRADE_PAGE_LEN] = {0};
	//确定交易页号
	if(stTotalTrade.tradePageNo < TRADE_MAX_PAGE)
		stTotalTrade.tradePageNo++;
	else
		stTotalTrade.tradePageNo = 1;
	RTCReadTime(&stDetailTradeInfo.tradeTime);
	addr = ADD_TRADE_INFO + stTotalTrade.tradePageNo * ADD_TRADE_PAGE_LEN;	
	len = sizeof(stDetailTradeInfo);
	memcpy(buf,(void *)&stDetailTradeInfo,len);
	crc = CrcCheck(buf,len);
	buf[len + 0] = HUINT16(crc);
	buf[len + 1] = LUINT16(crc);
	saveFlash(addr,buf,ADD_TRADE_PAGE_LEN);

	//清本次交易数据信息
	memset((void *)&stDetailTradeInfo,0,sizeof(stDetailTradeInfo));
	
	
	
}

/*********************************************************************************************************
** Function name:       saveOneTradeInfo
** Descriptions:        保存一次交易信息
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void saveOneTradeInfo(unsigned char columnNo,unsigned int price,unsigned char result)
{
	unsigned char tradeIndex = stDetailTradeInfo.tradeNum;
	
	stDetailTradeInfo.columnNo[tradeIndex] = columnNo;
	stDetailTradeInfo.tradePrice[tradeIndex] = (unsigned short)price;
	stDetailTradeInfo.tradeResult[tradeIndex] = result;
	stDetailTradeInfo.tradeNum++;
	
}



/*********************************************************************************************************
** Function name:       readDetailTradeInfo
** Descriptions:        读取交易记录
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char  readDetailTradeInfo(DETAIL_TRADE_INFO *stDetail,unsigned short pageNo)
{
	unsigned char buf[ADD_TRADE_PAGE_LEN] = {0};
	unsigned short flashPage,crc,len;	
	if(!stDetail || (pageNo > TRADE_MAX_PAGE))
		return 0;
	flashPage = ADD_TRADE_INFO + pageNo * ADD_TRADE_PAGE_LEN;
	len = sizeof(DETAIL_TRADE_INFO);
	readFlash(flashPage, buf,ADD_TRADE_PAGE_LEN);
	crc = CrcCheck(buf,len);
	if(crc == INTEG16(buf[len],buf[len + 1]))
	{
		memcpy((void *)stDetail,buf,len);
		return 1;
	}
	return 0;

}



/*********************************************************************************************************
** Function name:       saveSystemParaFromFlash
** Descriptions:        将系统参数保存到flash中  4个字节存
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char saveSystemParaFromFlash()
{
	unsigned char buf[256] = {0};
	unsigned short len = 0,crc;
	
	memcpy(&buf[len],stDevValue.BillValue,8 * 4);//纸币器通道面值
	len += 8 * 4;	
	memcpy(&buf[len],stDevValue.CoinValue,8 * 4);//硬币器通道面值
	len += 8 * 4;
	memcpy(&buf[len],stDevValue.HpValue,HOPPER_NUMS * 4);//hopper通道面值
	len += HOPPER_NUMS * 4;
	memcpy(&buf[len],(unsigned char *)&SYSPara,sizeof(SYSPara));
	len += sizeof(SYSPara);
	crc = CrcCheck(buf,len);
	buf[len + 0] = HUINT16(crc);
	buf[len + 1] = LUINT16(crc);
	
	return saveFlash(ADD_SYSTEM_PARA,buf,len + 2);
	
}


/*********************************************************************************************************
** Function name:       readSystemParaFromFlash
** Descriptions:        从flash中读取系统参数 4个字节读取
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char readSystemParaFromFlash()
{
	unsigned char buf[256] = {0};
	unsigned short len = 0,crc;
	len = (8 + 8 + HOPPER_NUMS) * 4 + sizeof(SYSPara);
	readFlash(ADD_SYSTEM_PARA, buf,len + 2);
	crc = CrcCheck(buf,len);
	if(crc == INTEG16(buf[len],buf[len + 1]))
	{
		len = 0;
		memcpy(stDevValue.BillValue,&buf[len],8 * 4);//纸币器通道面值
		len += 8 * 4;	
		memcpy(stDevValue.CoinValue,&buf[len],8 * 4);//硬币器通道面值
		len += 8 * 4;
		memcpy(stDevValue.HpValue,&buf[len],HOPPER_NUMS * 4);//hopper通道面值
		len += HOPPER_NUMS * 4;
		memcpy((unsigned char *)&SYSPara,&buf[len],sizeof(SYSPara));
		len += sizeof(SYSPara);
		return 1;
	}	
	return 0;

}




/*********************************************************************************************************
** Function name:       saveHuodaoInfo
** Descriptions:        保存货道信息
** input parameters:    type：保存数据所属的类型；
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char  saveHuodaoInfo(unsigned short type)
{
	unsigned char len_th,len_lh,res1,res2,*ptr;
	unsigned short addr;

	len_th = sizeof(stHuodao) / 2;
	len_lh = sizeof(stHuodao) - len_th;
	ptr = (void *)&stHuodao;
	addr = ADD_HUODAO_INFO;
	res1 = save_data((void *)&ptr[0],len_th,addr);
	res2 = save_data((void *)&ptr[len_th],len_lh,addr + 256);
	return (res1 && res2);

}

/*********************************************************************************************************
** Function name:       readHuodaoInfo
** Descriptions:        读取货道信息
** input parameters:    type：读取数据所属的类型；
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char readHuodaoInfo(unsigned short type)
{

	unsigned char len_th,len_lh,res1,res2,*ptr;
	unsigned short addr;

	len_th = sizeof(stHuodao) / 2;
	len_lh = sizeof(stHuodao) - len_th;
	ptr = (void *)&stHuodao;
	addr = ADD_HUODAO_INFO;
	res1 = read_data((void *)&ptr[0],len_th,addr);
	res2 = read_data((void *)&ptr[len_th],len_lh,addr + 256);
	return (res1 && res2);



}







