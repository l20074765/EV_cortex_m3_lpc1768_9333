#ifndef _BILLDEVCONTROL_H
#define _BILLDEVCONTROL_H


extern unsigned char NowBillDev;

extern void BillDevInit(void);

extern void BillDevDisable(void);

extern void BillDevEnable(void);

extern unsigned char BillDevProcess(unsigned int *RecvMoney,unsigned char *BillType);

extern unsigned char BillDevIsBillIn(unsigned char *InBillType,unsigned int *InBillValue);

extern void GetBillDevState(void);

extern void BillDevReject(void);

extern unsigned char CoinDevProcess(void);

extern unsigned int CoinDevGetIn(void);
unsigned int getCoinMoney(void);

void deviceEnable(unsigned char flag);


#endif
