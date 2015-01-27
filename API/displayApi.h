/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.h
** Last modified Date:  2014-04-04
** Last Version:        No
** Descriptions:        系统参数定义及通用函数部分                     
**------------------------------------------------------------------------------------------------------
** Created by:          yanbo
** Created date:        2013-04-04
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#ifndef _DISPLAYAPI_H_ 
#define _DISPLAYAPI_H_





void initDisplay(void);
void disp_clear_screen(void);
void disp_free_page(unsigned char vmcChangeLow);






void DispBusinessText(char *strlanguage,char *streng,char *rightstrlanguage,char *rightstreng);
void CLrBusinessText(void);
void DispFreePage(uint8_t vmcChangeLow);
void DispErrPage(void);
void DispSalePage(uint8_t haveSale,uint32_t invalue,uint8_t nPoinValue);

void DispColumnPage(uint8_t  columnNO,uint32_t invalue,uint8_t nPoinValue);
void DispColumnErrPage(void);
void DispCannotBuyPage(const unsigned int invalue);
void DispChuhuoPage(void);
void DispQuhuoPage(void);
void DispPayoutPage(void);
void DispQuChangePage(void);
void DispEndPage(void);
void DispIOUPage(uint32_t debtMoney);
void DispVersionText(void);
void DispChhuoFailPage(void);


void disp_err_page(void);

void disp_sale_page(unsigned char hasBuy,unsigned int amount,unsigned char point);

void disp_payout_page(void);
void disp_qu_change_page(void);
void disp_init_page(unsigned char type);
void disp_outgoods_page(unsigned char type);



void ledPaomaDisplay(void);











#endif


