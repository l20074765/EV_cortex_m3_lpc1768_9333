/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           LANGUAGE.C
** Last modified Date:  2013-03-06
** Last Version:        No
** Descriptions:        菜单语言                    
**------------------------------------------------------------------------------------------------------
** Created by:          gzz
** Created date:        2013-03-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"



/*************************************维护界面货道菜单************************************************/
/*
const char *     Menu_Channel[LANGUAGENO][5]=
{
	 {
	 "1.货道编号: ",
	 "2.单价:     ",
	 "3.商品余量: ",
	 "4.货道状态: ",
	 "5.商品编号: "	 
	 },
	 {
	 "1.Column code:",
	 "2.Price:",
	 "3.Goods remain:",
	 "4.Column status:",
	 "5.Goods NO:"
	 },
	 //change by liya 2012-10-16
	 {
	 "1.Код колонки: ",
	 "2.Цена: ",
	 "3.Остатки еды: ",
	 "4.Статус колонки: ",
	 "5. Номер еды: "
	 },
	 //土耳其语;by gzz 20121115
	 {
	 	"1 S\xf0\xfctun kodu:",
		"2.Fiyat:",
		"3.Kalan \xf0\xfcr\xf0\xfcn:",
		"4 S\xf0\xfctun  durumu:",
		"5.\xf0\xdcr\xf0\xfcn numaras\xf1\x31:"
	 }
};
*/










/*************************************交易界面空闲界面************************************************/
const char *     BUSINESS[MAXLANGUAGETYPE][10]=
{
	 {
	 "输入商品编号:",
	 "输入错误",
	 "禁能",
	 "单价:",
	 "故障",
	 "缺货", 
	 "欢迎选购",
	 "货道",
	  "无零币",
	  "本机不收纸币"
	 },
	 {
	 "Input item code:",
	 "Invalid choice",
	 "Disabled",
	 "Price:",
	 "Error",
	 "Empty",
	 "Welcome",
	 "Channel"
	 },
	 {
	 "Введите код позиции:",
	 "Неправильный выбор",
	 "Выключено",
	 "Price:",
	 "Ошибка",
	 "Выдача",
	 "Welcome",
	 "Канал",
	 "Welcome",
	 "Канал"
	 },
	 {
	 "f0dcrf0fcn kodunu giriniz:",
	 "Girif15f hatalarf131",
	 "Disable",
	 "Price:",
	 "Hatalf131",
	 "Bitti",
	 "Welcome",
	 "Sf0fctun",
	 "Welcome",
	 "Канал"
	 },
	 {
	 "ENSERIR CODIGO:",
	 "ESCOLHA INVALIDA",
	 "SEM FUNCIONAMENTO",
	 "VALOR:",
	 "ERRO",
	 "VAZIO",
	 "BENVINDO",
	 "BANDEJA",
	 "Welcome",
	 "Канал"
	 }
};

/*************************************交易界面故障界面************************************************/
const char  *BUSINESSERROR[MAXLANGUAGETYPE][22] =
{
   {
	  "暂停服务",   
	  "故障码:	",
	  "商品已售完",
	  "找零器故障",
	  "纸币器故障",
	  "硬币器故障",
	  "找币器缺币",
	  "所有货道无货",
	  "所有货道故障",
	  "货道板故障",
	  "货道不可用",
	  "系统参数故障",
	  "LCD 故障",
	  "货道单价异常",
	  "出货确认故障",
	  "Flash故障",
	  "时钟故障",
	  "取货机构故障",
	  "货柜门没关",
	  "故障",
	  "注意",
	  "暂无找零 请对应金额投币"
	  
   },
   {	
	  "Sorry, out of service",
	  "Err code: ",
	  "Channel err",
	  "Hopper is wrong",
	  "Bill err",
	  "Coin err",
	  "Hopper empty",
	  "Column empty",
	  "column err",
	  "column err",
	  "column err",
	  "system err",
	  "LCD err",
	  "price err",
	  "GOC err",
	  "Flash err",
	  "RTC err",
	  "FGP err",
	  "Door Open",
	  "Sorry",
	  "Caution",
	  "Unable to Give Change Now"
   },
   //change by liya 2012-10-16
   {
	  "Не работает",
	  "Код ошибки:",
	  "Ошибка канала",
	  "Хоппер ",
	  "Билл ",
	  "Монета",
	  "Отсутствие валюты",
	  "проданный",
	  "неисправность",
	  "неисправность",
	  "неисправность",
	  "параметры системы",
	  "LCD",
	  "Цена неправильно",
	  "GOC",
	  "Flash",
	  "RTC",
	  "FGP",
	  "двери контейнера",
	  "извините",
	  "извините",
	  "извините"
   },
   //土耳其语;by gzz 20121115
   {
	  "Servis d\xf1\x31\xf1\x5f\xf1\x31",   
	  "Hata kodu:  ",
	  "\xf0\xdcr\xf0\xfcn sat\xf1\x31ld\xf1\x31",
	  "De\xf1\x1fi\xf1\x5fikli\xf1\x1fi hatas\xf1\x31 tutun",
	  "Bill yetmezli\xf1\x1fi",
	  "Coin hatas\xf1\x31",
	  "Paralar eksik sikke bul",
	  "T\xf0\xfcm g\xf0\xF6nderiler hi\xf0\xe7bir mal road",
	  "T\xf0\xfcm Kargo Karayolu fay",
	  "Kargo Karayolu kart\xf1\x31 ar\xf1\x31zas\xf1\x31",
	  "Kargo Karayolu kullan\xf1\x31lam\xf1\x31yor",
	  "Sistem parametreleri ar\xf1\x31zas\xf1\x31",
	  "LCD ar\xf1\x31za",
	  "Fiyat anormal",
	  "GOC ar\xf1\x31za",
	  "Flash ar\xf1\x31za",
	  "RTC ar\xf1\x31za",
	  "FGP ar\xf1\x31za",
	  "Kap\xf1\x31 kapal\xf1\x31 de\xf1\x1fil Pick",
	  "\xf0\xfczg\xf0\xfcn\xf0\xfcm",
	  "\xf0\xfczg\xf0\xfcn\xf0\xfcm",
	  "\xf0\xfczg\xf0\xfcn\xf0\xfcm"
   },
   {	
	  "DESLIGADO",
	  "CODIGO ERRADO: ",
	  "Channel err",
	  "Hopper is wrong",
	  "Bill err",
	  "Coin err",
	  "Hopper empty",
	  "Column empty",
	  "column err",
	  "column err",
	  "column err",
	  "system err",
	  "LCD err",
	  "price err",
	  "GOC err",
	  "Flash err",
	  "RTC err",
	  "FGP err",
	  "Door Open",
	  "Sorry",
	  "Sorry",
	  "Sorry"
   }
};



