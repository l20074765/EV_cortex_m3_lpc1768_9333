#ifndef _CHANNELHANDLE_H
#define _CHANNELHANDLE_H


/*********************************************************************************************
**				定义与货道驱动板的命令类型
**********************************************************************************************/

#define COLUMN_OUTGOODS			(0x70)        //出货操作
#define COLUMN_CHECK_RST		(0x71)			//检查上次出货结果
#define COLUMN_CHECK_EMPTY		(0x81)			//检查是否无货


/*********************************************************************************************
**				定义货道通讯结果原因
**********************************************************************************************/
#define COLUMN_RST_OK		(1)//出货成功
#define COLUMN_RST_FAULT	(2)//货道故障
#define COLUMN_RST_EMPTY	(3)//货道缺货
#define COLUMN_RST_N_A		(4)//货道不存在
#define COLUMN_RST_COM		(5)//通讯失败
#define COLUMN_RST_ZERO		(6)//单价为零




#if 0
#define CHANNEL_OUTGOODS		(1)			//出货操作
#define CHANNEL_CHECKSTATE		(2)			//检测货道当前状态
#define CHANNEL_CHECKOUTRESULT	(3)			//检测货道上次的出货结果
#define CHANNEL_ISEMPTY			(4)			//检测有无货
#define OUTGOODSCMD				(0x70)			//出货
#define CHECKCHANNEL			(0x71)			//查询出货结果
#define CHECKOUTRESULT			(0x81)			//查询当前状态

#endif


extern unsigned char ChannelHandle(unsigned char Handletype,unsigned char logicNo);

//extern unsigned char ChannelhandleProcess(unsigned char handletype,unsigned char logicNO,unsigned char *chlstate);

extern unsigned char CheckColumnIsOk(unsigned char selectNo,unsigned int remainAmount,unsigned int *price);

extern void ChannelIsEmptyLed(void);
extern unsigned char ChannelHandleTest(unsigned char columnNo);
extern void columnInit(const unsigned char flag);
extern unsigned char fromPhysicGetLogicNum(unsigned char cabinetNo,unsigned char physicNo);
extern unsigned char fromLogicNoGetSelectNo(unsigned char cabinetNo,unsigned char logicNo);
extern unsigned char fromSelectNoGetLogicNo(unsigned char cabinetNo,unsigned char selectNo);

extern unsigned int getColumnInfoByPhysic(unsigned char cabinetNo,unsigned char physicNo,unsigned char type);
unsigned char setColumnInfoByPhysic(unsigned char cabinetNo,
						unsigned char physicNo,unsigned int value,unsigned char type);


extern unsigned int getColumnInfoByLogic(unsigned char cabinetNo,unsigned char logicNo,unsigned char type);
extern unsigned char setColumnInfoByLogic(unsigned char cabinetNo,
						unsigned char logicNo,unsigned int value,unsigned char type);
extern unsigned int getColumnInfoBySelectNo(unsigned char cabinetNo,unsigned char selectNo,unsigned char type);
extern unsigned char setColumnInfoBySelectNo(unsigned char cabinetNo,
						unsigned char selectNo,unsigned int value,unsigned char type);

extern void ChannelSaveParam(void);
extern unsigned char addColumnGoods(unsigned cabinetNo,unsigned char type,unsigned char no);

extern unsigned char ColumnTestByPcPost(unsigned char cabinetNo,unsigned char logicNo);
extern	unsigned char ColumnTestByPcPend(void);
extern	unsigned char getColumnIndex(unsigned char *levelIndex,unsigned char *columnIndex,
						unsigned char type,unsigned char no);

extern	unsigned char checkAllColumn(unsigned char type);
extern unsigned int getColumnsMaxPrice(void);
extern unsigned char getColumnIndex(unsigned char *levelIndex,unsigned char *columnIndex,
						unsigned char type,unsigned char no);
unsigned char hd_open_handle(unsigned char cabinetNo,unsigned char no,
								unsigned char type,unsigned char flag);


void hd_init(void);
unsigned char hd_outgoods_send(unsigned char logicNO,unsigned char *chlstate);
unsigned char hd_outgoods(unsigned char logicNo);
unsigned char hd_check_state(unsigned char logicNo);
unsigned char hd_outgoods_test(unsigned char logicNo);


#endif
