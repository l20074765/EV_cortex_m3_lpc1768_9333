
/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           pcControl
** Last modified Date:  2013-11-21
** Last Version:         
** Descriptions:        PC管理VMC通信                 
**------------------------------------------------------------------------------------------------------
** Created by:          yoc
** Created date:       2013-11-21
** Version:               V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#include "..\config.h"
#include "..\API\ChannelHandle.h"

#if VMC_TYPE == VMC_TYPE_TRADE


#define PCMPORT		0   //串口端口select	0串口0  3串口3
#if PCMPORT == 3
#define pcm_clear_uart	     uart3_clr_buf
#define pcm_buffer_not_mpty  Uart3BuffIsNotEmpty
#define pcm_put_str			 Uart3PutStr
#define pcm_get_ch			 Uart3GetCh
	
#else
#define pcm_clear_uart	     ClrUart0Buff
#define pcm_buffer_not_mpty  Uart0BuffIsNotEmpty
#define pcm_put_str			 Uart0PutStr
#define pcm_get_ch			 Uart0GetCh

#endif


//VMC指令
#define ACK_VMCCOM 0x00
#define NAK_VMCCOM 0x01
#define VMC_PCM_POLL 0x02
#define VMC_PCM_SEEK 0x03


#define START_RPT 0x82
#define TRADE_RPT 0x83
#define CLEAR_RPT 0x84
#define SYSPARA_RPT 0x85
#define HUODAO_RPT 0x86
#define FACTORRECOVER_RPT 0x87
#define DATETIME_RPT 0x88
#define STATUS_RPT 0x89
#define DEBUG_RPT 0x8A

//PC指令
#define ACK_PCCON  0x80
#define	NAK_PCCON  0x81
#define START_REQ 0x82
#define TRADE_REQ 0x83
#define CLEAR_REQ 0x84
#define SYSPARA_REQ 0x85
#define HUODAO_REQ 0x86
#define FACTORRECOVER_REQ 0x87
#define DATETIME_REQ 0x88
#define STATUS_REQ 0x89
#define DEBUG_REQ 0x8A



static unsigned char handleType = 0xFF;//任务处理请求类型
static unsigned char mainTrace = 0;//总调试接口
static unsigned char snNo = 0,comErr = 0;//序列号 和串口错误数
static const unsigned char ack_t = 0x00,nak_t = 0x01,head = 4,
							ack = 1,nak = 2,msg = 3;
static unsigned char busyFlag = 0;
static unsigned char pcm_index = 0;




#pragma arm section rwdata = "RAM2",zidata = "RAM2"

static OS_STK pcControlTaskStk[512];
PCCONMSG vmc_pc_mail[PCM_MAX_SIZE];
PCCONMSG pc_vmc_mail;
PCM_MSG pcm_msg;

#pragma arm section


PCM_MSG *pcUartMsg;

//PC下发邮箱
void *PCMSizeQ[PCM_MAX_SIZE];
OS_EVENT *pcm_vmc_event;
OS_EVENT *vmc_pcm_event;

#define MBOX_HUODAO_INIT	0x00//货到初始化
#define MBOX_HUODAO_TEST	0x01//货道测试



/*********************************************************************************************************
** Function name:     pcEncodAmount
** Descriptions:      将32位金额编码成一字节数据
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static unsigned char pcEncodAmount(unsigned int amount)
{

	unsigned char i = 0,value;
	if(amount == 0)
		return 0;
	while(!(amount % 10))
	{
		amount = amount / 10;
		i++;
	}
	switch(amount)
	{
		case 1:
			value = 1;
			break;
		case 2:
			value = 2;
			break;
		case 5:
			value = 5;
			break;
		default:
			value = 0;
			break;
	}
	if(value)
	{
		value = (i << 4) | (value & 0x0f);
		return value;
	}
	else
		return 0;

}
/*********************************************************************************************************
** Function name:     pcAnalysisAmount
** Descriptions:      将一字节数据解析为32位金额
** input parameters:    
** output parameters:   无
** Returned value:     
*********************************************************************************************************/
static unsigned int pcAnalysisAmount(unsigned char data)
{

	unsigned int amount;
	unsigned char uint;
	if(data == 0)
		return 0;
	uint =  data >> 4;
	amount = data & 0x0f;
	
	while(uint)
	{
		amount = amount * 10;
		uint--;
	}
	return amount;
}


/*********************************************************************************************************
** Function name:     pcControlSend
** Descriptions:      发送协议
** input parameters:    type 协议类型MT，len发送长度，data发送缓冲区指针
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/
static unsigned char pcControlSend(uint8_t type,uint8_t len,uint8_t *data)
{
	
	unsigned char i = 0,rcx,send_buf[150] = {0},
		temp,dataLen,mt;	
	unsigned short crc;
	send_buf[0] = 0xEF;
	send_buf[1] = len + head;
	if(type == ack_t || type == nak_t)
		send_buf[2] = snNo;
	else
		send_buf[2] = ++snNo;
	send_buf[3] = type;
	for(i = 0;i < len;i++)
	{
		send_buf[i+head] = *data++;
	}
	   
	crc = CrcCheck(send_buf,len+head);
	send_buf[len + head] =	HUINT16(crc);
	send_buf[len + head + 1] = LUINT16(crc);
	if(type == ack_t || type == nak_t)
	{
		return ack;
	}
	rcx = 2;
retry:
	
	
	pcm_clear_uart();/*串口清*/
	OSSchedLock();
	pcm_put_str(send_buf,len+head + 2);
	//Uart0PutStr(send_buf,len+head + 2); /*发串口*/	
	OSSchedUnlock();
	
	Timer.printTimer= 20;
	while(Timer.printTimer)	/*接收ACK 超时则返回0*/
	{
		if(pcm_buffer_not_mpty() == 1)/*串口非空*/
		{	
			i = 0;
			//temp = Uart0GetCh();//head
			temp = pcm_get_ch();
			if(temp != 0xEF)//判断包头是否正确		
			{
				print_err(2);
				goto out;
			}
			pcUartMsg->recv_msg[i++] = temp;
			
			dataLen = pcm_get_ch();//len
			if(dataLen < head)//验证包大小
			{
				print_err(2);
				goto out;
			}
			else
				pcUartMsg->recv_msg[i++] = dataLen;

			temp = pcm_get_ch();//sn			
			if(temp != snNo)//序列号不对
			{
				print_err(2);
				goto out;
			}
			pcUartMsg->recv_msg[i++] = temp;

			mt = pcm_get_ch();//MT
			pcUartMsg->recv_msg[i++] = mt;
			
			//接收余下数据
			Timer.printTimer= 20;
			while(Timer.printTimer)
			{
				if(pcm_buffer_not_mpty())
				{
					temp = pcm_get_ch();
					pcUartMsg->recv_msg[i++] = temp;
					if(i >= (dataLen + 2))
						break;
				}
					
			}
			if(!Timer.printTimer)
			{
				print_warning(2);
				goto out;
			}
			crc = CrcCheck(pcUartMsg->recv_msg,dataLen); 	
			if(pcUartMsg->recv_msg[dataLen] != HUINT16(crc) || pcUartMsg->recv_msg[dataLen + 1] != LUINT16(crc)) //CRC校验不正确
			{	
				//ClrUart0Buff();
				pcm_clear_uart();				
				pcm_put_str(pcUartMsg->recv_msg,i); 
				goto out;
			}		
			if(mt == ACK_PCCON)
			{
				comErr = 0;
				return ack;
			}
			else if(mt == NAK_PCCON)
			{
				comErr = 0;
				return nak;
			}
			else
			{
				send_buf[0] = 0xEF;
				send_buf[1] = head;
				send_buf[2] = snNo;
				send_buf[3] = ACK_VMCCOM;
				crc = CrcCheck(send_buf,head); 
				send_buf[4] = HUINT16(crc);
				send_buf[5] = LUINT16(crc);
				pcm_clear_uart();
				OSSchedLock();
				pcm_put_str(send_buf,head + 2);//立即回应邋ACk
				OSSchedUnlock();				
				comErr = 0;
				pcUartMsg->recv_len = dataLen;//SF LEN SN MT + DATA
				return msg;
			}
						
		}

		//OSTimeDly(1);
				
	}
out:
	comErr++; /*超时PC连接错误加1*/
	if(rcx && type != VMC_PCM_POLL)
	{
		rcx--;
		OSTimeDly(300/5);
		goto retry;
	}
	return 0;
	
}



