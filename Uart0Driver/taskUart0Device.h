

#ifndef __TASKUART0DEVICE_H__
#define __TASKUART0DEVICE_H__


//调试宏
#define  PRINT_ENABLE         1
#if PRINT_ENABLE
#define  print_log(...)       Trace(__VA_ARGS__)
#define  print_err(errno)        Trace("ERROR: In %s at Line %u - errno = %d\n", __FUNCTION__, __LINE__, errno)
#define  print_warning(no)		 Trace("Warning: In %s at Line %u - no = %d\n", __FUNCTION__, __LINE__, no)
#define  print_huodao(...)    Trace(__VA_ARGS__)
#else 
#define  print_log(...)       do {} while(0)
#define  print_err(errno)      do {} while(0)
#define  print_warning(no)	  do {} while(0)
#define  print_huodao(...)   do {} while(0)

#endif


#if VMC_TYPE == VMC_TYPE_TRADE


//=================针对小机型的配置===========================
#define VMC_SMALL_TYPE          1
#define VMC_NORMAL_TYPE         2
#define VMC_MACHINE_TYPE		VMC_SMALL_TYPE
//============================================================

//PCM :pc control the vmc
#define PCM_MAX_SIZE		5//消息队列大小
#define UART0_SIZE			10
typedef struct _pcconmsg_st
{
	unsigned char type;
	unsigned char msg[20];

}PCCONMSG;

typedef struct _pcm_msg{
	//报文头
	//发送包
	unsigned char  send_msg[256];
	unsigned short send_len;
	//应答包
	unsigned char  recv_msg[256];
	unsigned short recv_len;
	
}PCM_MSG;


extern OS_EVENT *pcConMail;
extern OS_EVENT *pcConMailBack;
extern PCCONMSG vmc_pc_mail[PCM_MAX_SIZE];
extern PCCONMSG pc_vmc_mail;
extern OS_STK pcControlTaskStk[512];


unsigned char pcControlInit(void);
void enterPCControl(void *pd);
void mainTaskPollPC(unsigned char state);
void pcPost(unsigned char type);
void vmcPost(unsigned char type);
void pcTesttingAisle(PCCONMSG *mail);
#endif

#endif
