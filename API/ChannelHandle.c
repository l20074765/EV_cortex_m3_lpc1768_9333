#include "ChannelHandle.h"
#include "..\config.h"
#include "Public.h"


//通信包序列号
uint8_t chSn = 1;


//货道的物理地址列表
unsigned char ColumnAdd[8]={0x01,0x04,0x05,0x06,0x02,0x07,0x08,0x09};

const unsigned char ColumnAddr[2][10] = {
	{0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A},
	{0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A}
	};


/*********************************************************************************************
**函   数   名: hd_logic_init
**函数功能说明：货道逻辑号初始化
**函数传入参数：cabinetNo 柜号
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/

void hd_logic_init(unsigned char cabinetNo)
{
	unsigned char logocNoIndex = 1,i,j,r = 0,c = 0;		
	for(i = 0;i < COLUMN_LEVEL_NUM;i++)
	{
		if(!stHuodao[i].isOpen)
			continue;
		for(j = 0,c = 0;j < COLUMN_ONE_LEVEL_NUM;j++)
		{
			if(!stHuodao[i].stColumn[j].isOpen)
			{
				stHuodao[i].stColumn[j].selectNo = 0;
				stHuodao[i].stColumn[j].logicNo = 0;
				stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;
			}
			else
			{
				if(c >= 9)
					stHuodao[i].stColumn[j].logicNo = (r + 1) * 10;	
				else
					stHuodao[i].stColumn[j].logicNo = (r + 1) * 10 + 1 + c;
				c++;
				stHuodao[i].stColumn[j].selectNo = logocNoIndex++;
				stHuodao[i].stColumn[j].state = HUODAO_STATE_NORMAL;						
			}
		}
		
		if(stHuodao[i].isOpen && c)
			r++;
					
	}
}


/*********************************************************************************************
**函   数   名: hd_init
**函数功能说明：新货道初始化函数
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
void hd_init(void)
{
	unsigned char i,j,res;

	readHuodaoInfo(TYPE_HUODAO_ALL);
	for(i = 0; i < COLUMN_LEVEL_NUM;i++)
	{
		if(!stHuodao[i].isOpen)  
			continue;
		for(j = 0; j < COLUMN_ONE_LEVEL_NUM;j++)
		{
			if(!stHuodao[i].stColumn[j].isOpen || !stHuodao[i].stColumn[j].logicNo) 
				continue;
			
			res = ChannelHandle(COLUMN_CHECK_EMPTY,stHuodao[i].stColumn[j].logicNo);
			if(res == COLUMN_RST_OK)
			{
				if(stHuodao[i].stColumn[j].remainCount)
					stHuodao[i].stColumn[j].state = HUODAO_STATE_NORMAL;
				else
					stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
			}
			else if(res == COLUMN_RST_EMPTY)
				stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
			else
				stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;

			
				
		}
	}
	
}


/*********************************************************************************************
**函   数   名: columnInit
**函数功能说明：新货道初始化函数
**函数传入参数：flag 0 初始化  1 读取flash
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
void columnInit(const unsigned char flag)
{
	unsigned char i,j;
	unsigned char level[COLUMN_LEVEL_NUM] = VMC_HUODAO_LEVEL_OPEN;
	unsigned char huodao[COLUMN_LEVEL_NUM][COLUMN_ONE_LEVEL_NUM] = VMC_HUODAO_COLUMN_OPEN;
	//全货道初始化
	for(i = 0; i < COLUMN_LEVEL_NUM;i++)
	{
#if VMC_HUODAO_OPEN_CONFIG == 1
		stHuodao[i].isOpen = level[i];
#else
		stHuodao[i].isOpen = 1;
#endif	
		
		for(j = 0;j < COLUMN_ONE_LEVEL_NUM;j++)
		{			
			stHuodao[i].stColumn[j].addr = ColumnAddr[i][j];
			stHuodao[i].stColumn[j].physicNo = (i + 1) * 10 + j + 1;	
			stHuodao[i].stColumn[j].logicNo = 0;
			stHuodao[i].stColumn[j].remainCount = 10;
			stHuodao[i].stColumn[j].maxCount = 10;
			stHuodao[i].stColumn[j].tradeCount = 0;
			stHuodao[i].stColumn[j].state = HUODAO_STATE_NORMAL;
			stHuodao[i].stColumn[j].price = 100;
#if VMC_HUODAO_OPEN_CONFIG == 1
			stHuodao[i].stColumn[j].isOpen = huodao[i][j];
#else
			stHuodao[i].stColumn[j].isOpen = 1;
#endif			
			
			Trace("column init..physicNo =%d\r\n",stHuodao[i].stColumn[j].physicNo);
		}
	}
		
	hd_logic_init(1);
	saveHuodaoInfo(TYPE_HUODAO_ALL);
	

	Trace("column init ..over..flag = %d\r\n",flag);

}



/*********************************************************************************************
**函   数   名: checkAllColumn
**函数功能说明：根据type检查全货道
**函数传入参数：type 类型 1 故障状态  2剩余量  3 单价
**              
**函数传出参数：无
**函 数 返回值：1 成功     0失败
**********************************************************************************************/
unsigned char checkAllColumn(unsigned char type)
{
	unsigned char i,j;
	unsigned int value;
	for(i = 0;i < COLUMN_LEVEL_NUM;i++)
	{
		if(!stHuodao[i].isOpen)
			continue;
		for(j = 0;j < COLUMN_ONE_LEVEL_NUM;j++)
		{
			if(!stHuodao[i].stColumn[j].isOpen)
				continue;
			
			if(type == 1)
				value = ((stHuodao[i].stColumn[j].state == HUODAO_STATE_NORMAL) || 
							(stHuodao[i].stColumn[j].state == HUODAO_STATE_EMPTY)) ? 1 : 0 ;	
			else if(type == 2)
				value = ((stHuodao[i].stColumn[j].remainCount != 0) &&
						(stHuodao[i].stColumn[j].state == HUODAO_STATE_NORMAL)) ? 1 : 0;
			else if(type == 3)	
				value = stHuodao[i].stColumn[j].price;
			else
				return 0;
			//Trace("type = %d stHuodao[%d][%d] vlaue = %d\r\n",type,i,j,value);
			if(value)
				return 1;
			
		}
	}

	return 0;
}




