/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        主程序文件                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"
#include "SailControl.h"
#include "../API/Public.h"
#include "Global.h"
#include "Menu.h"
#include "LANGUAGE.h"
#include "MAINTAIN.h"



/*********************************************************************************************************
  定义全局变量
*********************************************************************************************************/
#if 1
#define TRADE_BEGIN						(0x01)				//
#define TRADE_DISP						(0x02)				//显示信息
#define TRADE_HPOUT						(0x03)				//找零
#define TRADE_SAVETRADE					(0x04)				//保存交易记录
#define TRADE_BILLIN					(0x05)				//检测是否有纸币或硬币进入
#define TRADE_ISCANOUTGOODS				(0x06)				//检测是否可以出货
#define TRADE_TRADEOVER					(0x07)				//一次交易流程完成
#define TRADE_WEIHU						(0x08)				//进入维护模式
#define TRADE_CHECKCHANNEL				(0x12)				//查询货道
#define TRADE_OUTGOODS					(0x13)				//出货
#define TRADE_SELECTGOODS				(0x14)				//是否有选货按键
#define TRADE_CHECKDEVICEERR			(0x15)				//检测设备是否故障
#define TRADE_STOPSERVICE				(0x16)				//停止服务
#endif


//MAIN
#define MAIN_START						(1)//交易开始
#define MAIN_FREE						(2)//交易空闲
#define MAIN_CHECK_GOODS				(4)//出货前检查
#define MAIN_OUTGOODS					(5)//出货
#define MAIN_OVER						(6)//交易结束
#define MAIN_WEIHU						(7)//维护模式
#define MAIN_FAULT						(0x16)//故障模式
#define MAIN_PAYOUT						(9)//找零



uint8_t GoToMenuFlag = 0;///维护模式标志


volatile unsigned int TestPluse;
volatile uint16_t WaitMoneyInTimer;
volatile uint32_t WaitMoneyInTimer_1;

uint32_t nMinDispMoney = 0;
unsigned int MaxColumnMoney=0;
//按键邮箱句柄
OS_EVENT *g_KeyMsg;






void *KeyMsgArray[2]; // 按键队列数组

extern uint16_t HardWareErr;
volatile uint16_t WaitCmdTimer;


/*********************************************************************************************************
  交易主 任务ID、优先级、堆栈设置及函数声明                                
*********************************************************************************************************/
#define TASK_Trade_ID                          7                             /*   交易主任务ID                       */
#define TASK_Trade_PRIO                        TASK_Trade_ID                      /* 任务优先级                   */
#define TASK_Trade_STACK_SIZE                  1024                           /* 定义用户堆栈长度             */
OS_STK  TASK_Trade_STACK[TASK_Trade_STACK_SIZE];                                  /* 定义任务B堆栈                */
extern void    TASK_Trade(void *pdata);                                      /* 声明任务B                    */

/*********************************************************************************************************
  按键 任务ID、优先级、堆栈设置及函数声明                                
*********************************************************************************************************/
#define TASK_DispState_ID                          10                             /* 按键任务ID                       */
#define TASK_DispState_PRIO                        TASK_DispState_ID                      /* 任务优先级                   */
#define TASK_DispState_STACK_SIZE                  128                           /* 定义用户堆栈长度             */
OS_STK  TASK_DispState_STACK[TASK_DispState_STACK_SIZE];                                  /* 定义任务B堆栈                */
extern void    TASK_DispState(void *pdata);   



/*********************************************************************************************************
  设备 任务ID、优先级、堆栈设置及函数声明                                
*********************************************************************************************************/   
#define TASK_Device_ID                          5                             /* 任务ID                       */
#define TASK_Device_PRIO                        TASK_Device_ID                      /* 任务优先级                   */
#define TASK_Device_STACK_SIZE                  512                           /* 定义用户堆栈长度             */
OS_STK  TASK_Device_STACK[TASK_Device_STACK_SIZE];                                  /* 定义任务C 堆栈               */