/*********************************************************************************************************
** Function name:     pcSeek
** Descriptions:        每秒读串口
** input parameters:    
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/

static unsigned char pcSeek(void)
{
	unsigned char rcx = 10,i,buf[10] = {0},j,len = 6;
	unsigned short crc;
	for(i = 0;i < rcx;i++)
	{
		if(pcm_buffer_not_mpty())
		{
		
			for(j = 0;j < len;j++)
			{
				buf[j] = pcm_get_ch();
				Timer.printTimer = 5;
				while(Timer.printTimer)
				{
					if(pcm_buffer_not_mpty())
						break;
				}
			}
			pcm_clear_uart();
			crc = CrcCheck(buf,len - 2);
			
			if(crc != INTEG16(buf[len - 2],buf[len - 1]))
				return 0;
			if(buf[0] == 0xEF && buf[1] == len -2 &&
				buf[3] == START_REQ)
			{
#if 0
				buf[0] = 0xEF;
				buf[1] = head;
				buf[2] = snNo;
				buf[3] = ACK_VMCCOM;
				crc = CrcCheck(buf,head); 
				buf[4] = HUINT16(crc);
				buf[5] = LUINT16(crc);
				Uart0PutStr(buf,head + 2);//立即回应邋ACk
#endif
				comErr = 0;
				return 1;
			}
			
			return 0;
		}
		OSTimeDly(2);
	}
	return 0;
}



/*********************************************************************************************************
** Function name:     pcReply
** Descriptions:        pc回应VMC
** input parameters:    
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/

static unsigned char pcReply(uint8_t type,uint8_t len,uint8_t *data)
{
	return pcControlSend(type,len,data);
}



/*********************************************************************************************************
** Function name:     	pcSendTradeReady
** Descriptions:	 回应准备打印的信息
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static unsigned char pcSendTradeReady(unsigned char type)
{

//小机型没有此功能

	unsigned char index1 = 0;
	
	
#if (VMC_MACHINE_TYPE == VMC_SMALL_TYPE)
	unsigned int vlaue;	
	pcUartMsg->send_msg[index1++] = type;
	pcUartMsg->send_msg[index1++] = HUINT16(TRADE_MAX_PAGE);
	pcUartMsg->send_msg[index1++] = LUINT16(TRADE_MAX_PAGE);
	
	pcUartMsg->send_msg[index1++] = HUINT16(stTotalTrade.tradePageNo);
	pcUartMsg->send_msg[index1++] = LUINT16(stTotalTrade.tradePageNo);
	
	pcUartMsg->send_msg[index1++] = H0UINT32(stTotalTrade.BillMoneySum + stTotalTrade.CoinMoneySum);
	pcUartMsg->send_msg[index1++] = H1UINT32(stTotalTrade.BillMoneySum + stTotalTrade.CoinMoneySum);
	pcUartMsg->send_msg[index1++] = L0UINT32(stTotalTrade.BillMoneySum + stTotalTrade.CoinMoneySum);
	pcUartMsg->send_msg[index1++] = L1UINT32(stTotalTrade.BillMoneySum + stTotalTrade.CoinMoneySum);

	

	vlaue = stTotalTrade.tradeNums;
	pcUartMsg->send_msg[index1++] = H1UINT32(vlaue);
	pcUartMsg->send_msg[index1++] = L0UINT32(vlaue);
	pcUartMsg->send_msg[index1++] = L1UINT32(vlaue);

	vlaue = stTotalTrade.tradeAmount;
	pcUartMsg->send_msg[index1++] = H0UINT32(vlaue);
	pcUartMsg->send_msg[index1++] = H1UINT32(vlaue);
	pcUartMsg->send_msg[index1++] = L0UINT32(vlaue);
	pcUartMsg->send_msg[index1++] = L1UINT32(vlaue);

	pcUartMsg->send_msg[index1++] = H0UINT32(stTotalTrade.DispenceSum);
	pcUartMsg->send_msg[index1++] = H1UINT32(stTotalTrade.DispenceSum);
	pcUartMsg->send_msg[index1++] = L0UINT32(stTotalTrade.DispenceSum);
	pcUartMsg->send_msg[index1++] = L1UINT32(stTotalTrade.DispenceSum);

	
	pcUartMsg->send_msg[index1++] = H0UINT32(0);//疑问
	pcUartMsg->send_msg[index1++] = H1UINT32(0);
	pcUartMsg->send_msg[index1++] = L0UINT32(0);
	pcUartMsg->send_msg[index1++] = L1UINT32(0);

	vlaue = stTotalTrade.iouAmount;//欠费
	pcUartMsg->send_msg[index1++] = H0UINT32(vlaue);
	pcUartMsg->send_msg[index1++] = H1UINT32(vlaue);
	pcUartMsg->send_msg[index1++] = L0UINT32(vlaue);
	pcUartMsg->send_msg[index1++] = L1UINT32(vlaue);

#else
	unsigned short maxPage;
	pcUartMsg->send_msg[index1++] = type;	
	maxPage = TRADE_PAGE_MAX;
	pcUartMsg->send_msg[index1++] = HUINT16(maxPage);
	pcUartMsg->send_msg[index1++] = LUINT16(maxPage);
	
	pcUartMsg->send_msg[index1++] = HUINT16(LogPara.LogDetailPage);
	pcUartMsg->send_msg[index1++] = LUINT16(LogPara.LogDetailPage);
	
	pcUartMsg->send_msg[index1++] = H0UINT32(LogPara.IncomeTotal);
	pcUartMsg->send_msg[index1++] = H1UINT32(LogPara.IncomeTotal);
	pcUartMsg->send_msg[index1++] = L0UINT32(LogPara.IncomeTotal);
	pcUartMsg->send_msg[index1++] = L1UINT32(LogPara.IncomeTotal);
		
	pcUartMsg->send_msg[index1++] = H1UINT32(LogPara.vpSuccessNumTotal);
	pcUartMsg->send_msg[index1++] = L0UINT32(LogPara.vpSuccessNumTotal);
	pcUartMsg->send_msg[index1++] = L1UINT32(LogPara.vpSuccessNumTotal);

	pcUartMsg->send_msg[index1++] = H0UINT32(LogPara.vpSuccessMoneyTotal);
	pcUartMsg->send_msg[index1++] = H1UINT32(LogPara.vpSuccessMoneyTotal);
	pcUartMsg->send_msg[index1++] = L0UINT32(LogPara.vpSuccessMoneyTotal);
	pcUartMsg->send_msg[index1++] = L1UINT32(LogPara.vpSuccessMoneyTotal);

	pcUartMsg->send_msg[index1++] = H0UINT32(LogPara.TotalChangeTotal);
	pcUartMsg->send_msg[index1++] = H1UINT32(LogPara.TotalChangeTotal);
	pcUartMsg->send_msg[index1++] = L0UINT32(LogPara.TotalChangeTotal);
	pcUartMsg->send_msg[index1++] = L1UINT32(LogPara.TotalChangeTotal);
	
	pcUartMsg->send_msg[index1++] = H0UINT32(LogPara.DoubtNum);
	pcUartMsg->send_msg[index1++] = H1UINT32(LogPara.DoubtNum);
	pcUartMsg->send_msg[index1++] = L0UINT32(LogPara.DoubtNum);
	pcUartMsg->send_msg[index1++] = L1UINT32(LogPara.DoubtNum);

	pcUartMsg->send_msg[index1++] = H0UINT32(LogPara.Iou);
	pcUartMsg->send_msg[index1++] = H1UINT32(LogPara.Iou);
	pcUartMsg->send_msg[index1++] = L0UINT32(LogPara.Iou);
	pcUartMsg->send_msg[index1++] = L1UINT32(LogPara.Iou);
	
#endif
	OSTimeDly(20/5);
	pcReply(TRADE_RPT,index1,pcUartMsg->send_msg); //总交易
	return 1;

			
}

/*********************************************************************************************************
** Function name:     	pcSendDetailTrade
** Descriptions:	    打印交易记录明细
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static unsigned char pcSendDetailTrade(INT8U type,INT16U index1)
{	
	//char			//出货货道编号
	uint8_t	        i,j,pay_mode,buf[255] = {0};
	uint16_t index =0;
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	DETAIL_TRADE_INFO detailTrade;
	j = j;
	pay_mode = pay_mode;
	buf[index++] = type;
	if(!index1 || !readDetailTradeInfo(&detailTrade,index1))
	{
		buf[index++] =  0;
		buf[index++] =  0;		
	}
	else
	{
		buf[index++] =  HUINT16(index1);
		buf[index++] =  LUINT16(index1);
		buf[index++] =  HEC_to_BCD(detailTrade.tradeTime.year/100) ;
		buf[index++] =  HEC_to_BCD(detailTrade.tradeTime.year%100) ;
		buf[index++] =  HEC_to_BCD(detailTrade.tradeTime.month);
		buf[index++] =  HEC_to_BCD(detailTrade.tradeTime.day);
		buf[index++] =  HEC_to_BCD(detailTrade.tradeTime.hour);
		buf[index++] =  HEC_to_BCD(detailTrade.tradeTime.minute);
		buf[index++] =  HEC_to_BCD(detailTrade.tradeTime.second);

		buf[index++] = HUINT16(detailTrade.billIncome);
		buf[index++] = LUINT16(detailTrade.billIncome);
		buf[index++] = HUINT16(detailTrade.coinIncome);
		buf[index++] = LUINT16(detailTrade.coinIncome);
		buf[index++] = HUINT16(0);
		buf[index++] = LUINT16(0);
		buf[index++] = HUINT16(detailTrade.tradePayout);
		buf[index++] = LUINT16(detailTrade.tradePayout);
		buf[index++] = detailTrade.tradeNum;
		/*
		*pay_mode 0:现金 1:一卡通支付 2银联卡支付 3PC出货 4支付宝 5游戏出货  8 0xEE其它
		*
		*/
		for(i = 0;i < detailTrade.tradeNum;i++)
		{
			buf[index++] = 1;//默认柜1			
			buf[index++] = detailTrade.columnNo[i];		
			buf[index++] = LUINT16(0x0000);			
			buf[index++] = HUINT16(detailTrade.tradePrice[i]);
			buf[index++] = LUINT16(detailTrade.tradePrice[i]);			
			buf[index++] = 0;
			buf[index++] = detailTrade.tradeResult[i];
		
		}
		
		
	}