/*********************************************************************************************
**函   数   名: getColumnIndex
**函数功能说明：根据type 获得货道结构体的索引
**函数传入参数：type 类型 1 根据物理货道编号 货道索引  2逻辑号  3按键号
**              levelIndex 层号索引  columnIndex 货道号索引
**函数传出参数：无
**函 数 返回值：1 成功  0失败
**********************************************************************************************/
unsigned char getColumnIndex(unsigned char *levelIndex,unsigned char *columnIndex,
						unsigned char type,unsigned char no)
{	

	unsigned char i,j,tempNo = 0;
	if(!no)
	{
		Trace("getColumnIndex no is 0...\r\n");
		*levelIndex = 0;
		*columnIndex = 0;
		return 0;
	}
		
	
	for(i =0; i < COLUMN_LEVEL_NUM ;i++)
	{
		for(j = 0;j < COLUMN_ONE_LEVEL_NUM;j++)
		{
			
			if(type == 1)
				tempNo = stHuodao[i].stColumn[j].physicNo;
			else if(type == 2)
				tempNo = stHuodao[i].stColumn[j].logicNo;
			else if(type == 3)
				tempNo = stHuodao[i].stColumn[j].selectNo;
			else 
				tempNo = stHuodao[i].stColumn[j].physicNo;

			//print_huodao("tempNo[%d][%d] = %d  no = %d \r\n",i,j,tempNo,no);
			if(tempNo == no)
			{			
				*levelIndex = i;
				*columnIndex = j;
				return 1;
			}
		}
		
	}

	
	*levelIndex = 0;
	*columnIndex = 0;
	return 0;

	
	
}


/*********************************************************************************************
**函   数   名: getColumnInfoByIndex
**函数功能说明：根据索引号获得货道结构体信息
**函数传入参数：i 层号索引  j 货道号索引
**函数传出参数：无
**函 数 返回值：1 成功  0失败
**********************************************************************************************/
unsigned int getColumnInfoByIndex(unsigned char i,unsigned char j,unsigned char type)
{
	unsigned int value;
	switch(type)
	{
		case HUODAO_TYPE_ISOPEN:
			value = stHuodao[i].stColumn[j].isOpen;
			break;
		case HUODAO_TYPE_STATE:
			value = stHuodao[i].stColumn[j].state;
		break;
		case HUODAO_TYPE_REMAIN:
			value = stHuodao[i].stColumn[j].remainCount;
		break;
		case HUODAO_TYPE_PRICE:
			value = stHuodao[i].stColumn[j].price;
		break;
		case HUODAO_TYPE_TRADE_NUM:
			value = stHuodao[i].stColumn[j].tradeCount;
		break;
		
		case HUODAO_TYPE_TRADE_MAXCOUNT:
			value = stHuodao[i].stColumn[j].maxCount;
		break;
		default:
			value  = 0;
			break;
	}


	return value;
	
}

