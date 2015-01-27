#include "Public.h"
#include "..\App\Menu.h"
#include "..\App\SailControl.h"

#define DISABLEBILLDEV					(1)				//禁能纸币器设备
#define ENABLEBILLDEV					(0)				//使能纸币器设备

//定时重新使能纸币器
uint16_t ReEnableTime = 0;
//是否禁能设备标志
uint8_t DisableDevFlag = 0;


//任务通信邮箱的对发邮箱句柄
OS_EVENT *g_msg_main_to_dev;
OS_EVENT *g_msg_dev_to_main;
MAIN_DEV_TASK_MSG task_msg_main_to_dev;
MAIN_DEV_TASK_MSG task_msg_dev_to_main;



//纸币器通信邮箱
OS_EVENT *g_MoneyIN;
MONEY_RECV_MSG money_recv_msg; 


//初始化任务信号量
OS_EVENT *g_InitSem;
//硬币器通信消息队列
OS_EVENT *g_CoinIn;

volatile unsigned char coin_recv_flag =0;//收硬币标志1表示正在收硬币


void *coinInNum[G_COIN_IN_SIZE];
void *billInNum[10];

unsigned char CoinIn[G_COIN_IN_SIZE] = {0};


extern void    TASK_Device(void *pdata);   



void CreateCommonMBox()
{
	//创建主任务到设备任务的邮箱
	g_msg_main_to_dev = OSMboxCreate(NULL);
	//创建设备任务返回主任务的邮箱
	g_msg_dev_to_main = OSMboxCreate(NULL);
	
	g_MoneyIN = OSQCreate(&billInNum[0],10);
	g_CoinIn = OSQCreate(&coinInNum[0],G_COIN_IN_SIZE);
	//创建初始化控制信号量
	g_InitSem = OSSemCreate(0);
}

