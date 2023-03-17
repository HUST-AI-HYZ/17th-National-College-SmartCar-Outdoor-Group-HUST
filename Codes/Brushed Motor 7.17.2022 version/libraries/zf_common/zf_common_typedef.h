/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_common_typedef
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.32.4 or MDK 5.28
* @Target core      MM32F327X_G9P
* @Taobao           https://seekfree.taobao.com/
* @date             2022-04-11
********************************************************************************************************************/

#ifndef _zf_common_typedef_h_
#define _zf_common_typedef_h_

#include "stdio.h"
#include "stdint.h"
#include "stdarg.h"
#include "string.h"
#include "stdlib.h"

//=================================================== 类型定义 ===================================================

#define COMPATIBLE_WITH_OLDER_VERSIONS                                       	// 兼容旧版开源库接口
#define USE_ZF_TYPEDEF      (1)                                                 // 是否启用类型定义申明
#if USE_ZF_TYPEDEF
// 数据类型声明
// 尽量使用 stdint.h 定义的类型名称 避免冲突 这里可以裁剪
typedef unsigned char       uint8;                                              // 无符号  8 bits 
typedef unsigned short int  uint16;                                             // 无符号 16 bits 
typedef unsigned int        uint32;                                             // 无符号 32 bits 
typedef unsigned long long  uint64;                                             // 无符号 64 bits 

typedef signed char         int8;                                               // 有符号  8 bits 
typedef signed short int    int16;                                              // 有符号 16 bits 
typedef signed int          int32;                                              // 有符号 32 bits 
typedef signed long long    int64;                                              // 有符号 64 bits 

typedef volatile uint8      vuint8;                                             // 易变性修饰 无符号  8 bits 
typedef volatile uint16     vuint16;                                            // 易变性修饰 无符号 16 bits 
typedef volatile uint32     vuint32;                                            // 易变性修饰 无符号 32 bits 
typedef volatile uint64     vuint64;                                            // 易变性修饰 无符号 64 bits 

typedef volatile int8       vint8;                                              // 易变性修饰 有符号  8 bits 
typedef volatile int16      vint16;                                             // 易变性修饰 有符号 16 bits 
typedef volatile int32      vint32;                                             // 易变性修饰 有符号 32 bits 
typedef volatile int64      vint64;                                             // 易变性修饰 有符号 64 bits 
#endif

// 如果SDK使用了枚举体定义ENABLE和DISABLE 那么这里屏蔽
#ifndef ENABLE
#define ENABLE      (1)
#define DISABLE     (0)
#endif
//=================================================== 类型定义 ===================================================

#endif
