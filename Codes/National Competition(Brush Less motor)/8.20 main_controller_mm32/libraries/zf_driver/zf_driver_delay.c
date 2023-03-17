/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_driver_delay
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#include "zf_driver_delay.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief		system 延时函数
// @param		time			需要延时的时间 纳秒级别
// @param		num				需要延时的次数
// @return		void
// Sample usage:				无需用户调用 用户请使用 zf_driver_system.h 文件中的宏定义
//-------------------------------------------------------------------------------------------------------------------
void system_delay (uint32 time, uint32 num)
{
	while(num --)
	{
		SysTick->CTRL	= 0x00;
		SysTick->LOAD	= (system_clock / 1000000 * time / 1000);
		SysTick->VAL	= 0x00;
		SysTick->CTRL	=	SysTick_CTRL_CLKSOURCE_Msk |                        // 时钟源选择 (core clk)
							SysTick_CTRL_ENABLE_Msk;                            // 使能 systick
		while( !(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
	}
}