/*********************************************************************************************
**函   数   名: setColumnInfoByIndex
**函数功能说明：通过索引号设置货道信息
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char setColumnInfoByIndex(unsigned char i,unsigned char j,
						unsigned char type,unsigned int value)
{
	
	switch(type)
	{
		case HUODAO_TYPE_ISOPEN:
			stHuodao[i].stColumn[j].isOpen = (unsigned char)value;
			break;
		case HUODAO_TYPE_STATE:
			stHuodao[i].stColumn[j].state = (unsigned char)value;
		break;
		case HUODAO_TYPE_REMAIN:
			stHuodao[i].stColumn[j].remainCount = (unsigned char)value;;
		break;
		case HUODAO_TYPE_PRICE:
			stHuodao[i].stColumn[j].price  = (value);
		break;
		case HUODAO_TYPE_TRADE_NUM:
			stHuodao[i].stColumn[j].tradeCount = (unsigned char)value;;
		break;
		case HUODAO_TYPE_TRADE_MAXCOUNT:
			stHuodao[i].stColumn[j].maxCount= (unsigned char)value;;
		break;
		case HUODAO_TYPE_TRADE_LOGICNO:
			stHuodao[i].stColumn[j].logicNo = (unsigned char)value;
		break;
		default:
			
			break;
	}


	return 1;
}




/*********************************************************************************************
**函   数   名: fromPhysicGetLogicNum
**函数功能说明：新货道物理货道号转逻辑货道号
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char fromPhysicGetLogicNum(unsigned char cabinetNo,unsigned char physicNo)
{
	unsigned char i,j;
	cabinetNo = cabinetNo;
	if(physicNo && getColumnIndex(&i, &j,1,physicNo))
	{
		return stHuodao[i].stColumn[j].logicNo;
	}
	
	return 0;
}

/*********************************************************************************************
**函   数   名: fromLogicNoGetSelectNo
**函数功能说明：新货道物理货道号转逻辑货道号
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char fromLogicNoGetSelectNo(unsigned char cabinetNo,unsigned char logicNo)
{
	unsigned char i,j;
	cabinetNo = cabinetNo;
	if(logicNo && getColumnIndex(&i, &j,2,logicNo))
	{
		return stHuodao[i].stColumn[j].selectNo;
	}
	
	return 0;
}





/*********************************************************************************************
**函   数   名: fromSelectNoGetLogicNo
**函数功能说明：选货按键号转逻辑货道号
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char fromSelectNoGetLogicNo(unsigned char cabinetNo,unsigned char selectNo)
{
	unsigned char i,j;
	cabinetNo = cabinetNo;
	if(getColumnIndex(&i, &j,3,selectNo))
	{
		return stHuodao[i].stColumn[j].logicNo;
	}
	return 0;
}



/*********************************************************************************************
**函   数   名: getLevelOrColumnState
**函数功能说明：通过物理货道号获得货道信息
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned int getColumnInfoByPhysic(unsigned char cabinetNo,unsigned char physicNo,unsigned char type)
{

	unsigned char i,j;

	cabinetNo = cabinetNo;
	if(getColumnIndex(&i, &j,1,physicNo))
	{
		//Trace("getIndex i = %d,j =%d physicNo %d,type = %d\r\n",i,j,physicNo,type);
		return getColumnInfoByIndex(i,j,type);
	}
	return 0;
	
}



/*********************************************************************************************
**函   数   名: setColumnInfoByPhysic
**函数功能说明：通过物理货道号设置货道信息
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char setColumnInfoByPhysic(unsigned char cabinetNo,
						unsigned char physicNo,unsigned int value,unsigned char type)
{

	unsigned char i,j,err;

	cabinetNo = cabinetNo;
	err = getColumnIndex(&i, &j,1,physicNo);
	//Trace("setIndex i = %d,j =%d physicNo %d,type = %d,value =%d\r\n",i,j,physicNo,type,value);	
	if(err)
	{
		return setColumnInfoByIndex(i,j,type,value);
	}
	return 0;

	
}


/*********************************************************************************************
**函   数   名: getColumnInfoByLogic
**函数功能说明：通过逻辑货道号获得货道信息  逻辑货道十进制 11
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned int getColumnInfoByLogic(unsigned char cabinetNo,unsigned char logicNo,unsigned char type)
{

	
	unsigned char i,j;

	cabinetNo = cabinetNo;
	if(getColumnIndex(&i, &j,2,logicNo))
	{
		return getColumnInfoByIndex(i,j,type);
	}
	return 0;
}



/*********************************************************************************************
**函   数   名: setColumnInfoByLogic
**函数功能说明：通过逻辑货道号设置货道信息
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char setColumnInfoByLogic(unsigned char cabinetNo,
						unsigned char logicNo,unsigned int value,unsigned char type)
{
	unsigned char i,j;

	cabinetNo = cabinetNo;
	if(getColumnIndex(&i, &j,2,logicNo))
	{
		return setColumnInfoByIndex(i,j,type,value);
	}
	return 0;
}

/*********************************************************************************************
**函   数   名: getColumnInfoBySelectNo
**函数功能说明：通过逻辑按键号获得货道信息  
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned int getColumnInfoBySelectNo(unsigned char cabinetNo,unsigned char selectNo,unsigned char type)
{

	unsigned char i,j;

	cabinetNo = cabinetNo;
	if(getColumnIndex(&i, &j,3,selectNo))
	{
		return getColumnInfoByIndex(i,j,type);
	}
	return 0;
}



/*********************************************************************************************
**函   数   名: setColumnInfoBySelectNo
**函数功能说明：通过逻辑按键号设置货道信息
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char setColumnInfoBySelectNo(unsigned char cabinetNo,
						unsigned char selectNo,unsigned int value,unsigned char type)
{
	unsigned char i,j;

	cabinetNo = cabinetNo;
	if(getColumnIndex(&i, &j,3,selectNo))
	{
		return setColumnInfoByIndex(i,j,type,value);
	}
	return 0;
}


/*****************************************************************************
** Function name:	hd_open_handle	
** Descriptions:	      货道开关函数
**                            层操作后将重新对整个货道进行逻辑排号；
					单货道操作后,重新对该层的货道进行逻辑排号
** parameters:		cabinetNo：箱柜编号
					no:     物理货道编号/层号 					
					type：1——某货道操作；2——某层操作
					flag:	1——开启，0——关闭														
** Returned value:	1 成功  0失败
*******************************************************************************/
unsigned char hd_open_handle(unsigned char cabinetNo,unsigned char no,
								unsigned char type,unsigned char flag)
{
	unsigned char i,j,err,temp;
	cabinetNo = cabinetNo;//预留
	
	if(type == 2)//层操作
	{
		if(no < 1 || no > COLUMN_LEVEL_NUM)
			return 0;
		
		temp = (flag == 1) ? 1 : 0;
		if(stHuodao[no - 1].isOpen == temp)//此次操作作废即层开启又发来让其开启什么多不做直接返回1
			return 1;
		stHuodao[no - 1].isOpen = temp;
		for(j = 0;j < COLUMN_ONE_LEVEL_NUM;j++)//该层的所有货道执行相应的开关操作
		{
			if(!stHuodao[no - 1].stColumn[j].addr)
				stHuodao[no - 1].stColumn[j].isOpen = 0;
			else
				stHuodao[no - 1].stColumn[j].isOpen = temp;
		}
		hd_logic_init(cabinetNo);//重新排列逻辑号
		return 1;			
	}
	else//莫货道操作
	{
		err = getColumnIndex(&i,&j,1,no);
		if(err)
		{
			if(!stHuodao[i].isOpen)//层关闭了不允许操作货道开关
				return 0;
			if(!stHuodao[i].stColumn[j].addr)//物理地址不存在不允许操作
			{
				stHuodao[i].stColumn[j].isOpen = 0;
				return 0;
			}
			temp = (flag == 1) ? 1 : 0;
			if(stHuodao[i].stColumn[j].isOpen == temp)
				return 1;
			
			stHuodao[i].stColumn[j].isOpen = temp;
			hd_logic_init(cabinetNo);//重新排列逻辑号
			return 1;	
			
			
		}
	}
	
	return 0;
	
}



