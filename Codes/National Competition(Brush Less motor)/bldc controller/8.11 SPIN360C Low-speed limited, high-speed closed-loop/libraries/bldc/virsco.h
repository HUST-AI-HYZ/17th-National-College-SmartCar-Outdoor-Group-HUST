/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ����������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				SEEKFREE_VIRSCO
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note				ʾ�������ص�ַ��https://pan.baidu.com/s/198CMXTZsbI3HAEqNXDngBw�������廪ֱ���������ϣ�
********************************************************************************************************************/

#ifndef _virsco_H
#define _virsco_H

#include "common.h"

extern uint8 virtual_scope_data[10];

void data_conversion(int16 data1, int16 data2, int16 data3, int16 data4, uint8 *dat);

#endif 