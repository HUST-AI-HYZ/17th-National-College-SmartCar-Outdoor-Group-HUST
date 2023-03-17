/**
******************************************************************************
* @file     HAL_device.h
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    CMSIS Cortex-M Peripheral Access Layer for MindMotion
*           microcontroller devices
*
*           This is a convenience header file for defining the part number on the
*           build command line, instead of specifying the part specific header file.
*
*           Example: Add "-MM32SPIN0x_q" to your build options, to define part
*           Add "#include "HAL_device.h" to your source files
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

#ifndef __HAL_device_H
#define __HAL_device_H

#define  MM32SPIN2xx_p

#if defined(MM32SPIN2xx_p)
#include "MM32SPIN2xx_p.h"
#include "system_MM32SPIN2xx_p.h"
#else
#error "HAL_device.h: PART NUMBER undefined"
#endif
#endif /* __HAL_device_H */

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
