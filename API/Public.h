#ifndef _PUBLIC_H
#define _PUBLIC_H

#include "../API/BillDevControl.h"
#include "../API/ChannelHandle.h"
#include "../API/GoodsAndKey.h"
#include "../API/MemoryHandle.h"
#include "../API/ZikuList.h"
#include "../config.h"
#include "../API/UI.h"


//定义主任务与设备任务之间通信的结构体
typedef struct
{
    uint8_t   SendType;				//命令类型
	uint8_t   SubSendType;		   //子集命令类型

	unsigned char  value_8;
	unsigned short value_16;
	unsigned int   value_32;
	
} MAIN_DEV_TASK_MSG;	




/*---------------------------------------------------------
任务间通信设备任务接受的操作指令集合
-----------------------------------------------------------*/


#define TASK_DISABLE_REQ           1 //主任务发送禁能 纸硬币器请求
#define TASK_DISABLE_RPT		   2

#define TASK_ENABLE_REQ            3//主任务发送使能 纸硬币器
#define TASK_ENABLE_RPT            4

#define TASK_TRADE_REQ			5 //主任务发送出货命令请求
#define TASK_TRADE_RPT			6 //

#define TASK_HOPPER_REQ        7
#define TASK_HOPPER_RPT        7


#if 0
//获取按键
#define TASKCMD_GETKEY					(0x10)
//Hopper操作命令
#define TASKCMD_HOPPER					(0x11)
//纸币器收到纸币返回通道及面值
#define TASKCMD_BILLIN					(0x12)
//禁能纸币器命令
#define TASKCMD_DISABLEDEV				(0x13)
//使能纸币器命令
#define TASKCMD_ENABLEDEV				(0x14)
//维护状态禁能所有交易设备命令
#define TASKCMD_VINDICATE				(0x15)
//出货操作命令
#define TASKCMD_CHANNELHANDLE			(0x16)
//禁能硬币器
#define TASKCMD_DISABLECOIN				(0x17)
//使能硬币器
#define TASKCMD_ENABLECOIN				(0x18)
#endif

//任务通信邮箱的对发邮箱句柄
extern OS_EVENT *g_msg_main_to_dev;
extern OS_EVENT *g_msg_dev_to_main;
extern MAIN_DEV_TASK_MSG task_msg_main_to_dev;
extern MAIN_DEV_TASK_MSG task_msg_dev_to_main;


//纸币器邮箱
extern OS_EVENT *g_MoneyIN;
typedef struct _money_recv_msg_{
	unsigned char billType;
	unsigned int billValue;
	
}MONEY_RECV_MSG;
extern MONEY_RECV_MSG money_recv_msg;  


//初始化任务信号量
extern OS_EVENT *g_InitSem;
//硬币器通信消息队列
extern OS_EVENT *g_CoinIn;
#define G_COIN_IN_SIZE    50


//交易设备的通道面值数据结构
extern struct STDevValue stDevValue;

/*********************************************************************************************
**函   数   名:
**函数功能说明：
**函数传入参数：
**函数传出参数：
**函 数 返回值：
**********************************************************************************************/
/*-------------------------------------------------------------------------------------------
                                    底层设备控制接口API
---------------------------------------------------------------------------------------------*/

/*********************************************************************************************
**函   数   名: CreateCommonMBox
**函数功能说明：创建任务通信邮箱
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
void CreateCommonMBox(void);

/*********************************************************************************************
**函   数   名: TASK_Device
**函数功能说明：设备管理任务
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
void TASK_Device(void *pdata);

/*********************************************************************************************************
** Function name:       GetKeyInValue
** Descriptions:        获得输入的按键键值
** input parameters:    无
** output parameters:   无
** Returned value:      有按键返回相应的键值，无按键返回0xff
*********************************************************************************************************/
uint8_t GetKeyInValue(void);

/*********************************************************************************************************
** Function name:       SystemInit
** Descriptions:        系统初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SystemInit(void);

unsigned char mbox_post_main_to_dev(unsigned char type);
unsigned char mbox_post_dev_to_main(unsigned char type);
unsigned char checkAllDevice(void);

#endif