/*********************************************************************************************
**函   数   名:	ChannelHandle
**函数功能说明：货道操作函数
**函数传入参数：Handletype：操作类型；logicNo：受控货道逻辑货道号
**函数传出参数：无
**函 数 返回值：1：成功；0：操作失败；2 货道故障 3货道无货 4货道不存在5通讯失败
**********************************************************************************************/
uint8_t ChannelHandle(unsigned char Handletype,unsigned char logicNo)
{
	uint8_t ackdata[36],temp,rcx = 3;
	uint8_t res = 0,i,j;

	Trace("logicNo:%d\r\n",logicNo);
	if(!getColumnIndex(&i, &j, 2,logicNo))
	{
		return COLUMN_RST_N_A;
	}
	while(rcx--)
	{	
		res = EvbConversation(0,0xED,chSn,Handletype,stHuodao[i].stColumn[j].addr,0x00,ackdata);
		if(res == 1)
			break;
		msleep(500);
	}
	
	if(res == 1)
	{
		chSn++;
		temp = ackdata[1];//状态字节
		if(Handletype == COLUMN_OUTGOODS || Handletype == COLUMN_CHECK_RST)
		{	
			if(temp & 0x01)
			{
				Trace("COLUMN err status == %x\r\n",temp);
				if(temp & (0x03 << 4))//货道故障
				{
					return COLUMN_RST_FAULT;
				}
				if(temp & (0x01 << 3))//货道无货
				{
					return COLUMN_RST_EMPTY;
				}
				return COLUMN_RST_FAULT;
			}
			return COLUMN_RST_OK;
		}
		else if(Handletype == COLUMN_CHECK_EMPTY)
		{
			Trace("COLUMN check status == %x\r\n",temp);
			if(temp)//无货
			{
				return COLUMN_RST_EMPTY;
			}
			else  //有货
			{
				return COLUMN_RST_OK;
			}
		}
		else
			return COLUMN_RST_N_A;
		
	}

	return COLUMN_RST_COM;
	

}


