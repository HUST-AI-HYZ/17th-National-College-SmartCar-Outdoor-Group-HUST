
#ifndef _motor_H
#define _motor_H


#include "common.h"
#include "pid.h"




//定义电机转动速度输出引脚  电机越开引脚输出的波形频率越高
#define MOTOR_SPEED_OUT_PORT    GPIOA
#define MOTOR_SPEED_OUT_PIN     GPIO_Pin_12

//定义电机转动方向输出引脚      0：电机正转  1：电机反转   用于告知外部控制模块电机当前的运行方向
#define MOTOR_DIR_OUT_PORT      GPIOD
#define MOTOR_DIR_OUT_PIN       GPIO_Pin_2

//定义电机使能开关引脚          0：电机停止运行  1：电机根据输入信号正常运转
#define MOTOR_EN_STATUS_PORT    GPIOB
#define MOTOR_EN_STATUS_PIN     GPIO_Pin_14











//========PWM1 2 3通道输出使能位操作===================================== 
#define     PWMU_Enb            TIM1->CCER |= (uint16_t)(((uint16_t)TIM_CCER_CC1E)) 
#define     PWMU_Dis            TIM1->CCER &= (uint16_t)(~((uint16_t)TIM_CCER_CC1E)) 
#define     PWMV_Enb            TIM1->CCER |= (uint16_t)(((uint16_t)TIM_CCER_CC2E)) 
#define     PWMV_Dis            TIM1->CCER &= (uint16_t)(~((uint16_t)TIM_CCER_CC2E)) 
#define     PWMW_Enb            TIM1->CCER |= (uint16_t)(((uint16_t)TIM_CCER_CC3E)) 
#define     PWMW_Dis            TIM1->CCER &= (uint16_t)(~((uint16_t)TIM_CCER_CC3E))  
        
#define     PWMUVW_Enb          TIM1->CCER |= (uint16_t)(((uint16_t)(TIM_CCER_CC1E|TIM_CCER_CC2E|TIM_CCER_CC3E)))
#define     PWMUVW_Dis          TIM1->CCER &= (uint16_t)(~((uint16_t)(TIM_CCER_CC1E|TIM_CCER_CC2E|TIM_CCER_CC3E)))  
#define     PWMUVWN_Enb         TIM1->CCER |= (uint16_t)(((uint16_t)(TIM_CCER_CC1NE|TIM_CCER_CC2NE|TIM_CCER_CC3NE)))
#define     PWMUVWN_Dis         TIM1->CCER &= (uint16_t)(~((uint16_t)(TIM_CCER_CC1NE|TIM_CCER_CC2NE|TIM_CCER_CC3NE))) 

//========PWM1 2 3通道互补输出使能位操作===================================== 
#define     UL_ON               GPIO_SetBits(GPIOB,GPIO_Pin_6);
#define     VL_ON               GPIO_SetBits(GPIOB,GPIO_Pin_4);
#define     WL_ON               GPIO_SetBits(GPIOA,GPIO_Pin_15);
#define     UL_OFF              GPIO_ResetBits(GPIOB,GPIO_Pin_6);
#define     VL_OFF              GPIO_ResetBits(GPIOB,GPIO_Pin_4);
#define     WL_OFF              GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    
#define     PWMUH_ON_VL_ON		UL_OFF; WL_OFF; PWMUVW_Dis; PWMU_Enb; VL_ON;  //注意I/O操作时的输出短时间的关闭打开
#define     PWMUH_ON_WL_ON		UL_OFF; VL_OFF; PWMUVW_Dis; PWMU_Enb; WL_ON;
#define     PWMVH_ON_WL_ON  	UL_OFF; VL_OFF; PWMUVW_Dis; PWMV_Enb; WL_ON;
#define     PWMVH_ON_UL_ON   	VL_OFF; WL_OFF; PWMUVW_Dis; PWMV_Enb; UL_ON;
#define     PWMWH_ON_UL_ON   	VL_OFF; WL_OFF; PWMUVW_Dis; PWMW_Enb; UL_ON;
#define     PWMWH_ON_VL_ON   	UL_OFF; WL_OFF; PWMUVW_Dis; PWMW_Enb; VL_ON;
#define     PWMUVWH_OFF_UVWL_ON PWMUVW_Dis; UL_ON; VL_ON; WL_ON;


#define     PWMH_OFF_PWML_ON    PWMUVW_Dis; 	PWMUVWN_Enb;
#define     PWMH_ON_PWML_OFF    PWMUVWN_Dis;	PWMUVW_Enb;
#define     PWMH_OFF_PWML_OFF   PWMUVWN_Dis;	PWMUVW_Dis;





typedef enum
{
    FORWARD,    //正转
    REVERSE,    //反转
}MOTOR_DIR_enum;

typedef enum
{
    MOTOR_DISABLE,  //驱动关闭
    MOTOR_ENABLE,   //驱动使能
}MOTOR_EN_STATUS_enum;

typedef struct
{
    MOTOR_EN_STATUS_enum en_status; //指示电机使能状态
    uint8 brake_flag;   //指示当前刹车是否有效    1：正在刹车  0：正常运行    
    MOTOR_DIR_enum  dir;//电机旋转方向 FORWARD：正转  REVERSE：反转     BRAKE：刹车
    int32 set_speed;    //设置的速度
    int32 max_speed;    //速度最大值
    int32 min_speed;    //速度最小值
    int16 increase_step;//速度增加的步进值  加速的时候，值越大set_speed的值就会越快逼近except_speed
}motor_struct;


     



extern motor_struct motor_control;




void motor_speed_curve(void);
void motor_set_dir(void);
void motor_power_out(void);
void motor_commutation(uint8 except_hall);
void motor_speed_curve_init(void);


void motor_speed_out(void);
void motor_dir_out(void);
void motor_en(void);
void motor_information_out_init(void);



#endif
