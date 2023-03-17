#ifndef _pwm_input_h
#define _pwm_input_h

#include "common.h"




#define MOTOR_DIR_PORT  GPIOA
#define MOTOR_DIR_PIN   GPIO_Pin_9

#define MOTOR_PWM_PORT  GPIOA
#define MOTOR_PWM_PIN   GPIO_Pin_10

extern float duty_cycle;
extern uint8 duty_flag;
extern int16 period, duty_low, duty_high;



void pwm_input_init(void);
uint8 pwm_dir_get(void);

















#endif