/*********************************************************************************************
**函   数   名:	hd_outgoods
**函数功能说明：货道出货
**函数传入参数：logicNo：受控货道逻辑货道号
**函数传出参数：无
**函 数 返回值：1：成功；0：操作失败；2 货道故障 3货道无货 4货道不存在5通讯失败
**********************************************************************************************/
uint8_t hd_outgoods(unsigned char logicNo)
{
	unsigned char res,rCheck,i,j;
	if(!getColumnIndex(&i,&j, 2, logicNo))
	{
		return 0;
	}

	//执行出货操作
	res = ChannelHandle(COLUMN_OUTGOODS,logicNo);
	if(res == COLUMN_RST_OK)
	{
		//执行出货操作成功后查询出货结果
		rCheck = ChannelHandle(COLUMN_CHECK_RST,logicNo);
		if(rCheck == COLUMN_RST_OK)
		{
			if(stHuodao[i].stColumn[j].remainCount)
					stHuodao[i].stColumn[j].remainCount --;
			stHuodao[i].stColumn[j].tradeCount++;	
			
			//出货成功后检查余量
			res = ChannelHandle(COLUMN_CHECK_EMPTY,logicNo);
			if(res == COLUMN_RST_OK)
			{
				if(stHuodao[i].stColumn[j].remainCount == 0)
					stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
				else
					stHuodao[i].stColumn[j].state = HUODAO_STATE_NORMAL;
			}
			else if(res == COLUMN_RST_EMPTY)//缺货
			{
				stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
			}
			else
				stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;	
			
			return COLUMN_RST_OK;
			
		}
		else//出货不成功货道故障
		{
			if(rCheck == COLUMN_RST_EMPTY)
				stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;	
			else
				stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;	
			return rCheck;
		}
			
	}
	else //出货不成功货道故障
	{
		if(res == COLUMN_RST_EMPTY)
			stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;	
		else
			stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;	
		return res;
	}
		

}