#else
	LOGPARAMETERDETAIL detailTrade;
	buf[index++] = type;
	if(!index1 || !ReadDetailTraceAPI((char *)&detailTrade,index1))
	{
		buf[index++] =  0;
		buf[index++] =  0;	
		
	}	
	else
	{
		buf[index++] =  HUINT16(index1);
		buf[index++] =  LUINT16(index1);
		/*
		*pay_mode 0:现金 1:一卡通支付 2银联卡支付 3PC出货 4支付宝 5游戏出货  8 0xEE其它
		*
		*/
		buf[index++] =  HEC_to_BCD(detailTrade.BeginYear/100) ;
		buf[index++] =  HEC_to_BCD(detailTrade.BeginYear%100) ;
		buf[index++] =  HEC_to_BCD(detailTrade.BeginMonth);
		buf[index++] =  HEC_to_BCD(detailTrade.BeginDay);
		buf[index++] =  HEC_to_BCD(detailTrade.BeginHour);
		buf[index++] =  HEC_to_BCD(detailTrade.BeginMinute);
		buf[index++] =  HEC_to_BCD(detailTrade.BeginSec);

		buf[index++] = HUINT16(detailTrade.IncomeBill);
		buf[index++] = LUINT16(detailTrade.IncomeBill);
		buf[index++] = HUINT16(detailTrade.IncomeCoin);
		buf[index++] = LUINT16(detailTrade.IncomeCoin);
		buf[index++] = HUINT16(detailTrade.IncomeCard);
		buf[index++] = LUINT16(detailTrade.IncomeCard);
		buf[index++] = HUINT16(detailTrade.Change);
		buf[index++] = LUINT16(detailTrade.Change);
		buf[index++] = detailTrade.TradeNum;
		for(i = 0,j = 0;i < detailTrade.TradeNum;i++)
		{
			if(detailTrade.ColumnNo[j] == 'A')
			{
				buf[index++] = 1;
			}
			else if(detailTrade.ColumnNo[j] == 'B')
			{
				buf[index++] = 2;
			}
			else//其他情况
				buf[index++] = 1;
			
			buf[index++] = (INT8U)(detailTrade.ColumnNo[j+1] - '0') * 10 + (INT8U)(detailTrade.ColumnNo[j+2] - '0');
		
			buf[index++] = LUINT16(detailTrade.GoodsNo[i]);
			
			buf[index++] = HUINT16(detailTrade.PriceNo[i]);
			buf[index++] = LUINT16(detailTrade.PriceNo[i]);
			
			pay_mode = detailTrade.PayMode[i];

			buf[index++] = pay_mode;
			buf[index++] = detailTrade.TransSucc[i];
			j += 3;
		
		}
		
	}
#endif	
	OSTimeDly(20/5);
	pcReply(TRADE_RPT,index,buf);
	
	return 1;
	
}



