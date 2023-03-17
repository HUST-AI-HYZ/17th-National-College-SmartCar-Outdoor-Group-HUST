#ifndef _gpio_H
#define _gpio_H	 

#include "common.h"



#define LED_RUN_PORT        GPIOC
#define LED_RUN_PIN         GPIO_Pin_15

#define LED_FAULT_PORT      GPIOC
#define LED_FAULT_PIN       GPIO_Pin_14

#define LED_EN_PORT         GPIOB
#define LED_EN_PIN          GPIO_Pin_13



typedef enum
{
    LED_ON,
    LED_OFF,
}LED_STATUS_enum;




void led_run_control(LED_STATUS_enum status);
void led_fault_control(LED_STATUS_enum status);
void led_en_control(LED_STATUS_enum status);
void led_init(void);


#endif