/*********************************************************************************************
**函   数   名:	hd_outgoods_test
**函数功能说明：货道出货检测
**函数传入参数：logicNo：受控货道逻辑货道号
**函数传出参数：无
**函 数 返回值：1：成功；0：操作失败；2 货道故障 3货道无货 4货道不存在5通讯失败
**********************************************************************************************/
uint8_t hd_outgoods_test(unsigned char logicNo)
{
	unsigned char res,rCheck,i,j;
	if(!getColumnIndex(&i,&j, 2, logicNo))
	{
		return 0;
	}
	//执行出货操作
	res = ChannelHandle(COLUMN_OUTGOODS,logicNo);
	if(res == COLUMN_RST_OK)
	{
		//执行出货操作成功后查询出货结果
		rCheck = ChannelHandle(COLUMN_CHECK_RST,logicNo);
		if(rCheck == COLUMN_RST_OK)
		{
			//出货成功后检查余量
			res = ChannelHandle(COLUMN_CHECK_EMPTY,logicNo);
			if(res == COLUMN_RST_OK)
			{
				if(stHuodao[i].stColumn[j].remainCount == 0)
					stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
				else
					stHuodao[i].stColumn[j].state = HUODAO_STATE_NORMAL;
			}
			else if(res == COLUMN_RST_EMPTY)//缺货
			{
				stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
			}
			else
				stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;	
			
			return COLUMN_RST_OK;
		}
		else
		{
			if(rCheck == COLUMN_RST_EMPTY)
			stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;	
			else
				stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;	
			return rCheck;
		}
		

		
	}
	else //出货不成功货道故障
	{
		if(res == COLUMN_RST_EMPTY)
			stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;	
		else
			stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;	
		return res;
	}
		

}


/*********************************************************************************************
**函   数   名:	hd_check_state
**函数功能说明：货道状态检测
**函数传入参数：logicNo：受控货道逻辑货道号
**函数传出参数：无
**函 数 返回值：1：成功；0：操作失败；2 货道故障 3货道无货 4货道不存在5通讯失败
**********************************************************************************************/
uint8_t hd_check_state(unsigned char logicNo)
{
	unsigned char res,i,j;
	if(!getColumnIndex(&i,&j, 2, logicNo))
	{
		return COLUMN_RST_N_A;
	}
	//出货成功后检查余量
	res = ChannelHandle(COLUMN_CHECK_EMPTY,logicNo);
	if(res == COLUMN_RST_OK)
	{
		if(stHuodao[i].stColumn[j].remainCount == 0)
			stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
		else
			stHuodao[i].stColumn[j].state = HUODAO_STATE_NORMAL;
	}
	else if(res == COLUMN_RST_EMPTY)//缺货
	{
		stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
	}
	else
		stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;	
	
	return COLUMN_RST_OK;

	

}




/*********************************************************************************************
**函   数   名: hd_outgoods_send
**函数功能说明：发送货道出货操作邮箱
**函数传入参数：logicNO：货道逻辑地址
**函数传出参数：chlstate：货道状态
**函 数 返回值：0：失败；1：成功
**********************************************************************************************/
unsigned char hd_outgoods_send(unsigned char logicNO,unsigned char *chlstate)
{
	uint8_t err,temp;
	MAIN_DEV_TASK_MSG *nMsgPack;

	//给设备任务发送出货命令
	task_msg_main_to_dev.SubSendType = COLUMN_OUTGOODS;
	task_msg_main_to_dev.value_8 = logicNO;
	mbox_post_main_to_dev(TASK_TRADE_REQ);
	//等待出货结果返回
	nMsgPack = OSMboxPend(g_msg_dev_to_main,3000,&err);//12s

	Trace("rst = %d sendType = %d\r\n",nMsgPack->value_8,nMsgPack->SendType);
	if(err == OS_NO_ERR && nMsgPack->SendType == TASK_TRADE_RPT)
	{
		temp = nMsgPack->value_8;
		*chlstate = nMsgPack->value_8;	
		return ((temp == COLUMN_RST_OK) ?  1 : 0);
	}
	*chlstate = 0;
	return 0;


		
		
}


