#ifndef _ZIKULIST_H
#define _ZIKULIST_H


extern const unsigned char CAlib[][32];


unsigned char GetPic(unsigned char mode,unsigned char *strAddress);

unsigned char GetLOGO(unsigned char mode,unsigned char *strAddress);

unsigned char GetPituLibStr(unsigned char *str,unsigned char *strAddress,unsigned char reverse, unsigned char ziti);



#endif