/*********************************************************************************************************
** Function name:       CreateMBox
** Descriptions:        为任务之间通信创建邮箱和信号量
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CreateMBox(void)
{
	//创建按键邮箱
	g_KeyMsg = OSQCreate(&KeyMsgArray[0],2);
	CreateCommonMBox();
}



static unsigned char GotoWeiHuMenu(const unsigned char state)
{

	GoToMenuFlag = GetInMenuType();
	
	if(GoToMenuFlag == 1)
	{
		Trace("GotoWeiHuMenu...\r\n",GoToMenuFlag);
		deviceEnable(FALSE);
		if(SYSPara.LCDSel == VMC_DISP_LCD)
			MaintainUserProcess();
		else
			UserMenu();
		
	}
	else
	if(GoToMenuFlag == 2)
	{
		deviceEnable(FALSE);
		if(SYSPara.LCDSel == VMC_DISP_LCD)
			MaintainProcess();
		else
			AdminMenu();
	}	
	if(GoToMenuFlag)
	{
		//退出维护选择显示页面
		if(state == MAIN_FAULT)
		{
			DispErrPage();
			deviceEnable(FALSE);
		}	
		else
		{
			DispFreePage(hopperIsEmpty());
			deviceEnable(TRUE);
		}	
		
		GoToMenuFlag = 0;//跳出维护 清标志
		return 1;
		
	}
	return 0;
	
	
}

/*********************************************************************************************************
** Function name:       CreateMBox
** Descriptions:        为任务之间通信创建邮箱和信号量
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void setPayKey(const unsigned char flag)
{

	static unsigned char on = 0;
	on = flag;
	
#if 0
	if(flag >= 2)
	{
		FIO0DIR |= (1ul << 30);
		FIO0CLR	|= (1ul << 30);
		on = 0;
		Trace("setPayKey:%d\r\n",flag);
	}
	else
	{
		if(on != flag)
		{
			Trace("setPayKey:%d\r\n",flag);
			FIO0DIR |= (1ul << 30);
			if(flag == 1)
				FIO0SET |= (1ul << 30);
			else
				FIO0CLR	|= (1ul << 30);

			on = flag;
		}		
	}
#endif	

}


/*********************************************************************************************************
** Function name:       trade_task_changer
** Descriptions:        兑零机交易流程
** input parameters:   
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void trade_task_changer(void)
{

#if (VMC_TYPE == VMC_TYPE_CHANGER)
	uint8_t err,billtype=0,hopperState = 0,hasBuy = 0,rcx;
	uint32_t moy=0,bValue=0,iouMoney;
	uint8_t CurrentState = 0;
	
	static unsigned int remainAmount = 0,curBillAmount = 0,curCoinAmount = 0;//当前剩余金额invalue = 0,isBuyGoods = 0,HpOutMoney=0,
	
	//接收初始化完成信号
	while(1)
	{
		OSSemPend(g_InitSem,1000,&err);
		if(err == OS_NO_ERR)
		{	
			break;
		}	
	}
	CurrentState = TRADE_BEGIN;
	disp_clear_screen();
	//主流程
	while(1)
	{
		switch(CurrentState)
		{
			//起始流程
			case TRADE_BEGIN:
				remainAmount = 0;
				curBillAmount = 0;
				curCoinAmount = 0;		
				hasBuy = 0;
				Timer.sec_usr_op_timer = 10;
				deviceEnable(TRUE);
				CurrentState = TRADE_DISP;
				break;
			case TRADE_DISP:
				//显示欢迎光临
				Trace("huanying.....\r\n");
				disp_free_page(hopperIsEmpty());
				CurrentState = TRADE_BILLIN;
				break;
			case TRADE_BILLIN:
				//查询纸币器
				if(SYSPara.BillType)
				{
					if(BillDevIsBillIn(&billtype,&bValue))
					{
						if(bValue != 0)
						{							
							curBillAmount += bValue;
							remainAmount += bValue;
							disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);	
							stTotalTrade.BillMoneySum += bValue;
							saveTradeInfo();
							Timer.sec_usr_op_timer = 60;//
							Timer.sec_changer_timer = 0;//纸币器选择10s超时
						}	
					}
				}
				
				//查询硬币器
				if(SYSPara.CoinType)
				{	
					moy = getCoinMoney();
					if(moy)
					{						
						curCoinAmount += moy;
						remainAmount += moy;
						stTotalTrade.CoinMoneySum += moy;	
						disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);
						saveTradeInfo();
						Timer.sec_usr_op_timer = 60;//
						Timer.sec_changer_timer = 4;//	
					}
				}	
				//空闲状态
				if((remainAmount == 0) || hopperChangerFailCheck(remainAmount))
				{
					//查询维护按键
					if(GotoWeiHuMenu(CurrentState))
						disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);
					if(HardWareErr)//有故障则跳转故障页面
							CurrentState = TRADE_STOPSERVICE;
						else
							CurrentState = TRADE_BILLIN;		
					if(hopperState != hopperIsEmpty())
					{
						hopperState = hopperIsEmpty();
						//刷新主页面
						disp_free_page(hopperIsEmpty());
					}	
					if(!Timer.sec_usr_op_timer && remainAmount == 0)
					{
						ledPaomaDisplay();
					}
					
				}
				else
				{
					CurrentState = TRADE_BILLIN;	
				}

				//查询找零按钮
				//超时兑零
				if(!Timer.sec_changer_timer && remainAmount )
				{
					if(curCoinAmount)//dsfg
					{
						if(vmcCheckCoinChanger(curCoinAmount))
						{
							Trace("duiling.c remainAmount=%d\r\n",remainAmount);
							CurrentState = TRADE_HPOUT; 
						}
					}
					
					if(curBillAmount)//纸币器处理
					{
						if(vmcCheckBillChanger(curBillAmount))
						{
							Trace("duiling.b remainAmount=%d\r\n",remainAmount);
							CurrentState = TRADE_HPOUT; 
						}
					}				
				}	
				break;
			
			case TRADE_HPOUT://找零
			
				if(remainAmount)
				{		
					deviceEnable(FALSE);
					disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);//刷新金额页面
					//显示下要找零的金额
					OSTimeDly(20);
					disp_payout_page();//正在找零
					if(curBillAmount)//纸币兑零
					{
						if(!hopperChanger(curBillAmount, &iouMoney,1))
							curBillAmount = iouMoney;
						else
							curBillAmount = 0;
					}
					
					if(curCoinAmount)//硬币兑零
					{
						if(!hopperChanger(curCoinAmount, &iouMoney,2))
						{
							print_log("coin payout fail iouMoney = %d\r\n",iouMoney);
							curCoinAmount = iouMoney;
						}
						else
							curCoinAmount = 0;
						
					}	
					//存储找零金额
					stTotalTrade.DispenceSum += remainAmount - curBillAmount - curCoinAmount;
					
					remainAmount = curBillAmount + curCoinAmount;
					curBillAmount = 0;
					curCoinAmount = remainAmount;
					
					print_log("hp.....remainAmount = %d\r\n",remainAmount);
					
					if(remainAmount)//找零失败
					{	
						disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);
						if(hopperChangerFailCheck(remainAmount))//hopper斗无法兑零 非故障
						{
							deviceEnable(TRUE);
							CurrentState = TRADE_BILLIN;
						}
						else //故障
						{
							print_log("hopper err.....\r\n");
							stTotalTrade.iouAmount += remainAmount;
							CurrentState = TRADE_STOPSERVICE;
						}
						saveTradeInfo();
					}
					else
					{
						disp_qu_change_page();//取零钱
						CurrentState = TRADE_SAVETRADE;
					}	
					
				}	
				else
					CurrentState = TRADE_BEGIN;
				
				
				break;
			case TRADE_SAVETRADE:
				//保存详细交易信息
				saveDetailTradeInfo();
				//保存交易信息
				saveTradeInfo();
				CurrentState = TRADE_TRADEOVER;
				break;
			case TRADE_TRADEOVER:
				CurrentState = TRADE_BEGIN;
				DispEndPage();//交易结束
				break;
			case TRADE_WEIHU:
				GotoWeiHuMenu(CurrentState);
				CurrentState = TRADE_BEGIN;
				break;
			case TRADE_STOPSERVICE:
				GotoWeiHuMenu(CurrentState);
				checkAllDevice();
				if(HardWareErr==SYS_ERR_NO_NORMAL)//故障清除
				{
					if(remainAmount)
					{
						CurrentState = TRADE_BILLIN;
						disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);
					}
					else
					{
						disp_free_page(hopperIsEmpty());	
						CurrentState = TRADE_BEGIN;
					}	
				}
				else
				{
					if(remainAmount)
					{	
						led_dispaly_off();
						for(rcx = 0;rcx < 10;rcx++)
						{
							GotoWeiHuMenu(CurrentState);
							OSTimeDly(10);
						}
						disp_err_page();
						for(rcx = 0;rcx < 20;rcx++)
						{
							GotoWeiHuMenu(CurrentState);
							OSTimeDly(10);
						}
						led_dispaly_off();
						for(rcx = 0;rcx < 10;rcx++)
						{
							GotoWeiHuMenu(CurrentState);
							OSTimeDly(10);
						}
						disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);
						for(rcx = 0;rcx < 20;rcx++)
						{
							GotoWeiHuMenu(CurrentState);
							OSTimeDly(10);
						}
					}
					else
					{
						disp_err_page();
					}
					
					deviceEnable(FALSE);
					CurrentState = TRADE_STOPSERVICE;
					
				}
				
				break;
			default:
				break;
		}
		
		OSTimeDly(10);
	}	


#endif
}


/*********************************************************************************************************
** Function name:       trade_task_souhuo
** Descriptions:          售货机交易流程
** input parameters:   
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void trade_task_souhuo(void)
{
	uint8_t res,err,billtype=0,hopperState = 0,hasBuy = 0;
	uint32_t moy=0,bValue=0,iouMoney;
	uint8_t CurrentState = 0,Cstate;
	static unsigned int remainAmount = 0,curBillAmount = 0,curCoinAmount = 0,columnPrice = 0;//当前剩余金额invalue = 0,isBuyGoods = 0,HpOutMoney=0,
	static unsigned char payoutEnable = 0,selectKey = 0,columnNo = 0,logicNo = 0,payoutKey = 0;//可以找零的标志	

	//接收初始化完成信号
	while(1)
	{
		OSSemPend(g_InitSem,20000,&err);
		if(err == OS_NO_ERR)
			break;
		OSTimeDly(5);
	}
	Trace("mainTask init over.....\r\n");
	CurrentState = MAIN_START;
	disp_clear_screen();
	
	while(1)
	{
		switch(CurrentState)
		{	
			case MAIN_START://起始流程
				remainAmount 	= 0;
				curBillAmount 	= 0;
				curCoinAmount 	= 0;
				selectKey 		= 0;
				columnNo 		= 0;
				logicNo 		= 0;
				hasBuy 			= 0;
				columnPrice 	= 0;
				deviceEnable(TRUE);
				disp_free_page(hopperIsEmpty());//显示欢迎光临	
				CurrentState = MAIN_FREE;					
				break;
			case MAIN_FREE:
				// 1.查询纸币器
				if(SYSPara.BillType)
				{
					if(BillDevIsBillIn(&billtype,&bValue))
					{
						stTotalTrade.BillMoneySum += bValue;
						stDetailTradeInfo.billIncome += bValue;
						curBillAmount += bValue;
						remainAmount += bValue;	
						payoutEnable = 0;//一旦有投钱 就必须购物后才能找零				
						
						if(curBillAmount >= getColumnsMaxPrice())	//判断纸币器是否禁能
						{
							deviceEnable(FALSE);
						}
						disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);
						saveTradeInfo();	
					}
				}
				// 2.查询硬币器
				if(SYSPara.CoinType)
				{
					moy = getCoinMoney();
					if(moy != 0)
					{
						stTotalTrade.CoinMoneySum += moy;
						stDetailTradeInfo.coinIncome += moy;
						curCoinAmount += moy;
						remainAmount += moy;	
						disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);
						saveTradeInfo();
					}			
				}

				// 3.查询货道按键
				selectKey = UserSelectKey();

				// 4.查询退币按键
				payoutKey = ReturnKey();

				// 5.根据余额 扫描按键灯
				setColumnLedON(remainAmount);

				if(remainAmount)//有余额
				{
					if(selectKey)
					{
						columnNo = selectKey;
						CurrentState = MAIN_CHECK_GOODS;
						break;
					}
					if((HardWareErr & SYS_ERR_NO_HUODAO_EMPTY) ||(HardWareErr & SYS_ERR_NO_HUODAO_FAULT))
						payoutEnable = 1;
					//有退币按钮响应 能找零 有余额 则找零
					if(payoutKey ==1 && payoutEnable)
					{
						CurrentState = MAIN_PAYOUT;					
					}
					
				}
				else//空闲
				{
					// 6.查询维护按键	
					GotoWeiHuMenu(CurrentState);
					if(selectKey)//有按键显示货道信息
					{
						columnPrice = getColumnInfoBySelectNo(1, selectKey,HUODAO_TYPE_PRICE);
						DispColumnPage(selectKey,columnPrice,SYSPara.nPoinValue);//显示货道金额
						OSTimeDly(1000/5);
						disp_free_page(hopperIsEmpty());//刷新主页面
					}
					if(HardWareErr != SYS_ERR_NO_NORMAL)//有故障
						CurrentState = MAIN_FAULT;

					if(hopperState != hopperIsEmpty())
					{
						hopperState = hopperIsEmpty();
						disp_free_page(hopperIsEmpty());//刷新主页面
					}
					
				}
				
				
				break;

			case MAIN_CHECK_GOODS:
				res = CheckColumnIsOk(columnNo,remainAmount,&columnPrice);
				if(res == 1)
				{
					DispColumnPage(columnNo,columnPrice,SYSPara.nPoinValue);//显示货道金额
					deviceEnable(FALSE);
					msleep(300);
					CurrentState = MAIN_OUTGOODS;
				}
				else if(res == 5)
				{
					DispCannotBuyPage(columnPrice);//显示金额不足
					CurrentState = MAIN_FREE;
					disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);//刷新金额页面	

				}
				else
				{	
					CurrentState = MAIN_FREE;//货道不可出货					
					disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);//刷新金额页面	
				}
				break;

			case MAIN_OUTGOODS:
				DispChuhuoPage();//正在出货
				logicNo = fromSelectNoGetLogicNo(1,columnNo);//转换为逻辑货道
				res = hd_outgoods_send(logicNo,&Cstate);
				if(res==1)
				{
					remainAmount -= columnPrice;
					if(curCoinAmount >= columnPrice)//计算 纸币 和硬币 扣钱规则
					{
						curCoinAmount -=  columnPrice;
					}
					else
					{
						curBillAmount = (curBillAmount - (columnPrice - curCoinAmount));
						curCoinAmount = 0;
					}
					hasBuy = 1;
					payoutEnable = 1;//购买商品后方能找零
					stTotalTrade.tradeAmount += columnPrice;
					stTotalTrade.tradeNums++;
					saveOneTradeInfo(columnNo, columnPrice,res);//保存一次出货记录
					disp_outgoods_page(1);//请取货
					
				}
				else
				{
					saveOneTradeInfo(columnNo, columnPrice,0);//保存一次出货记录	
					disp_outgoods_page(0);
				}
				
				checkDeviceState();//刷新故障码

				if(remainAmount)//一次出货后 判断余额
				{
					disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);//刷新金额页面	
					//查询 纸币器收币后选择是否 大于最大可用货道单价
					if(curBillAmount >= getColumnsMaxPrice())
					{
						deviceEnable(FALSE);
					}
					else
						deviceEnable(TRUE);
					
					CurrentState = MAIN_FREE;
				}	
				else
					CurrentState = MAIN_OVER;
				
				OSQFlush(g_KeyMsg);//计算出一次货后清空 选货按键消息队列
				break;
			case MAIN_PAYOUT://找零
				disp_sale_page(hasBuy,remainAmount,SYSPara.nPoinValue);//刷新金额页面
				DispPayoutPage();//正在找零
				deviceEnable(FALSE);
				if(HopperDispence(remainAmount,&iouMoney))//显示下要找零的金额
				{
					//找零成功
					DispQuChangePage();//取零钱
				}
				else
				{
					//找零失败
					DispIOUPage(iouMoney);
					stTotalTrade.iouAmount += iouMoney;
				}	
					
				CurrentState = MAIN_OVER;	
				break;
			case MAIN_OVER:
				saveHuodaoInfo(TYPE_HUODAO_REMAIN | TYPE_HUODAO_TRADE_COUNT);//保存货道信息			
				msleep(100);
				saveDetailTradeInfo();//保存详细交易信息
				msleep(100);
				saveTradeInfo();	//保存交易信息
				DispEndPage();		//交易结束			
				msleep(300);
				CurrentState = MAIN_START;
				break;
				
			case MAIN_FAULT:
				disp_err_page();
				deviceEnable(FALSE);
				while(1)
				{
					GotoWeiHuMenu(CurrentState);
					//checkAllDevice();
					if(HardWareErr == SYS_ERR_NO_NORMAL)//故障清除
					{
						disp_free_page(hopperIsEmpty());	
						CurrentState = MAIN_START;
						break;
					}
					else
					{
						CurrentState = MAIN_FAULT;
						//deviceEnable(FALSE);
					}
				}
				break;
			default:
				break;
		}

		msleep(10);
		
	}	
}




/*********************************************************************************************************
** Function name:       TASK_Trade
** Descriptions:       交易主任务
** input parameters:   
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

void TASK_Trade(void *pvData)
{
	pvData = pvData;

	//系统基本接口初始化
	SystemInit();
	//建立邮箱、信号量	
	CreateMBox();
	//建立交易任务和设备维护任务

   //创建 设备 包括硬币器 纸币器 hopper找零器 货道任务
	OSTaskCreateExt(TASK_Device, 
				(void *)0,
				&TASK_Device_STACK[TASK_Device_STACK_SIZE-1],
				TASK_Device_PRIO, 
				TASK_Device_ID, 
				&TASK_Device_STACK[0], 
				TASK_Device_STACK_SIZE, 
				(void *)0,
				OS_TASK_OPT_STK_CHK);

	//创建按键任务
	OSTaskCreateExt(TASK_DispState, 
				(void *)0,
				&TASK_DispState_STACK[TASK_DispState_STACK_SIZE-1],
				TASK_DispState_PRIO, 
				TASK_DispState_ID, 
				&TASK_DispState_STACK[0], 
				TASK_DispState_STACK_SIZE, 
				(void *)0,
				OS_TASK_OPT_STK_CHK);

#if VMC_TYPE == VMC_TYPE_TRADE
	pcControlInit();//创建PC管理任务
	trade_task_souhuo();		
#else 
	trade_task_changer();
#endif	
}



void TASK_DispState(void *pdata)
{
#if VMC_TYPE == VMC_TYPE_TRADE
	unsigned char selectKeyIn=0,SelectKey=0;
	while(1)
	{
		SelectKey = IsSelectKeyIn();
		if(SelectKey!=0)
		{
			selectKeyIn = SelectKey;
			OSQPost(g_KeyMsg,(void *)&selectKeyIn);
		}
		OSTimeDly(10);	
	}
#else
	while(1)
	{
		OSTimeDly(1000);
	}
#endif
	

}




/*********************************************************************************************************
** Function name:       main
** Descriptions:          用户程序入口函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int main(void)
{
    targetInit();																//初始化目标板
    pinInit();      															//IO初始化                                                                     
    OSInit();                                                                                                       
	//创建 交易主任务	
	OSTaskCreateExt(TASK_Trade, 
					(void *)0,
					&TASK_Trade_STACK[TASK_Trade_STACK_SIZE-1],
					TASK_Trade_PRIO, 
					TASK_Trade_ID, 
					&TASK_Trade_STACK[0], 
					TASK_Trade_STACK_SIZE, 
					(void *)0,
					OS_TASK_OPT_STK_CHK);

	OSStart();																	//启动多任务
}
/**************************************End Of File*******************************************************/