#if 0
/*********************************************************************************************
**函   数   名: ChannelhandleProcess
**函数功能说明：货道操作控制函数
**函数传入参数：handletype：操作类型；logicNO：货道逻辑地址
**函数传出参数：chlstate：货道状态
**函 数 返回值：0：失败；1：成功
**********************************************************************************************/
unsigned char ChannelhandleProcess(unsigned char handletype,unsigned char logicNO,unsigned char *chlstate)
{
	uint8_t err,i,j,temp;
	MAIN_DEV_TASK_MSG *nMsgPack;

	if(!getColumnIndex(&i,&j, 2, logicNO))
	{
		*chlstate = COLUMN_RST_N_A;
		return 0;
	}
	
	
	//给设备任务发送出货命令
	task_msg_main_to_dev.SubSendType = handletype;
	task_msg_main_to_dev.value_8 = logicNO;
	mbox_post_main_to_dev(TASK_TRADE_REQ);
	//等待出货结果返回
	nMsgPack = OSMboxPend(g_msg_dev_to_main,2500,&err);//12s

	Trace("HandleType==%d rst = %d sendType = %d\r\n",handletype,nMsgPack->value_8,nMsgPack->SendType);
	if(err == OS_NO_ERR && nMsgPack->SendType == TASK_TRADE_RPT)
	{
		temp = nMsgPack->value_8;
		*chlstate = nMsgPack->value_8;	
		if(temp == COLUMN_RST_OK)
		{
			if(handletype == COLUMN_OUTGOODS)
			{
				if(stHuodao[i].stColumn[j].remainCount)
					stHuodao[i].stColumn[j].remainCount --;
				stHuodao[i].stColumn[j].tradeCount++;	
			}
			
			if(stHuodao[i].stColumn[j].remainCount == 0)
				stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
			else
				stHuodao[i].stColumn[j].state = HUODAO_STATE_NORMAL;
			return 1;
		}
		else if(temp == COLUMN_RST_EMPTY)//缺货
		{
			stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
		}
		else
			stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;

	}

	return 0;


		
		
}
#endif

/*********************************************************************************************
**函   数   名: CheckColumnIsOk
**函数功能说明：检测货道是否可用  
**函数传入参数：handletype：操作类型；logicNO：货道逻辑地址
**函数传出参数：chlstate：货道状态
**函 数 返回值：0：未知 ；1：可出货 2 货道故障 3 货道缺货 4 货道不存在 5 余额不足 6单价为0
**********************************************************************************************/
unsigned char CheckColumnIsOk(unsigned char selectNo,unsigned int remainAmount,unsigned int *price)
{
	unsigned char i,j;
	*price = 0;
	if(!getColumnIndex(&i,&j,3,selectNo))
		return 4;

	*price = stHuodao[i].stColumn[j].price;
	if(!stHuodao[i].stColumn[j].price)
		return 6;

	if(remainAmount < stHuodao[i].stColumn[j].price)	
		return 5;

	if(!stHuodao[i].stColumn[j].remainCount)
		return 3;

	if(stHuodao[i].stColumn[j].state == HUODAO_STATE_EMPTY)
		return 3;
	
	if(stHuodao[i].stColumn[j].state != HUODAO_STATE_NORMAL)
		return 2;
	
	if(!stHuodao[i].stColumn[j].canShip)
		return 0;

	*price = stHuodao[i].stColumn[j].price;
	return 1;
	
	
}


//获取四个可用货道最大单价
unsigned int getColumnsMaxPrice(void)
{
	unsigned char i,j;
	unsigned int value = 0;
	for(i = 0;i < COLUMN_LEVEL_NUM;i ++)
	{
		if(!stHuodao[i].isOpen)
			continue;
		
		for(j = 0; j < COLUMN_ONE_LEVEL_NUM;j++)
		{
			if(stHuodao[i].stColumn[j].isOpen && (stHuodao[i].stColumn[j].state == HUODAO_STATE_NORMAL))
				value = (value > stHuodao[i].stColumn[j].price)  ? value : stHuodao[i].stColumn[j].price;
		}
	}
	
	return value;
}


/*********************************************************************************************
**函   数   名:	ChannelIsEmptyLed
**函数功能说明：
**函数传入参数：
**函数传出参数：无
**函 数 返回值：
**********************************************************************************************/
void ChannelIsEmptyLed()
{
	if(stHuodao[0].stColumn[1].state==2)
		FIO1CLR |= (1ul<<19); //灯1
	else
		FIO1SET |= (1ul<<19);
	if(stHuodao[0].stColumn[0].state==2)
		FIO1CLR |= (1ul<<18); //灯2
	else
		FIO1SET |= (1ul<<18);
	if(stHuodao[0].stColumn[3].state==2)
		FIO1CLR |= (1ul<<31);//灯3
	else
		FIO1SET |= (1ul<<31);
	if(stHuodao[0].stColumn[2].state==2)
		FIO1CLR |= (1ul<<20);//灯4
	else
		FIO1SET |= (1ul<<20);
}