/*************************************交易界面交易界面************************************************/
const char  *BUSINESSDEAL[MAXLANGUAGETYPE][18] =
{
   {	  
	  "金额",
	  "余额",
	  "请插入更小面额纸币",
	  "金额不足",
	  "请按键选购",
	  "继续选购",
	  "找零退币",
	  "输入商品编号:",
	  "输入错误",
	  "故障",
	  "缺货",
	  "正在出货..",
	  "选购成功, 请取货",
	  "商品已售完",
	  "零钱不足",
	  "正在扣款,请稍后..",
	  "扣款失败",
	  "计时:"
	  
   },
   {
   	  "Amount",	
	  "Balance",
	  "Pls insert a smaller bill",
	  "Insufficient fund",
	  "Pls choose the product",
	  "Continue",
	  "Coin refund",
	  "Input Item code:",
	  "Invalid choice",
	  "Error",
	  "Empty",
	  "Dispensing..",
	  "Taking the goods",
	  "Sold out",
	  "Insufficient change",
	  "Debiting, please wait ... ",
	  "Debit failed",
	  "Time:"
	  
	  
   },
   //change by liya 2012-10-16
   {
   	  "Баланс",
	  "Баланс",
	  "Вставьте поменьше",
	  "Найдено несоотв,",
	  "Пожалуйста, нажмите купить",
	  "продолжать",
	  "CASHOUT"	 ,
	  "Введите код позиции:",
	  "Неправильный выбор",
	  "Ошибка",
	  "Выдача",
	  "Захват еды..",
	  "Заберите еду",
	  "Продано",
	  "Изменить нехватки",
	  "дебетуется..",
	  "Fail",
	  "Time:"
	   
   },
   //土耳其语;by gzz 20121115
   {
   	  "Bakiye",
	  "Bakiye",
	  "Kucuk banknot girin",
	  "Yetersiz fon,",
	  "xf0xdcrxf0xfcn kodunu giriniz",
	  "Yeni xf0xfcrxf0xfcn",
	  "Para iade",
	  "xf0xdcrxf0xfcn kodunu giriniz:",
	  "Girixf1x5f hatalarxf1x31",
	  "Hatalxf1x31",
	  "Bitti",
	  "xf0xdcrxf0xfcn veriliyor ,bekleyiniz..",
	  "xf0xfcrxf0xfcnxf0xfc alabilirsiniz",
	  "Urun yok",
	  "sxf1x31kxf1x31ntxf1x31sxf1x31 dexf1x1fixf1x5ftirin",
	  "borxf0xe7 olmak..",
	  "Fail",
	  "Time:"
   },
   {
   	  "VALOR",	
	  "SALDO",
	  "POR FAVOR ENSERIR MENOR NOTA",
	  "SALDO INSUFICIENTE,",
	  "ESCOLHA O PRODUTO",
	  "CONTINUE",
	  "TROCO MOEDA",
	  "ENSERIR CODIGO:",
	  "ESCOLHA INVALIDA",
	  "ERRO",
	  "VAZIO",
	  "PREPARANDO PARA SAIR",
	  "PEGAR PRODUTOS",
	  "SEM PRODUTO",
	  "TROCO  INSUFICIENTE",
	  "COBRANDO, OBRIGADO ... ",
	  "FALHA NA COBRAN\x83\A",
	  "TEMPO:"
	  
	  
   }
};

/*************************************交易界面找零界面************************************************/
const char  *BUSINESSCHANGE[MAXLANGUAGETYPE][5] =
{
   {
      "正在找零",	  
	  "请致电客服热线,欠条:",
	  "请取零钱",
	  "品味旅途，收藏记忆",
	  "谢谢惠顾!"
   },
   {
      "Changing",
	  "IOU:",	  
	  "Please take the changes",
	  "Thank You",
	  " "
   },
   //change by liya	2012-10-16
   {
      "Замена:",
	  "я долен Вам: ",	 
	  "Набор монет",
	  "Спасибо",
	  "Спасибо"
	  
   },
   //土耳其语;by gzz 20121115
   {
      "Dexf1x1fixf1x5fim:",
	  "Borxf0xe7 makbuzu: ",	 
	  "xf0xfcrxf0xfcnxf0xfc dexf1x1fixf1x5ftirebilirsiniz",
	  "Texf1x5fekkxf0xfcrler",
	  "Texf1x5fekkxf0xfcrler"
   },
   {
      "TROCANDO",
	  "IOU:",	  
	  "POR FAVOR PEGUE O TROCO",
	  "OBRIGADO",
	  "OBRIGADO"
   }
};




