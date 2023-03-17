/*
 * 文件名称：Vofa.h
 * 创建时间: 2020年7月15日
 * 最后修改：2020年7月15日
 * 文件作者: Wind_Just_Fall
 */
 
#ifndef _VOFA_H
#define _VOFA_H
#include "zf_common_headfile.h"

#include "parameter.h"


typedef union
{
	float fdata;
	unsigned long ldata;
}FloatLongType;



#define SEND_HEAD0	0x31
#define SEND_HEAD1	0x32
#define SEND_HEAD2	0x33
#define SEND_HEAD3	0x34

#define SEND_MAX_BUFF   256

#define SEND(buff, len)    seekfree_wireless_send_buff(buff, len)
void send_vars(uint8 number, Varible* vars);

void send_spin(void);
void send_spin_vofa(void);

void Float_to_Byte(float f,unsigned char byte[4]);
void Vofa_1data(float data);
void Vofa_2data(float data1,float data2);
void Vofa_3data(float data1,float data2,float data3);
void Vofa_4data(float data1,float data2,float data3,float data4);
void Vofa_5data(float data1,float data2,float data3,float data4,float data5);
void Vofa_7data(float data1,float data2,float data3,float data4,float data5,float data6,float data7);


#endif