/*********************************************************************************************
**函   数   名:	ChannelHandleTest
**函数功能说明：货道测试函数
**函数传入参数：Handletype：操作类型；columnNo：受控货道号
**函数传出参数：无
**函 数 返回值：1：成功；0：操作失败；
**********************************************************************************************/
uint8_t ChannelHandleTest(unsigned char columnNo)
{
	uint8_t ackdata[36];
	uint8_t res = 0,i,j,ClAdd = 0;

	if(!getColumnIndex(&i,&j,3,columnNo))
		return 0;
	ClAdd = stHuodao[i].stColumn[j].addr;
	
	res = EvbConversation(0,0xED,chSn,COLUMN_OUTGOODS,ClAdd,0x00,ackdata);
	if(res == 1)
	{
		chSn++;
		//OSTimeDly(350);
		//EvbConversation(0,0xED,chSn,CHECKCHANNEL,ClAdd,0x00,ackdata);
		if(ackdata[1]&0x01)
		{
			Trace("AckData==%x\r\n",ackdata[1]);
			//货道缺货
			if(ackdata[1]&0x08)
				stHuodao[i].stColumn[j].state = HUODAO_STATE_EMPTY;
			//货道故障
			if(ackdata[1]&0x10)
				stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;
			//出货失败
			return 0;
		}
		else
		{
			stHuodao[i].stColumn[j].state = HUODAO_STATE_NORMAL;
			//出货成功
			return 1;
		}
		
	}	
	else //没有返回应答包
	{
		stHuodao[i].stColumn[j].state = HUODAO_STATE_FAULT;//通信故障
		
	}
	return 0;

}

/*********************************************************************************************
**函   数   名: ColumnTestByPcPost
**函数功能说明：测试货道发送邮箱
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char ColumnTestByPcPost(unsigned char cabinetNo,unsigned char logicNo)
{
		
	//给设备任务发送出货命令
	task_msg_main_to_dev.SubSendType = COLUMN_OUTGOODS;
	task_msg_main_to_dev.value_8 = logicNo;

	mbox_post_main_to_dev(TASK_TRADE_REQ);
	OSTimeDly(2);
	return 1;
}



/*********************************************************************************************
**函   数   名: ColumnTestByPcPend
**函数功能说明：测试货道接收结果
**函数传入参数：无
**函数传出参数：无
**函 数 返回值：无
**********************************************************************************************/
unsigned char ColumnTestByPcPend(void)
{
	unsigned char err;
	MAIN_DEV_TASK_MSG *nMsgPack;
	nMsgPack = OSMboxPend(g_msg_dev_to_main,5,&err);
	if(err == OS_NO_ERR)
	{
		if(nMsgPack->value_8 == 1)//出货成功
		{
			return 0;
		}
		else
		{	
			return 1;
		}
			
	}
	return 0xFF;//超时
}



/*****************************************************************************
** Function name:	ChannelSaveParam	
**
** Descriptions:	货到信息保存
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
void ChannelSaveParam()
{
	saveHuodaoInfo(TYPE_HUODAO_ALL);

}


/*****************************************************************************
** Function name:	addColumnGoods	
**
** Descriptions:	货道添货
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char addColumnGoods(unsigned cabinetNo,unsigned char type,unsigned char no)
{
	unsigned char i,j;

	cabinetNo = cabinetNo;
	if(type == 1)//全货道添货
	{
		for(i =0;i < COLUMN_LEVEL_NUM;i++)
		{
			for(j = 0;j < 8;j++)
			{
				stHuodao[i].stColumn[j].remainCount = stHuodao[i].stColumn[j].maxCount;
			}
		}
		
			
	}
	else if(type == 2)//层添货
	{
		for(j = 0;j < 8;j++)
		{
			stHuodao[no - 1].stColumn[j].remainCount = stHuodao[no - 1].stColumn[j].maxCount;
		}
	}
	else if(type == 3)//单货道添货
	{
		getColumnIndex(&i,&j,2,no);		
		stHuodao[i].stColumn[j].remainCount = stHuodao[i].stColumn[j].maxCount;
	}

	return 1;

	
	
}


