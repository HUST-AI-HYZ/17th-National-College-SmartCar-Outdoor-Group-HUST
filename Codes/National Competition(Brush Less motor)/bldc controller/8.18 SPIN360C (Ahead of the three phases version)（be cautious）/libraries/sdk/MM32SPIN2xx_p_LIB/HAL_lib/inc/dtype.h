/**
******************************************************************************
* @file     dtype.h
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    type define

******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MindMotion SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2019 MindMotion</center></h2>
*/

#ifndef _DTYPE_H_
#define _DTYPE_H_

#define __I volatile const
#define __O volatile
#define __IO volatile

typedef unsigned char int8u;
typedef signed char int8s;
typedef unsigned short int16u;
typedef signed short int16s;
typedef unsigned int int32u;
typedef signed int int32s;
typedef float fp32;
typedef double fp64;

//typedef   signed          char int8_t;
//typedef   signed short     int int16_t;
//typedef   signed           int int32_t;
//
//typedef unsigned          char uint8_t;
//typedef unsigned short     int uint16_t;
//typedef unsigned           int uint32_t;
//
//typedef int32_t   s32;
//typedef int16_t   s16;
//typedef int8_t    s8;
//typedef uint32_t  u32;
//typedef uint16_t  u16;
//typedef uint8_t   u8;
/*
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
*/
#endif //_DTYPE_H_
/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/*-------------------------(C) COPYRIGHT 2019 MindMotion ----------------------*/