/*********************************************************************************************************
** Function name:     	pcTradeReq
** Descriptions:	   处理PC交易请求
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcTradeReq(unsigned char len,unsigned char *data)
{
	unsigned char type,dataLen;
	if(len < 1)
	{
		print_warning(2);
		return;
	}
	dataLen = len - 1;
	type = data[0];
	switch(type)
	{
		case 0:		
			pcSendTradeReady(0);
			break;
		case 1:
			if(dataLen >= 2)
			{
				pcSendDetailTrade(1,INTEG16(data[1],data[2]));
			}			
			break;
		default:
			break;
	}
}




/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcClearReq(unsigned char len,unsigned char data)
{
	unsigned char mode,sbuf[2];
	if(len < 1)
		return ;
	mode = data;
	mode = mode;
	
	clearTradeInfo();
	OSTimeDly(20/5);
	sbuf[0] = mode;
	sbuf[1] = 0x01;
	pcReply(CLEAR_RPT,2,sbuf);
}


/*********************************************************************************************************
** Function name:     	pcSendSysPara
** Descriptions:	   发送系统参数
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSendSysPara(unsigned char type)
{
	unsigned char buf[100],index1 = 0,i;
	unsigned int temp;
	buf[index1++] = type;//type段

#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	buf[index1++] = SYSPara.Language;
	temp = SYSPara.macSN;
	index1+=4;
	buf[index1 - 1] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 2] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 3] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 4] = HEC_to_BCD(temp % 100);


	buf[index1++] = SYSPara.armSN[0];
	buf[index1++] = SYSPara.armSN[1];
	buf[index1++] = SYSPara.vmcType;

	//纸币器类型
	buf[index1++] = SYSPara.BillType;
	
	buf[index1++] = SYSPara.CoinType;
	buf[index1++] = SYSPara.DispenceType;
	buf[index1++] = 0;
	//通道面值
	for(i = 0;i < 8;i++)
	{	
		buf[index1++] = pcEncodAmount(stDevValue.BillValue[i]);			
	}
	for(i = 0;i < 8;i++)
	{
		buf[index1++] = pcEncodAmount(stDevValue.CoinValue[i]);						
	}
	for(i = 0; i < 3;i++)
	{
		buf[index1++] = pcEncodAmount(stDevValue.HpValue[i]);			
	}
	
	buf[index1++] = 255;
	//不支持 目前
	buf[index1++] = H0UINT32(0);
	buf[index1++] = H1UINT32(0);
	buf[index1++] = L0UINT32(0);
	buf[index1++] = L1UINT32(0);

	buf[index1++] = (0x01 << 4) | (0x00);//add by yoc 2013.10.8 
	buf[index1++] = (0x00 << 4) | (0x00);

	
	buf[index1++] = 0;

	buf[index1++] = SYSPara.nPoinValue;

	buf[index1++] = 0x00;
	//
	buf[index1++] = ((0x00 & 0x01) << 3) |
					((0x00 & 0x01) << 2) |
					((0x00 & 0x01) << 1) |
					((0x00 & 0x01) << 0)
					;
	
	buf[index1++] = (1 & 0x01) |
					((1 & 0x01) << 1) |
					((0 & 0x01) << 2) |
					((1 & 0x01) << 3) |
					((SYSPara.logo & 0x01) << 4) |
					((0 & 0x01) << 5) |  //选货按键板
					((0 & 0x01) << 6);
	
	
#else
	buf[index1++] = SystemPara.Language;
		//add by yoc 2013 10.25  index1 + 8
	temp = SystemPara.macSN;
	index1+=4;
	buf[index1 - 1] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 2] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 3] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 4] = HEC_to_BCD(temp % 100);	

	buf[index1++] = SystemPara.armSN[0];
	buf[index1++] = SystemPara.armSN[1];
	buf[index1++] = SystemPara.vmcType;
	if(SystemPara.BillITLValidator == 1 && SystemPara.BillValidatorType == MDB_BILLACCEPTER)
	{
		buf[index1++] = 3;
	}
	else
		buf[index1++] = SystemPara.BillValidatorType;
	
	buf[index1++] = SystemPara.CoinAcceptorType;
	buf[index1++] = SystemPara.CoinChangerType;
	buf[index1++] = SystemPara.CashlessDeviceType;
	for(i = 0;i < 8;i++)
	{
		buf[index1++] = pcEncodAmount(SystemPara.BillValue[i]);			
	}
	for(i = 0;i < 8;i++)
	{
		buf[index1++] = pcEncodAmount(SystemPara.CoinValue[i]);						
	}
	for(i = 0; i < 3;i++)
	{
		buf[index1++] = pcEncodAmount(SystemPara.HopperValue[i]);			
	}
	buf[index1++] = SystemPara.SaleTime;
	buf[index1++] = H0UINT32(SystemPara.MaxValue);
	buf[index1++] = H1UINT32(SystemPara.MaxValue);
	buf[index1++] = L0UINT32(SystemPara.MaxValue);
	buf[index1++] = L1UINT32(SystemPara.MaxValue);


	buf[index1++] = (SystemPara.Channel << 4) | (SystemPara.ChannelType & 0x0f);//add by yoc 2013.10.8 
	buf[index1++] = (SystemPara.SubChannel << 4) | (SystemPara.SubBinChannelType & 0x0f);

	if(SystemPara.EasiveEnable == 1 && SystemPara.PcEnable == 2)
	{
		buf[index1++] = 4;
	}
	else
	 	buf[index1++] = SystemPara.PcEnable;

	buf[index1++] = SystemPara.DecimalNum;

	buf[index1++] = 0x00;
	//
	buf[index1++] = ((UserPara.LEDCtr.flag & 0x01) << 3) |
					((UserPara.LEDCtr2Bin.flag & 0x01) << 2) |
					((UserPara.CompressorCtr.flag & 0x01) << 1) |
					((UserPara.CompressorCtr2Bin.flag & 0x01) << 0)
					;
	
	buf[index1++] = (SystemPara.CunhuoKou & 0x01) |
					((SystemPara.GOCIsOpen & 0x01) << 1) |
					((UserPara.TransEscape & 0x01) << 2) |
					((UserPara.TransMultiMode & 0x01) << 3) |
					((SystemPara.Logo & 0x01) << 4) |
					((SystemPara.UserSelectKeyBoard & 0x01) << 5) |
					((SystemPara.SubBinOpen & 0x01) << 6);

#endif	
		
	OSTimeDly(30/5);
	pcReply(SYSPARA_RPT,index1,buf);
	
}


/*********************************************************************************************************
** Function name:     	pcSetSysPara
** Descriptions:	    设置系统参数
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSetSysPara(unsigned char type,unsigned char *buf)
{

	unsigned char index1 = 0,i,temp,rst[3] = {0};
	unsigned int mac_sn;
	unsigned int tempValue;	
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	
	SYSPara.Language = buf[index1++];
	mac_sn = (unsigned int)BCD_to_HEC(buf[index1]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1 + 1]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1 + 2]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1 + 3]);	
	SYSPara.macSN = mac_sn;
	index1 += 4;
	
	SYSPara.armSN[0] = buf[index1++];
	SYSPara.armSN[1] = buf[index1++];
	SYSPara.vmcType = buf[index1++];	

	temp = buf[index1++];
	SYSPara.BillType = temp;		
	SYSPara.CoinType = buf[index1++];
	SYSPara.DispenceType = buf[index1++];
	
	temp = buf[index1++];
	
	for(i = 0;i < 8;i++)
	{
		stDevValue.BillValue[i] = pcAnalysisAmount(buf[index1++]);
		
				
	}
	for(i = 0;i < 8;i++)
	{
	
		stDevValue.CoinValue[i] = pcAnalysisAmount(buf[index1++]);
	}
	for(i = 0; i < 3;i++)
	{
		stDevValue.HpValue[i] = pcAnalysisAmount(buf[index1++]);
	}
	temp = buf[index1++];		//reserved
	
	tempValue = INTEG32(buf[index1++],buf[index1++],
		buf[index1++],buf[index1++]);//reserved
	tempValue = tempValue;   	

	temp = buf[index1++];//reserved	
	
	temp = buf[index1++];//reserved
	

	temp = buf[index1++];//reserved


	SYSPara.nPoinValue = buf[index1++];

	temp = buf[index1++];//reserved
	
	temp = buf[index1++];//reserved
	
		
	
	temp = buf[index1++];
	//SystemPara.CunhuoKou = temp & 0x01;
	//SystemPara.GOCIsOpen = 		(temp >> 1) & 0x01;
	//UserPara.TransEscape = 	(temp >> 2) & 0x01;
	//UserPara.TransMultiMode = (temp >> 3) & 0x01;
	SYSPara.logo = 			(temp >> 4) & 0x01;
	//SystemPara.UserSelectKeyBoard = (temp >> 5) & 0x01;
	//SystemPara.SubBinOpen = 	(temp >> 6) & 0x01;

	saveSystemParaFromFlash();

#else
	
	SystemPara.Language = buf[index1++];
	mac_sn = (unsigned int)BCD_to_HEC(buf[index1]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1+ 1]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1+ 2]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1+ 3]);	
	SystemPara.macSN = mac_sn;
	
	index1 += 4;
	//SystemPara.macSN = INTEG32(buf[index1++],buf[index1++],
	//	buf[index1++],buf[index1++]);
	SystemPara.armSN[0] = buf[index1++];
	SystemPara.armSN[1] = buf[index1++];
	SystemPara.vmcType = buf[index1++];	

	temp = buf[index1++];
	if(temp == 3)
	{
		SystemPara.BillITLValidator = 1;
		SystemPara.BillValidatorType = MDB_BILLACCEPTER;
	}
	else
	{
		SystemPara.BillValidatorType = temp;
		SystemPara.BillITLValidator = 0;
	}
		

	SystemPara.CoinAcceptorType = buf[index1++];
	SystemPara.CoinChangerType = buf[index1++];
	SystemPara.CashlessDeviceType = buf[index1++];
	
	for(i = 0;i < 8;i++)
	{
		SystemPara.BillValue[i] = pcAnalysisAmount(buf[index1++]);
				
	}
	for(i = 0;i < 8;i++)
	{
		SystemPara.CoinValue[i] = pcAnalysisAmount(buf[index1++]);	
	}
	for(i = 0; i < 3;i++)
	{
		SystemPara.HopperValue[i] = pcAnalysisAmount(buf[index1++]);		
	}
	SystemPara.SaleTime = buf[index1++];
	SystemPara.MaxValue = INTEG32(buf[index1++],buf[index1++],
		buf[index1++],buf[index1++]);

	temp = buf[index1++];
	SystemPara.Channel = (temp & 0xf0) >> 4;
	SystemPara.ChannelType = temp & 0x0f;
	
	temp = buf[index1++];
	SystemPara.SubChannel = (temp & 0xf0) >> 4;
	SystemPara.SubBinChannelType = temp & 0x0f;

	temp = buf[index1++];
	if(temp == 4)
	{
		SystemPara.EasiveEnable = 1;
		SystemPara.PcEnable = 2;
	}
	else
	{
		SystemPara.PcEnable = temp;
		SystemPara.EasiveEnable = 0;
	}	
	SystemPara.DecimalNum = buf[index1++];

	temp = buf[index1++];
	
	temp = buf[index1++];
	UserPara.CompressorCtr2Bin.flag = temp & 0x01;
	UserPara.CompressorCtr.flag = (temp >> 1) & 0x01;
	
	UserPara.LEDCtr2Bin.flag = (temp >> 2) & 0x01;
	UserPara.LEDCtr.flag = (temp >> 3) & 0x01;
	
		
	
	temp = buf[index1++];
	SystemPara.CunhuoKou = temp & 0x01;
	SystemPara.GOCIsOpen = 		(temp >> 1) & 0x01;
	UserPara.TransEscape = 	(temp >> 2) & 0x01;
	UserPara.TransMultiMode = (temp >> 3) & 0x01;
	SystemPara.Logo = 			(temp >> 4) & 0x01;
	SystemPara.UserSelectKeyBoard = (temp >> 5) & 0x01;
	SystemPara.SubBinOpen = 	(temp >> 6) & 0x01;
	
	WriteDefaultSystemPara(SystemPara);
	WriteUserSystemPara(UserPara);

#endif	
	OSTimeDly(20/5);	
	rst[0] = type;
	rst[1] = 1;
	
	pcReply(SYSPARA_RPT,2,rst);
	
}

/*********************************************************************************************************
** Function name:     	pcSysParaReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static unsigned char pcSysParaReq(unsigned char len,unsigned char *data)
{

	switch(data[0])
	{
		case 0x00:
			pcSendSysPara(0);
			break;
		case 0x01:
			pcSetSysPara(1,&data[1]);
			break;
	}

	return len;
}


/*********************************************************************************************************
** Function name:     	pcSendHuoDao
** Descriptions:	    发送货道信息
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSendHuoDao(unsigned char type,unsigned char *data)
{
	unsigned char buf[120] ={0},index = 0,i,physicNo,no,levelOpen,cabinet;
	unsigned int temp32;
	buf[index++] = type;
	//柜号
	cabinet = data[0];
	buf[index++] = data[0];
	//发送方式
	buf[index++] = data[1];
	
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE

	//层号 or 货道号
	no = data[2];
	if(data[1] == 0x01)//level
	{
		//第level层  目前小机型 只有一层
		buf[index++] = no;
		if(no && (no <= COLUMN_LEVEL_NUM))
			levelOpen = stHuodao[no - 1].isOpen;
		else
			levelOpen = 0 ;
		
		buf[index++] = levelOpen;
		if(levelOpen)
		{
			for(i =0;i< 10;i++)
			{	
				//获取物理货道
				if(i >= 9)  			
					physicNo = no * 10;
				else
					physicNo = no * 10 + i + 1;				
				buf[index++] = physicNo;//货道编号
				
				buf[index++] = fromPhysicGetLogicNum(cabinet,physicNo);
				buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_ISOPEN);//货道开启标志
				temp32 = getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_PRICE);
				buf[index++] = L0UINT32(temp32);//单价
				buf[index++] = L1UINT32(temp32);//单价	
				buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_STATE);//状态
				buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_REMAIN);//剩余数量			
				buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_TRADE_MAXCOUNT);//最大存货量
				buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_TRADE_NUM);//销售量			
				buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,0xFF);;//ID									
			}			
		}
		else
		{
			for(i =0;i< 10;i++)
			{
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
			}		
			
		}				
		
	}
	else if(data[1] == 0x02)//aisle
	{
			physicNo = no ;
			buf[index++] = physicNo;	
			buf[index++] = fromPhysicGetLogicNum(cabinet,physicNo);
			buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_ISOPEN);//货道开启标志
			temp32 = getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_PRICE);
			buf[index++] = L0UINT32(temp32);//单价
			buf[index++] = L1UINT32(temp32);//单价	
			buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_STATE);//状态
			buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_REMAIN);//剩余数量			
			buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_REMAIN);//最大存货量
			buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,HUODAO_TYPE_TRADE_NUM);//销售量			
			buf[index++] = (unsigned char)getColumnInfoByPhysic(cabinet,physicNo,0xFF);;//ID													
	}
	else
	{
		print_warning(no);
		return;
	}

	//print_log("index= %d\r\n",index);
	//for(j = 0;j < index;j++)
	//	print_log("%x ",buf[j]);
	//print_log("\r\n");
#else	

	//层号 or 货道号
	no = data[2];
	if(data[1] == 0x01)//level
	{
		//第level层
		buf[index++] = no;	
		levelOpen = GetLevelOrColumnState(cabinet,2,no);
	    buf[index++] = levelOpen;
		if(levelOpen)
		{
			for(i =0;i< 10;i++)
			{	
				//获取逻辑货道
				if(i >= 9)  			
					physicNo = no * 10;
				else
					physicNo = no * 10 + i + 1;				
				buf[index++] = physicNo;	//货道编号
				buf[index++] = FromPhysicGetLogicNum(cabinet,physicNo);
				buf[index++] = GetLevelOrColumnState(cabinet,1,physicNo);//货道开启标志
				temp32 = GetColumnStateByPhysic(cabinet,physicNo,1);
				buf[index++] = L0UINT32(temp32);//单价
				buf[index++] = L1UINT32(temp32);//单价
				buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,3);//状态
				buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,2);//剩余数量			
				buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,4);//最大存货量
				buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,5);//销售量			
				buf[index++] = (unsigned char)GetColumnStateByPhysic(cabinet,physicNo,7);//ID									
			}			
		}
		else
		{
			for(i =0;i< 10;i++)
			{
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
			}		
			
		}				
		
	}
	else if(data[1] == 0x02)//aisle
	{
			physicNo = no ;
			buf[index++] = physicNo;	
			buf[index++] = FromPhysicGetLogicNum(cabinet,physicNo);
			buf[index++] = GetLevelOrColumnState(cabinet,1,physicNo);//货道开启标志			
			temp32 = GetColumnStateByPhysic(cabinet,physicNo,1);
			buf[index++] = L0UINT32(temp32);//单价
			buf[index++] = L1UINT32(temp32);//单价
			buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,3);//状态
			buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,2);//剩余数量			
			buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,4);//最大存货量
			buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,5);//销售量			
			buf[index++] = (unsigned char)GetColumnStateByPhysic(cabinet,physicNo,7);//ID		
	}
	else
	{
		print_warning(no);
		return;
	}

	print_log("index= %d\r\n",index);
	for(j = 0;j < index;j++)
		print_log("%x ",buf[j]);
	print_log("\r\n");

#endif	
	OSTimeDly(20/5);
	pcReply(HUODAO_RPT,index,buf);
		
}



/*********************************************************************************************************
** Function name:     	pcSetHuoDao
** Descriptions:	   设置货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void  pcSetHuoDao(unsigned char type,unsigned char *buf)
{
	unsigned char index = 0,index1 = 0,reqMode,sBuf[10] = {0},cabinet,no,
	aisleType,phsicyNo,i,j;
	
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	cabinet = buf[index++];
	reqMode = buf[index++];
	no = buf[index++];//层号 or 货道物理编号
	sBuf[index1++] = type;
	sBuf[index1++] = cabinet;
	sBuf[index1++] = reqMode;

	Trace("reqMode = %d,no = %d\r\n",reqMode,no);

	j = j;
	if(reqMode == 0x01)//level
	{	
		if(no <= COLUMN_LEVEL_NUM)//小机型只有一层
		{
			aisleType = 8;
			for(i =0;i< aisleType;i++)
			{	
				
				phsicyNo = buf[index++];
				Trace("phsicyNo = %d\r\n",phsicyNo);
				setColumnInfoByPhysic(cabinet,phsicyNo,INTEG16(buf[index++],buf[index++]),HUODAO_TYPE_PRICE);
				setColumnInfoByPhysic(cabinet,phsicyNo,buf[index++],HUODAO_TYPE_STATE);
				setColumnInfoByPhysic(cabinet,phsicyNo,buf[index++],HUODAO_TYPE_REMAIN);
				setColumnInfoByPhysic(cabinet,phsicyNo,buf[index++],HUODAO_TYPE_TRADE_MAXCOUNT);
				setColumnInfoByPhysic(cabinet,phsicyNo,buf[index++],0xFF);

				
			}
		}
		
		
	}
	else if(reqMode == 0x02)//aisle
	{	
		phsicyNo = no;		
		setColumnInfoByPhysic(cabinet,phsicyNo,INTEG16(buf[index++],buf[index++]),HUODAO_TYPE_PRICE);
		setColumnInfoByPhysic(cabinet,phsicyNo,buf[index++],HUODAO_TYPE_STATE);
		setColumnInfoByPhysic(cabinet,phsicyNo,buf[index++],HUODAO_TYPE_REMAIN);
		setColumnInfoByPhysic(cabinet,phsicyNo,buf[index++],HUODAO_TYPE_REMAIN);
		setColumnInfoByPhysic(cabinet,phsicyNo,buf[index++],0xFF);	
		
	}
	else
	{
		print_warning(reqMode);
		return;
	}
	
	sBuf[index1++] = 0x01;
#else
	cabinet = buf[index++];
	reqMode = buf[index++];
	no = buf[index++];//层号 or 货道物理编号
	sBuf[index1++] = type;
	sBuf[index1++] = cabinet;
	sBuf[index1++] = reqMode;
	
	if(reqMode == 0x01)//level
	{
		aisleType = (SystemPara.ChannelType == 1) ? 10 : 8;
		for(i =0;i< aisleType;i++)
		{	
			print_log("cabinet = %d,index = %d\r\n",cabinet,index);
			for(j = 0; j < 7;j++)
				print_log("%x ",buf[index + j]);
			print_log("\r\n");
			
			phsicyNo = buf[index++];
			SetColumnParamByPhysic(cabinet,phsicyNo,1,INTEG16(buf[index++],buf[index++]));
			SetColumnParamByPhysic(cabinet,phsicyNo,3,buf[index++]);
			SetColumnParamByPhysic(cabinet,phsicyNo,2,buf[index++]);
			SetColumnParamByPhysic(cabinet,phsicyNo,4,buf[index++]);
			SetColumnParamByPhysic(cabinet,phsicyNo,7,buf[index++]);
			
			
		
		}
		
	}
	else if(reqMode == 0x02)//aisle
	{	
		phsicyNo = no;		
		SetColumnParamByPhysic(cabinet,phsicyNo,1,INTEG16(buf[index++],buf[index++]));
		SetColumnParamByPhysic(cabinet,phsicyNo,3,buf[index++]);
		SetColumnParamByPhysic(cabinet,phsicyNo,2,buf[index++]);
		SetColumnParamByPhysic(cabinet,phsicyNo,4,buf[index++]);
		SetColumnParamByPhysic(cabinet,phsicyNo,7,buf[index++]);		
		
	}
	else
	{
		print_warning(reqMode);
		return;
	}
	
	sBuf[index1++] = 0x01;


#endif

	OSTimeDly(20/5);
	pcReply(HUODAO_RPT,index1,sBuf);

	
}



/*********************************************************************************************************
** Function name:     	pcInitHuoDaoRpt
** Descriptions:	   初始化货道结果上报
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcInitHuoDaoRpt(unsigned type,unsigned char result)
{
	unsigned char rst[3];
	rst[0] = type;
	
	rst[1] = result;
	pcReply(HUODAO_RPT,3,rst);
	busyFlag = 0;
	
}

/*********************************************************************************************************
** Function name:     	pcInitAisle
** Descriptions:	   初始化货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcInitHuoDao(unsigned char type,unsigned char *data)
{	
	unsigned char cabinet,rst[3];
	cabinet = data[0];
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	columnInit(0);
#else
	ChannelResetinit();
#endif
	rst[0] = type;
	rst[1] = cabinet;
	rst[2] = 0x01;
	pcReply(HUODAO_RPT,3,rst);	



}




/*********************************************************************************************************
** Function name:     	pcSaveHuodao
** Descriptions:	   保存货道信息到FLASH
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSaveHuodao(unsigned char type,unsigned char cabinet)
{
	unsigned char rst[4];
	ChannelSaveParam();
	rst[0] = type;
	rst[1] = cabinet;
	rst[2] = 0x01;
	pcReply(HUODAO_RPT,3,rst);	
}


/*********************************************************************************************************
** Function name:     	pcONOFFHuodao
** Descriptions:	   关闭货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcONOFFHuodao(unsigned char type, unsigned char *buf)
{
	unsigned char  i,j,index = 0,aisleType,leveOpen = 0,logicNo,aisleOpen,rst[5],cabinet; 
	unsigned short temp;
	unsigned char  physicNo;
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	logicNo = logicNo;
	aisleType = aisleType;
	//柜号
	cabinet = buf[index++];
	
	for(i = 0; i < COLUMN_LEVEL_NUM;i++)//小机型 只需要一层
	{
		
		logicNo = (i + 1) * 10 + 1;
		temp = INTEG16(buf[index++],buf[index++]);
		leveOpen = (temp >> 15) & 0x01;
		if(leveOpen)
		{
			hd_open_handle(1,i + 1,2,1);
		}	
		else
			hd_open_handle(1,i + 1,2,0);
		if(leveOpen)
		{
			for(j = 0;j < COLUMN_ONE_LEVEL_NUM;j++)
			{				
				if(j >= 9)
					physicNo = (i + 1) * 10;
				else
					physicNo = (i + 1) * 10 + j + 1;						
				aisleOpen = (temp >> j) & 0x01;
				hd_open_handle(1,physicNo,1,aisleOpen);
			}
			
		}				
	}
	ChannelSaveParam();
#else
	aisleType = (SystemPara.ChannelType == 0) ? 8: 10;
	//柜号
	cabinet = buf[index++];
	
	for(i = 0; i < 8;i++)
	{
		temp = INTEG16(buf[index++],buf[index++]);
		leveOpen = (temp >> 15) & 0x01;
		if(leveOpen)
		{
			//old can't execute open
			ChannelExChannge(i + 1,2,1,1);
		}	
		else
			ChannelExChannge(i + 1,2,2,1);
		
		if(leveOpen)
		{
			for(j = 0;j < aisleType;j++)
			{				
				if(j >= 9)
					logicNo = (i + 1) * 10;
				else
					logicNo = (i + 1) * 10 + j + 1;						
				aisleOpen = (temp >> j) & 0x01;
				if(aisleOpen)
				{
					ChannelExChannge(logicNo,1,1,1);	
				}
				else
				{
					ChannelExChannge(logicNo,1,2,1);	
				}								
			}
		}				
	}
	ChannelSaveParam();


#endif	
	rst[0] = type;
	rst[1] = cabinet;
	rst[2] = 0x01;
	pcReply(HUODAO_RPT,3,rst);

	
}

static void pcAddGoods(unsigned char type,unsigned char *data)
{										  
	unsigned char addGoodsType,cabinet,no,err,
		    rbuf[10] = {0},index = 0;

#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	cabinet = data[0];	
	addGoodsType = data[1];
	no = data[2];
	rbuf[index++] = type;
	rbuf[index++] = cabinet;
	rbuf[index++] = addGoodsType;
	switch(addGoodsType)
	{
		case 0://all
			err = addColumnGoods(cabinet,1,no);
			break;

		case 1://level
			err = addColumnGoods(cabinet,2,no);
			break;

		case 2://aisle
			err = addColumnGoods(cabinet,3,no);
			break;

		default:
			err = 0;
		break;
	}
	if(err)
	{
		ChannelSaveParam();
		rbuf[index++] = 0x01;
	}
	else
	{
		rbuf[index++] = 0x00;
	}
	
#else

	cabinet = data[0];	
	addGoodsType = data[1];
	no = data[2];
	rbuf[index++] = type;
	rbuf[index++] = cabinet;
	rbuf[index++] = addGoodsType;
	switch(addGoodsType)
	{
		case 0://all
			err = AddColumnGoods(cabinet,3,no);
			break;

		case 1://level
			err = AddColumnGoods(cabinet,2,no);
			break;

		case 2://aisle
			err = AddColumnGoods(cabinet,1,no);
			break;

		default:
			err = 0;
		break;
	}
	if(err)
	{
		ChannelSaveParam();
		rbuf[index++] = 0x01;
	}
	else
	{
		rbuf[index++] = 0x00;
	}
	
#endif	
	pcReply(HUODAO_RPT,index,rbuf);
}

/*********************************************************************************************************
** Function name:     	pcTestAisle
** Descriptions:	   测试货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcTestAisle(unsigned char type,unsigned char *data)
{

	unsigned char err,err1,rst = 0,i,cabinet;
	
	pc_vmc_mail.msg[0] = type;//类型
	pc_vmc_mail.msg[1] = data[0];//柜号
	pc_vmc_mail.msg[2] = data[1];	//逻辑号


#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	err1  =err1;
	i = i;
	cabinet = pc_vmc_mail.msg[1];	
	//selectNo = fromLogicNoGetSelectNo(cabinet,pc_vmc_mail.msg[2]);
	err = ColumnTestByPcPost(cabinet,pc_vmc_mail.msg[2]);
	if(err == 1)//发送成功  处理请求
	{
		busyFlag = 1;
		handleType = HUODAO_REQ;
		Timer.pcm_handle_timeout = 3000; //30秒超时
		
		rst = 1;
	}
		
#else
	print_log("pcTestAisle :%d %d %d\r\n",
		pc_vmc_mail.msg[0],pc_vmc_mail.msg[1],pc_vmc_mail.msg[2]);

	cabinet = pc_vmc_mail.msg[1];
	if((cabinet == 1 && SystemPara.Channel == 2) ||
			(cabinet == 2 && SystemPara.SubChannel == 2 && SystemPara.SubBinOpen))
	{
		err = LiftTableTestByPcPost(pc_vmc_mail.msg[1],pc_vmc_mail.msg[2],LIFTTABLE_CHECKHANDLE);
		if(err == 1)
		{
			for(i = 0;i < 10;i++)
			{
				err1 = LiftTableTestByPcPend(LIFTTABLE_CHECKHANDLE);
				if(err1 != 0xFF)
					break;
			}			
			if(err1 == 1)
			{
				err1 = LiftTableTestByPcPost(pc_vmc_mail.msg[1],pc_vmc_mail.msg[2],LIFTTABLE_OUTHANDLE);
				if(err1 == 1)//自此升降机出货命令发送成功
				{
					busyFlag = 1;
					handleType = HUODAO_REQ;
					Timer.pcm_handle_timeout = 100; //100秒超时
					rst = 1;
				}
			}
		}
	}
	else
	{
		err = ColumnTestByPcPost(pc_vmc_mail.msg[1],pc_vmc_mail.msg[2]);
		if(err == 1)//发送成功  处理请求
		{
			busyFlag = 1;
			handleType = HUODAO_REQ;
			Timer.pcm_handle_timeout = 30; //30秒超时
			rst = 1;
		}
		// 失败
	}

#endif	
	if(!rst)//失败
	{
		pc_vmc_mail.msg[3] = 0x00;
		pcReply(HUODAO_RPT,4,pc_vmc_mail.msg);
	}
	
	
	
}
/*********************************************************************************************************
** Function name:     	pcTesttingAisle
** Descriptions:	    主任务处理货道测试
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void pcTesttingAisle(PCCONMSG *mail)
{
	unsigned char type,cabinet,logicNo,err;
	type = mail->msg[0];
	cabinet = mail->msg[1];
	logicNo = mail->msg[2];
	//if(SystemPara.Channel)
	//err = TestColumnByPC(cabinet,logicNo);
	vmc_pc_mail[pcm_index].msg[0] = type;
	vmc_pc_mail[pcm_index].msg[1] = cabinet;
	vmc_pc_mail[pcm_index].msg[2] = logicNo;
	vmc_pc_mail[pcm_index].msg[3] = err;
	vmcPost(MBOX_HUODAO_TEST);
}
/*********************************************************************************************************
** Function name:     	pcTestAisleRpt
** Descriptions:	   测试货道结果上报
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcTestAisleRpt(PCCONMSG *mail)
{

#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	unsigned char sendBuf[10] = {0},result,cabinet,err;	
	err = err;	
	cabinet = cabinet;
	if(mail == NULL)
	{
		print_warning(2);
		busyFlag = 0;
		return;
	}
	else
	{
		result = ColumnTestByPcPend();	
		if(result != 0xFF)
		{
			sendBuf[0] = mail->msg[0];
			sendBuf[1] = mail->msg[1];
			sendBuf[2] = mail->msg[2];
			sendBuf[3] = result;	
			pcReply(HUODAO_RPT,4,sendBuf);	
			busyFlag = 0;
		}
	}
#else
	unsigned char sendBuf[10] = {0},result,cabinet,err;		
	if(mail == NULL)
	{
		print_warning(2);
		busyFlag = 0;
		return;
	}
	else
	{
		cabinet = mail->msg[1];
		if((cabinet == 1 && SystemPara.Channel == 2) ||
			(cabinet == 2 && SystemPara.SubChannel == 2 && SystemPara.SubBinOpen))
		{
			err = LiftTableTestByPcPend(LIFTTABLE_CHECKHANDLE);
			if(err == 1 || err == 2)
			{
				result = 0;
			}
			else if(err == 0)
			{
				result = 1;
			}
			else
				result = 0xFF;
			
			if(result != 0xFF)
			{
				sendBuf[0] = mail->msg[0];
				sendBuf[1] = mail->msg[1];
				sendBuf[2] = mail->msg[2];
				sendBuf[3] = result;	
				pcReply(HUODAO_RPT,4,sendBuf);	
				busyFlag = 0;
			}
		}
		else
		{
			result = ColumnTestByPcPend();	
			if(result != 0xFF)
			{
				sendBuf[0] = mail->msg[0];
				sendBuf[1] = mail->msg[1];
				sendBuf[2] = mail->msg[2];
				sendBuf[3] = result;	
				pcReply(HUODAO_RPT,4,sendBuf);	
				busyFlag = 0;
			}
		}
		
	}
#endif
	
}


/*********************************************************************************************************
** Function name:     	pcsendKeyNo
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcsendKeyNo(unsigned char type,unsigned char *data)
{


#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
#else
	unsigned char index = 0,rbuf[80] = {0},cabinet,recvIndex = 0,sendType,aisleNo;
	unsigned char i,j,physicNo;
	
	cabinet = data[recvIndex++];	
	sendType = data[recvIndex++];
	aisleNo = data[recvIndex++];
	//类型 0x07
	rbuf[index++] = type;
	//柜号
	rbuf[index++] = cabinet;
	//发送类型
	rbuf[index++] = sendType;
	if(sendType == 0x00)
	{
		//全货道选货按键板
		for(i = 0;i < 8;i++)
		{
			for(j =0;j< 10;j++)
			{	
				//获取物理货道
				if(j >= 9)  			
					physicNo = (i + 1) * 10;
				else
					physicNo = (i + 1) * 10 + j + 1;				
				rbuf[index++] = GetColumnStateByPhysic(cabinet,physicNo,6);
			}
		}	
	}
	else
	{
		aisleNo =aisleNo;
	}
  	pcReply(HUODAO_RPT,index,rbuf);
#endif
	

}


/*********************************************************************************************************
** Function name:     	pcsetKeyNo
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcsetKeyNo(unsigned char type,unsigned char *data)
{
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE



#else
	unsigned char index = 0,rbuf[80] = {0},cabinet,recvIndex = 0,sendType,physicNo;
	unsigned char i,j;
	cabinet = data[recvIndex++];
	sendType = data[recvIndex++];
	rbuf[index++] = type;
	rbuf[index++] = cabinet;
	rbuf[index++] = sendType;
	if(sendType == 0x00)
	{
		//全货道选货按键板
		for(i = 0;i < 8;i++)
		{
			for(j =0;j< 10;j++)
			{	
				//获取逻辑货道
				if(j >= 9)  			
					physicNo = (i + 1) * 10;
				else
					physicNo = (i + 1) * 10 + j + 1;					
				SetColumnParamByPhysic(cabinet,physicNo,6,data[recvIndex++]);
			}
		}	
		rbuf[index++] = 0x01;
	}
	else
	{
	
	}
	pcReply(HUODAO_RPT,index,rbuf);
#endif

	
}


/*********************************************************************************************************
** Function name:     	pcSysParaReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcHuodaoReq(unsigned char len,unsigned char *data)
{
	if(len < 1)
	{
		print_warning(len);
		return;
	}
		
	switch(data[0])
	{
		case 0x00:			
			pcSendHuoDao(0,&data[1]);
			break;
		case 0x01:
			pcSetHuoDao(0x01,&data[1]);
			break;
		case 0x02:
			pcInitHuoDao(0x02,&data[1]);
			break;
		case 0x03:
			pcSaveHuodao(0x03,data[1]);
			break;
		case 0x04:
			pcONOFFHuodao(0x04,&data[1]);
			break;
		case 0x05:
			pcAddGoods(0x05,&data[1]);
			break;
		case 0x06:
			pcTestAisle(0x06,&data[1]);
			break;
		case 0x07:
			pcsendKeyNo(0x07,&data[1]);
			break;
		case 0x08:
			pcsetKeyNo(0x08,&data[1]);
			break;
		default:
			break;
	}
}
/*********************************************************************************************************
** Function name:     	pcFactorReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcFactorReq(void)
{
	unsigned char rst = 1;
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	//factoryDefaultSystemPara();
#else
	FactoryDefaultSystemPara();
#endif
	pcReply(FACTORRECOVER_RPT,1,&rst);
}


/*********************************************************************************************************
** Function name:     	pcDateTimeReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcDateTimeReq(unsigned char len,unsigned char *buf)
{
	unsigned char type,index1 = 0,sbuf[10] = {0}, i;
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
	RTC_DATE rtc;
	for(i = 0;i < 10;i++)
	{
		print_log("%x ",buf[i]);
	}
	print_log("\r\n");
	

	if(len < 1)
	{
		print_warning(len);
		return; 		
	}
	type = buf[0];
	sbuf[index1++] = type;
	if(type == 0x00)//set Time
	{	
		
		rtc.year = (unsigned short)BCD_to_HEC(buf[1])*100+ BCD_to_HEC(buf[2]);
		rtc.month = BCD_to_HEC(buf[3]);
		rtc.day = BCD_to_HEC(buf[4]);
		rtc.hour = BCD_to_HEC(buf[5]);
		rtc.minute = BCD_to_HEC(buf[6]);
		rtc.second = BCD_to_HEC(buf[7]);
		print_log("dateSet:%d,%d,%d,%d,%d,%d\r\n",rtc.year ,
		rtc.month,rtc.day,rtc.hour,rtc.minute,rtc.second);
	//设置时间不准调度		
	OSSchedLock();
		RTCSetTime(&rtc);
	OSSchedUnlock();
		RTCReadTime(&rtc);
		print_log("dateSet:%d,%d,%d,%d,%d,%d\r\n",rtc.year ,
			rtc.month,rtc.day,rtc.hour,rtc.minute,rtc.second);
		sbuf[index1++] = 0x01;
	}
	else if(type == 0x01)//get Time
	{
		RTCReadTime(&rtc);
		sbuf[index1++] =  HEC_to_BCD(rtc.year/100) ;
		sbuf[index1++] =  HEC_to_BCD(rtc.year%100) ;
		sbuf[index1++] =  HEC_to_BCD(rtc.month);
		sbuf[index1++] =  HEC_to_BCD(rtc.day);
		sbuf[index1++] =  HEC_to_BCD(rtc.hour);
		sbuf[index1++] =  HEC_to_BCD(rtc.minute);
		sbuf[index1++] =  HEC_to_BCD(rtc.second);
	}
	
#else
	RTC_DATE rtc;
	for(i = 0;i < 10;i++)
	{
		print_log("%x ",buf[i]);
	}
	print_log("\r\n");
	

	if(len < 1)
	{
		print_warning(len);
		return; 		
	}
	type = buf[0];
	sbuf[index1++] = type;
	if(type == 0x00)//set Time
	{	
		
		rtc.year = (unsigned short)BCD_to_HEC(buf[1])*100+ BCD_to_HEC(buf[2]);
		rtc.month = BCD_to_HEC(buf[3]);
		rtc.day = BCD_to_HEC(buf[4]);
		rtc.hour = BCD_to_HEC(buf[5]);
		rtc.minute = BCD_to_HEC(buf[6]);
		rtc.second = BCD_to_HEC(buf[7]);
		print_log("dateSet:%d,%d,%d,%d,%d,%d\r\n",rtc.year ,
		rtc.month,rtc.day,rtc.hour,rtc.minute,rtc.second);
	//设置时间不准调度		
	OSSchedLock();
		RTCSetTime(&rtc);
		RTCReadTime(&RTCData);
	OSSchedUnlock();
		print_log("dateSet:%d,%d,%d,%d,%d,%d\r\n",rtc.year ,
			rtc.month,rtc.day,rtc.hour,rtc.minute,rtc.second);
		sbuf[index1++] = 0x01;
	#if 0	
		sbuf[index1++] =  HEC_to_BCD(rtc.year/100) ;
		sbuf[index1++] =  HEC_to_BCD(rtc.year%100) ;
		sbuf[index1++] =  HEC_to_BCD(rtc.month);
		sbuf[index1++] =  HEC_to_BCD(rtc.day);
		sbuf[index1++] =  HEC_to_BCD(rtc.hour);
		sbuf[index1++] =  HEC_to_BCD(rtc.minute);
		sbuf[index1++] =  HEC_to_BCD(rtc.second);
	#endif	
	}
	else if(type == 0x01)//get Time
	{
		sbuf[index1++] =  HEC_to_BCD(RTCData.year/100) ;
		sbuf[index1++] =  HEC_to_BCD(RTCData.year%100) ;
		sbuf[index1++] =  HEC_to_BCD(RTCData.month);
		sbuf[index1++] =  HEC_to_BCD(RTCData.day);
		sbuf[index1++] =  HEC_to_BCD(RTCData.hour);
		sbuf[index1++] =  HEC_to_BCD(RTCData.minute);
		sbuf[index1++] =  HEC_to_BCD(RTCData.second);
	}

#endif	
	pcReply(DATETIME_RPT,index1,sbuf);
	
}


/*********************************************************************************************************
** Function name:     	pcStatusReq
** Descriptions:	    整机状态信息上报给PC机
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcStatusReq(unsigned char len,unsigned char *buf)
{
	unsigned char index1 = 0,rst[10] = {0},type,temp,temp1 = 0,err;

#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE

	len = len;
	type = buf[0];
	rst[index1++] = type;


	if(type == 0)//获取整机状态
	{
		//售货机状态
		if(SYSPara.initOK)
		{	
			rst[index1++] = (HardWareErr) ? 2 : 0; //0 正常1 全货道无货 2故障 3维护 
		}	
		else
			rst[index1++] = 0x04; //正在初始化中
		//纸币器类型
		rst[index1++] = SYSPara.BillType;
		//纸币器状态
		err = HardWareErr & SYS_ERR_NO_BILL;
		if(err == 0)
		{
			temp = 0;
		}
		else 
		{				
			temp = (0 &0x01) |
				((0 & 0x01)<< 1) |
				((0 & 0x01)<< 2) |
				((0 & 0x01)<< 3)|
				((0 & 0x01)<< 4) |
				((0 & 0x01)<< 5) |
				((1 & 0x01)<< 6) |
				(0 << 7); 
		}
		rst[index1++] = 0;//备用
		rst[index1++] = temp;

		//硬币器类型

		rst[index1++] = (SYSPara.CoinType << 4) | 
						(SYSPara.DispenceType & 0x0F);
		//硬币器状态
		err = HardWareErr & SYS_ERR_NO_COIN;;

		rst[index1] = 0;
		rst[index1+ 1] = 0;
		rst[index1+ 2] = 0;
		rst[index1+ 3] = 0;
		if(err)
		{
			rst[index1+ 3] = (0 & 0x01) |
				((0 & 0x01)<< 1) |
				((0 & 0x01)<< 2) |
				((0 & 0x01)<< 3) |
				((0 & 0x01)<< 4) |
				((0 & 0x01)<< 5) |
				((1 & 0x01)<< 6) 
				;

			//第二字节
			rst[index1+ 2] = (0 & 0x01);
								
			if(SYSPara.DispenceType == VMC_CHANGER_HOPPER)
			{	
				rst[index1+ 1] |= ((((hopperErrNo & HP_ERR_NO_HOPPER1_EMPTY)? 1:0) & 0x01)<< 0);
				rst[index1+ 1] |= ((((hopperErrNo & HP_ERR_NO_HOPPER1_FAULT)? 1:0) & 0x01)<< 1)	;
				rst[index1+ 1] |= ((((hopperErrNo & HP_ERR_NO_HOPPER2_EMPTY)? 1:0) & 0x01)<< 2)	;
				rst[index1+ 1] |= ((((hopperErrNo & HP_ERR_NO_HOPPER2_FAULT)? 1:0) & 0x01)<< 3)	;
				rst[index1+ 1] |= ((((hopperErrNo & HP_ERR_NO_HOPPER3_EMPTY)? 1:0) & 0x01)<< 4);
				rst[index1+ 1] |= ((((hopperErrNo & HP_ERR_NO_HOPPER3_FAULT)? 1:0) & 0x01)<< 5);
				rst[index1+ 1] |= ((0 & 0x01)<< 6);
				rst[index1+ 1] |= ((0 & 0x01)<< 7);
		
			}
		}
		index1 += 4;//偏移
			

		//主柜室运行状态
		rst[index1++] = 0;//备用
		rst[index1++] = 0;

		//主柜室故障状态
			
		temp = 0;
		temp = ((0 & 0x01) << 2) |
			((((HardWareErr & SYS_ERR_NO_HUODAO_FAULT)? 1:0) & 0x01) << 1) |
			((((HardWareErr & SYS_ERR_NO_HUODAO_EMPTY)? 1:0) & 0x01) << 0)
		;
	
	
		
		rst[index1++] = temp1;//备用
		rst[index1++] = temp;
	}

	
	OSTimeDly(50/5);
	pcReply(STATUS_REQ,index1,rst);
#else
   	unsigned char type,index1 = 0,rst[10] = {0},temp,temp1 = 0,err;
	len = len;
	type = buf[0];
	rst[index1++] = type;
	if(type == 0)//获取整机状态
	{
		//售货机状态
		if(DeviceStateBusiness.ErrorInit)
			rst[index1++] = ErrorStatus(4);//11 1GOC  2 bill 3 coin 4 vmc
		else
			rst[index1++] = 0x04; //正在初始化中
		//纸币器类型
		rst[index1++] = SystemPara.BillValidatorType;
		//纸币器状态
		err = ErrorStatus(2);
		if(err == 0)
		{
			temp = 0;
		}
		else if(err == 1)
		{
			temp = 0x80;
		}
		else 
		{				
			temp = (DeviceStateBusiness.BillCommunicate &0x01) |
				((DeviceStateBusiness.Billmoto & 0x01)<< 1) |
				((DeviceStateBusiness.Billsensor & 0x01)<< 2) |
				((DeviceStateBusiness.Billromchk & 0x01)<< 3)|
				((DeviceStateBusiness.Billjam & 0x01)<< 4) |
				((DeviceStateBusiness.BillremoveCash & 0x01)<< 5) |
				((DeviceStateBusiness.Billdisable & 0x01)<< 6) |
				(0 << 7); 
		}
		rst[index1++] = 0;//备用
		rst[index1++] = temp;

		//硬币器类型

		rst[index1++] = (SystemPara.CoinAcceptorType << 4) | 
						(SystemPara.CoinChangerType & 0x0F);
		//硬币器状态
		err = ErrorStatus(3);

		rst[index1] = 0;
		rst[index1+ 1] = 0;
		rst[index1+ 2] = 0;
		rst[index1+ 3] = 0;
		if(err)
		{
			rst[index1+ 3] = (DeviceStateBusiness.CoinCommunicate & 0x01) |
				((DeviceStateBusiness.Coinsensor & 0x01)<< 1) |
				((DeviceStateBusiness.Cointubejam & 0x01)<< 2) |
				((DeviceStateBusiness.Coinromchk & 0x01)<< 3) |
				((DeviceStateBusiness.Coinrouting & 0x01)<< 4) |
				((DeviceStateBusiness.CoinremoveTube & 0x01)<< 5) |
				((DeviceStateBusiness.Coindisable & 0x01)<< 6) 
				;
			rst[index1+ 3] |= (err == 1) ? 0x80 : 0;

			//第二字节
			rst[index1+ 2] = (DeviceStateBusiness.CoinunknowError & 0x01)
								;
								
			if(SystemPara.CoinChangerType == 1)
			{
				rst[index1+ 1] = ((DeviceStateBusiness.Hopper1State == 1) & 0x01) |
				(((DeviceStateBusiness.Hopper1State == 2) & 0x01)<< 1) |
				(((DeviceStateBusiness.Hopper2State == 1) & 0x01)<< 2) |
				(((DeviceStateBusiness.Hopper2State == 2) & 0x01)<< 3) |
				(((DeviceStateBusiness.Hopper3State == 1) & 0x01)<< 4) |
				(((DeviceStateBusiness.Hopper3State == 2) & 0x01)<< 5) |
				((0)<< 6) |
				(0 << 7) 
				;
			}
		}
		index1 += 4;//偏移
			

		//主柜室运行状态
		rst[index1++] = 0;//备用
		rst[index1++] = 0;

		//主柜室故障状态
		if(SystemPara.Channel == 2)
		{
			temp = ((DeviceStateBusiness.Emp_Gol & 0x01) << 0) |
				((DeviceStateBusiness.ColBoardError & 0x01) << 1) |
				((DeviceStateBusiness.GOCError & 0x01) << 2) |
				((stLiftTable.Error_LIFTER & 0x01) << 3) |
				((stLiftTable.Error_TAKEMOTTOR & 0x01) << 4) |
				((stLiftTable.Error_FANGDAO & 0x01) << 5) |
				(0 << 6)|
				(0 << 7)
				;
		}
		else
		{
			temp = 0;
			temp = ((DeviceStateBusiness.GOCError & 0x01) << 2) |
				((DeviceStateBusiness.ColBoardError & 0x01) << 1) |
				((DeviceStateBusiness.Emp_Gol & 0x01) << 0)
			;
		}
	
		
		rst[index1++] = temp1;//备用
		rst[index1++] = temp;
		
		pcReply(STATUS_REQ,index1,rst);
	}

#endif	
	
	
}

/*********************************************************************************************************
** Function name:     	pcFaultReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcDebugReq(unsigned char len,unsigned char *data)
{
	unsigned char type,sendBuf[10] = {0},index = 0,index1 = 0; 
	type = data[index++];
	sendBuf[index1++] = type;
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
#else
	if(type == 0)//获取
	{
		sendBuf[index1++] = mainTrace;
		sendBuf[index1++] = UserPara.billTrace;
		sendBuf[index1++] = UserPara.coinTrace;
		sendBuf[index1++] = UserPara.changelTrace;
		sendBuf[index1++] = UserPara.readerTrace;
		sendBuf[index1++] = UserPara.channelTrace;
		sendBuf[index1++] = UserPara.compressorTrace;
		sendBuf[index1++] = UserPara.ledTrace;
		sendBuf[index1++] = UserPara.selectTrace;
		sendBuf[index1++] = UserPara.PC;
		
	}
	else if(type == 1)//设置总Trace
	{		
		
		mainTrace = data[index++];
		//UserPara.TraceFlag = data[index++];
		sendBuf[index1++] = mainTrace;
		sendBuf[index1++] = UserPara.billTrace;
		sendBuf[index1++] = UserPara.coinTrace;
		sendBuf[index1++] = UserPara.changelTrace;
		sendBuf[index1++] = UserPara.readerTrace;
		sendBuf[index1++] = UserPara.channelTrace;
		sendBuf[index1++] = UserPara.compressorTrace;
		sendBuf[index1++] = UserPara.ledTrace;
		sendBuf[index1++] = UserPara.selectTrace;
		sendBuf[index1++] = UserPara.PC;
		
		
					
		//sendBuf[index1++] = 0x01;
		
	}
	else if(type == 2)//设置纸币器
	{
		UserPara.billTrace = data[index++];
		sendBuf[index1++] = UserPara.billTrace;
		
	}
	else if(type == 3)
	{
		UserPara.coinTrace = data[index++];
		sendBuf[index1++] = UserPara.coinTrace;
	}
	else if(type == 4)
	{
		UserPara.changelTrace = data[index++];
		sendBuf[index1++] = UserPara.changelTrace;
		
	}
	else if(type == 5)
	{
		UserPara.readerTrace = data[index++];
		sendBuf[index1++] = UserPara.readerTrace;
	}
	else if(type == 6)
	{
		UserPara.channelTrace = data[index++];
		sendBuf[index1++] = UserPara.channelTrace;
	}
	else if(type == 7)
	{
		UserPara.compressorTrace = data[index++];
		sendBuf[index1++] = UserPara.compressorTrace;
	}
	else if(type == 8)
	{
		UserPara.ledTrace = data[index++];
		sendBuf[index1++] = UserPara.ledTrace;
	}
	else if(type == 9)
	{
		UserPara.selectTrace = data[index++];
		sendBuf[index1++] = UserPara.selectTrace;
	}
	else if(type == 10)
	{
		UserPara.PC = data[index++];
		sendBuf[index1++] = UserPara.PC;
	}
	else if(type == 0xF0)
	{
		UserPara.TraceFlag = mainTrace;
		WriteUserSystemPara(UserPara);
		UserPara.TraceFlag = 0;
		sendBuf[index1++] = 0x01;
	}
	else
	{
		print_warning(type);
		return;
	}
#endif	
	OSTimeDly(100/5);	
	pcReply(DEBUG_RPT,index1,sendBuf) ;
	
}


/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static unsigned char vmcPoll(unsigned char busy)
{

	unsigned char rlen,type;
	if(pcControlSend(VMC_PCM_POLL,1,&busy) != msg)
		return 1;

	if(busy)
	{
		print_warning(busy);
		return 1;
	}		
	type = pcUartMsg->recv_msg[3];//MT
	rlen = pcUartMsg->recv_len - head;
	switch(type)
	{
		case TRADE_REQ:
			pcTradeReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case CLEAR_REQ:
			pcClearReq(rlen,pcUartMsg->recv_msg[head]);
			break;
		case SYSPARA_REQ:
			pcSysParaReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case HUODAO_REQ:
			pcHuodaoReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case FACTORRECOVER_REQ:
			pcFactorReq();
			break;
		case DATETIME_REQ:
			pcDateTimeReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case STATUS_REQ:
			pcStatusReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case DEBUG_REQ:
			pcDebugReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		default:
			break;

	}
	return 1;
}


/*********************************************************************************************************
** Function name:     	pcControlInit
** Descriptions:	    串口0任务初始化
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char pcControlInit()
{
	vmc_pcm_event = OSQCreate(&PCMSizeQ[0],PCM_MAX_SIZE);
	pcm_vmc_event = OSMboxCreate(NULL);

	//串口创建消息队列
	//uart0_event =  OSQCreate(&uart0SizeQ[0],UART0_SIZE);
	OSTaskCreate(enterPCControl,(void *)0,&pcControlTaskStk[512 - 1],12);
	print_log("Create taskPCManagement...\r\n");
	OSTimeDly(4);
	return 1;
	
}

/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void uart0Post(unsigned char *buf)
{
	
//	OSQPost(uart0_event,buf);
//	uart0_index = (++uart0_index) % UART0_SIZE;
	OSTimeDly(2);
}




/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void enterPCControl(void *pd)
{

	unsigned char errMBox;	
	PCCONMSG *recvMsg;
	pd = pd;
	pcUartMsg = &pcm_msg;//为了节省点内存又不影响可读性和移植性 	
	
	comErr = 12;

	mainTrace = SYSPara.TraceFlag;

	//自作假数据
	//trade_log_write();
	while(1)
	{
		if(comErr > 10)
		{
			SYSPara.TraceFlag = mainTrace;//idsconnected PC Enable the uart0
			while(1)
			{						
				if(pcSeek())
				{
					comErr = 0;
					busyFlag = 0;

					mainTrace = SYSPara.TraceFlag;
#if PCMPORT != 3
				    SYSPara.TraceFlag = 0;//connected PC disable the uart0	
#endif	
				
					
					break;
				}
				else
					OSTimeDly(1000/5);	
			}			
		}	
		recvMsg = OSQPend(vmc_pcm_event,500/5,&errMBox);
		if(errMBox == OS_NO_ERR)
		{
			switch(recvMsg->type)
			{
				//回应结果
				case MBOX_HUODAO_INIT:
					pcInitHuoDaoRpt(0x02,1);
					break;
				case MBOX_HUODAO_TEST:
					pcTestAisleRpt(recvMsg);
					break;
				default:
					
					break;
			}
			
			OSTimeDly(500/5);
		}
		vmcPoll(busyFlag);

		//如果在处理请求 轮询 处理结果
		if(busyFlag)
		{
			if(!Timer.pcm_handle_timeout)
			   	busyFlag = 0;
			else
			{
				switch(handleType)
				{
					case HUODAO_REQ://处理
						pcTestAisleRpt(&pc_vmc_mail);
						break;
					
					default:
						handleType = 0xFF;
						busyFlag = 0;
						break;
				}
			}						
		}
		
	}

}


/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/


void vmcPost(unsigned char type)
{
	vmc_pc_mail[pcm_index].type = type;
	OSQPost(vmc_pcm_event,&vmc_pc_mail[pcm_index]);
	pcm_index = (++pcm_index) % PCM_MAX_SIZE;
	OSTimeDly(4);
}

/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

void pcPost(unsigned char type)
{
	pc_vmc_mail.type = type;
	OSMboxPost(pcm_vmc_event,&pc_vmc_mail);
	OSTimeDly(4);
}


/*********************************************************************************************************
** Function name:     	mainTaskPollPC
** Descriptions:	    主任务轮询PC长时间的指令
** input parameters:    state 1运行  2 故障  3维护
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void mainTaskPollPC(unsigned char state)
{
	INT8U err;
	PCCONMSG *recvMsg;

	state = state;
	recvMsg = OSMboxPend(pcm_vmc_event,2,&err);
	if(err == OS_NO_ERR)
	{
		switch(recvMsg->type)
		{
			case MBOX_HUODAO_INIT:
#if VMC_MACHINE_TYPE == VMC_SMALL_TYPE
				columnInit(0);
#else				
				ChannelResetinit();	
#endif
				vmcPost(MBOX_HUODAO_INIT);
				break;
			case MBOX_HUODAO_TEST:
				print_log("recvMsg:%d %d %d",recvMsg->msg[0],
					recvMsg->msg[1],recvMsg->msg[2]);
				pcTesttingAisle(recvMsg);				
				break;
			default:
				
				break;
		}
		
	}
	
}


#endif


/**************************************************************************************************
*
*新协议目前止步于货道请求部分
************************************************************************************************/