/*********************************************************************************************************
** Function name:     	initDeviecByfile
** Descriptions:	   根据配置文件初始化设备
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void initDeviecByfile(void)
{	

	SYSPara.LCDSel = VMC_DISP_SELECT;//选择lcd液晶屏显示
	SYSPara.BillType = VMC_BILL_TYPE;
	SYSPara.CoinType= VMC_COIN_TYPE;
	SYSPara.DispenceType = VMC_CHANGER_TYPE;
	stDevValue.hpBaseChange = HOPPER_BASE_CHANGED;
	SYSPara.TraceFlag = VMC_TRACE_FLAG;
	SYSPara.nPoinValue = VMC_PIONT_TYPE;
	
#if VMC_OPEN_COINFG == 1
	
	
	stDevValue.CoinValue[0] = VMC_COIN_CHANNEL_1;
	stDevValue.CoinValue[1] = VMC_COIN_CHANNEL_2;
	stDevValue.CoinValue[2] = VMC_COIN_CHANNEL_3;
	stDevValue.CoinValue[3] = VMC_COIN_CHANNEL_4;
	stDevValue.CoinValue[4] = VMC_COIN_CHANNEL_5;
	stDevValue.CoinValue[5] = VMC_COIN_CHANNEL_6;
	stDevValue.CoinValue[6] = VMC_COIN_CHANNEL_7;
	stDevValue.CoinValue[7] = VMC_COIN_CHANNEL_8;

	stDevValue.HpValue[0] = VMC_CHANGER_CHANNEL_1;
	stDevValue.HpValue[1] = VMC_CHANGER_CHANNEL_2;
	stDevValue.HpValue[2] = VMC_CHANGER_CHANNEL_3;
#endif


}




/*********************************************************************************************************
** Function name:       SystemParaInit
** Descriptions:        系统参数初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SystemParaInit()
{
	//统一从flash中读取所有有关售货机的参数
	readSystemParaFromFlash();//changed by yoc 2014.2.19
	
	//从flash中读取交易有关的数据
	readTradeInfo();
	initDeviecByfile();

	
}




/*********************************************************************************************************
** Function name:       SystemDevInit
** Descriptions:        系统设备初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SystemDevInit()
{		
	//硬币器初始化	
	if(SYSPara.CoinType == VMC_COIN_PARALLEL)
	{
		disp_init_page(1);
		InitParallelPluseCoinAcceptor();
	}	
	else if(SYSPara.CoinType == VMC_COIN_SERIAL)
	{
		disp_init_page(1);
		InitSerialPluseCoinAcdeptor();
	}	
	msleep(1000);
	
	//纸币器初始化
	if(SYSPara.BillType)
	{
		disp_init_page(2);
		BillDevInit();
	}
	
	
	//找零器初始化
	if(SYSPara.DispenceType)
	{
		disp_init_page(3);
		hopperInit();
	}
	msleep(1000);
		
#if VMC_TYPE == VMC_TYPE_TRADE
	disp_init_page(4);
	msleep(2000);//等待货道驱动板稳定	
	hd_init();//货道初始化
	Trace("Channel init over.....\r\n");
#endif

	//使能键盘
	EnableKeyBoard();

	//使能串行、并行硬币器
	if(SYSPara.CoinType == 2)
		DisableParallelPluseCoinAcceptor();
	else if(SYSPara.CoinType == 1)
		DisableSerialPluseCoinAcceptor();
	Trace("BillType = %d,CoinType = %d,changerType = %d,language = %d, lcdSel = %d,nPointValue = %d \r\n",
		SYSPara.BillType,SYSPara.CoinType,SYSPara.DispenceType,
		SYSPara.Language,SYSPara.LCDSel,SYSPara.nPoinValue);
	
}


/*********************************************************************************************************
** Function name:       SystemInit
** Descriptions:        系统初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SystemInit()
{
	//注意复位后，Uart0使能，Uart2/3被禁能
	InitUart0();
	InitUart1();
	InitUart2();
	InitUart3();
    InitBuzzer();
	InitI2C0();
	InitTimer(0,240000);
	InitRtc();
	RTCStartOrStop(1);
	//维护按键初始化
	InitKeyboard();
	//初始化LCD显示
#if (VMC_DISP_SELECT == VMC_DISP_LCD)
	LCDInit();	
	DispVersionText();//显示版本号
#else
	InitDisplay();
	led_show(VMC_VERSION);//显示版本号
	msleep(1500);		
#endif
	//选货按键初始化
	SelectKeyInit();
	Trace("SelectKey init over.....\r\n");
	
}



/*********************************************************************************************************
** Function name:       allDeviceIsIdle
** Descriptions:        所有设备是否空闲
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char allDeviceIsIdle(void)
{
	if(GoToMenuFlag)
		return 0;
	if(SYSPara.initOK == 0)
		return 0;
	
	//if(coin_recv_flag)
	//	return 0;

	return 1;
	
}

void TASK_Device(void *pdata)
{
	uint8_t err,res=0;
	uint32_t Bvalue = 0;
	uint8_t Btype = 0;
	MAIN_DEV_TASK_MSG *nMsgPack;
	SYSPara.initOK = 0;

	
	//系统参数初始化
	SystemParaInit();
	//系统设备初始化
	SystemDevInit();	
	OSSemPost(g_InitSem);
	
	Trace("device init over.....\r\n");
	OSTimeDly(100);
	SYSPara.initOK = 1;


	//设备任务流程
	while(1)
	{
		//轮训纸币器
		if(SYSPara.BillType && (DisableDevFlag == ENABLEBILLDEV))//纸币器控制函数
		{	
			BillDevProcess(&Bvalue,&Btype);			
		}
		//接收设备邮件
		nMsgPack = OSMboxPend(g_msg_main_to_dev,20,&err);
		if(err == OS_NO_ERR)
		{
			//Hopper操作指令
			if(nMsgPack->SendType == TASK_HOPPER_REQ)
			{
#if VMC_TYPE == VMC_TYPE_CHANGER
				if(nMsgPack->SubSendType == 1)
					vmcBillChanger(nMsgPack->value_32, &task_msg_dev_to_main.value_32);
				else
					vmcCoinChanger(nMsgPack->value_32, &task_msg_dev_to_main.value_32);
#else
				hopperChangerHandle(nMsgPack->value_32,&task_msg_dev_to_main.value_32);
#endif			
				//将出币结果发送给交易任务
				task_msg_dev_to_main.SubSendType = nMsgPack->SubSendType;
				mbox_post_dev_to_main(TASK_HOPPER_RPT);
				
			}
			else if(nMsgPack->SendType == TASK_ENABLE_REQ)//接收到使能纸币器的邮件
			{
				if(SYSPara.BillType)
				{
					BillDevEnable();
					DisableDevFlag=ENABLEBILLDEV;
				}	
				if(SYSPara.CoinType == VMC_COIN_SERIAL)
				{
					EnableSerialPluseCoinAcceptor();
				}
				task_msg_dev_to_main.value_8 = 1;
				//mbox_post_dev_to_main(TASK_ENABLE_RPT);
				print_log("dev Enabled \r\n");
				
			}
			else if(nMsgPack->SendType == TASK_DISABLE_REQ)//接收到禁能纸币器的邮件
			{		
				if(SYSPara.BillType)
				{
					BillDevDisable();
					DisableDevFlag= DISABLEBILLDEV;
				}	
				if(SYSPara.CoinType == VMC_COIN_SERIAL)
				{
					DisableSerialPluseCoinAcceptor();
				}
				task_msg_dev_to_main.value_8 = 1;
				//mbox_post_dev_to_main(TASK_DISABLE_RPT);
				print_log("dev Disabled \r\n");
			}
			else if(nMsgPack->SendType == TASK_TRADE_REQ)//出货操作
			{
				//根据不同命令做不同的操作
				
				//res = ChannelHandle(nMsgPack->SubSendType,nMsgPack->value_8);
				res = hd_outgoods(nMsgPack->value_8);
				//将出货结果发送给交易任务
				task_msg_dev_to_main.value_8 = res;
				task_msg_dev_to_main.SubSendType = nMsgPack->SubSendType;
				mbox_post_dev_to_main(TASK_TRADE_RPT);
			}
			
			OSTimeDly(20);
			
		}
		else
		{
			if(!Timer.checkDeviceTimeout && allDeviceIsIdle())//查询设备状态
			{
				print_log("checkeStatus.....\r\n");
					//检查hopper
				if(SYSPara.DispenceType)
					checkHopperAndUpdate();
				checkDeviceState(); 
				
				Timer.checkDeviceTimeout = 500;//每3秒查询设备状态
				
			}
		}
			
	}	
}


/*********************************************************************************************************
** Function name:       GetKeyInValue
** Descriptions:        获得输入的按键键值
** input parameters:    无
** output parameters:   无
** Returned value:      有按键返回相应的键值，无按键返回0xff
*********************************************************************************************************/
uint8_t GetKeyInValue()
{
	uint8_t mkey;
	
	mkey = ReadKeyValue();
	if(mkey != 0x00)
	{
		switch(mkey)
		{
			case 0x31:
				return 0x01;
			case 0x32:
				return 0x02;
			case 0x33:
				return 0x03;
			case 0x34:
				return 0x04;
			case 0x35:
				return 0x05;
			case 0x36:
				return 0x06;
			case 0x37:
				return 0x07;
			case 0x38:
				return 0x08;
			case 0x39:
				return 0x09;
			case 0x30:
				return 0x00;
			case 0x44:
				return 0x0c;
			case 0x2e:
				return 0x0d;
			case 0x3c:
				return 0x0a;
			case 0x3e:
				return 0x0b;
			case 0x43:
				return 0x0e;
			case 0x45:
				return 0x0f;
			default:
				return 0xff;
		}
	}
	return 0xff;	
}






/*********************************************************************************************************
** Function name:       mbox_post_main_to_dev
** Descriptions:        主任务发送邮箱到设备任务
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char mbox_post_main_to_dev(unsigned char type)
{
	unsigned char rst;
	task_msg_main_to_dev.SendType = type;
	rst = OSMboxPost(g_msg_main_to_dev,&task_msg_main_to_dev);
	OSTimeDly(5);
	return (rst == OS_NO_ERR) ? 1 : 0;
}




/*********************************************************************************************************
** Function name:       mbox_post_dev_to_main
** Descriptions:        设备任务到主任务的邮箱发送函数
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char mbox_post_dev_to_main(unsigned char type)
{
	unsigned char rst;
	task_msg_dev_to_main.SendType = type;
	rst = OSMboxPost(g_msg_dev_to_main,&task_msg_dev_to_main);
	OSTimeDly(5);
	return (rst == OS_NO_ERR) ? 1 : 0;
}


/*********************************************************************************************************
** Function name:       checkAllDevice
** Descriptions:        检查所有设备
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char checkAllDevice(void)
{
	//检查纸币器
	GetBillDevState();
	checkHopperAndUpdate();//检查hopper
	checkDeviceState();//故障码刷新

	return 1;
}



