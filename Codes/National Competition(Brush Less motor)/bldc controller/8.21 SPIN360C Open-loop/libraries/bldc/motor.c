/*-------------------- Includes -----------------------*/
#include "HAL_conf.h"
#include "hall.h"
#include "adc.h"
#include "timer.h"
#include "gpio.h"
#include "move_filter.h"
#include "pwm_input.h"
#include "bldc_config.h"
#include "motor.h"


motor_struct motor_control;


//-------------------------------------------------------------------------------------------------------------------
//  @brief      速度曲线计算函数
//  @param      void
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_speed_curve(void)
{
    if(FORWARD == motor_control.dir)
    {   //设置的速度比闭环的目标速度大，则使用步进值将闭环目标速度逐步逼近设置速度
        if(motor_control.set_speed > closed_loop.target_speed)
        {
            closed_loop.target_speed += motor_control.increase_step;
            if(closed_loop.target_speed > motor_control.set_speed)
            {
                closed_loop.target_speed = motor_control.set_speed;
            }
        }
        //设置的速度比目标速度低，则不使用步进值
        else
        {
            closed_loop.target_speed = motor_control.set_speed;
            if(0 > closed_loop.target_speed) 
            {
                closed_loop.target_speed = 0;
            }
        }
    }
    else//反转
    {   //设置的速度比闭环的目标速度大，则使用步进值将闭环目标速度逐步逼近设置速度
        if(motor_control.set_speed < closed_loop.target_speed)
        {
            closed_loop.target_speed -= motor_control.increase_step;
            if(closed_loop.target_speed < motor_control.set_speed)
            {
                closed_loop.target_speed = motor_control.set_speed;
            }
        }
        //设置的速度比目标速度低，则不使用步进值
        else
        {
            closed_loop.target_speed = motor_control.set_speed;
            if(0 < closed_loop.target_speed) 
            {
                closed_loop.target_speed = 0;
            }
        }
    }
    
    
    //限幅
    if(closed_loop.target_speed > motor_control.max_speed)
    {
        closed_loop.target_speed = motor_control.max_speed;
    }
    if(closed_loop.target_speed < motor_control.min_speed)
    {
        closed_loop.target_speed = motor_control.min_speed;
    }
}


void motor_set_dir(void)
{
    //当速度为0的时候才检测是否需要切换方向
    if(speed_filter.data_average == 0)
    {
        if(!pwm_dir_get())
        {
            motor_control.dir = FORWARD;
        }
        else
        {
            motor_control.dir = REVERSE;
        }
    }
    
    //输出电机实际运行的方向信息
    motor_dir_out();
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      刹车
//  @param      void
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_brake(void)
{
    PWMUVWH_OFF_UVWL_ON;//如果刹车使能则下桥全开，上桥全关
    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      更新占空比
//  @param      void
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_duty_out(uint16 duty)
{
    TIM1->CCR1 = duty;
    TIM1->CCR2 = duty;
    TIM1->CCR3 = duty;
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      输出动力
//  @param      void
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_power_out(void)
{
    int16 duty;
    duty = closed_loop.out;

    if(motor_control.dir == FORWARD)
    {//电机正转
        if(0 > duty)
        {
            duty = 0;
        }
    }
    else
    {//电机反转
        if(0 >= duty)
        {
            duty = -duty;
        }
        else
        {
            duty = 0;
        }
    }
    
    
    if(0 == closed_loop.target_speed && motor_control.brake_flag)
    {//当刹车标志位开启的时候并且设置的速度为0 此时需要进行刹车
        motor_brake();
    }
    else if(MOTOR_DISABLE == motor_control.en_status)
    {//当电机使能开关关闭的时候需要进行刹车
        motor_brake();
    }
    else
    {
        motor_duty_out(duty);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机换相函数
//  @param      except_hall 期望下次霍尔的值
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_commutation(uint8 except_hall)
{
    if(0 == closed_loop.target_speed && motor_control.brake_flag)
    {
        motor_brake();
    }
    else if(MOTOR_DISABLE == motor_control.en_status)
    {
        motor_brake();
    }
    else
    {
        switch(except_hall)
        {
            case 1:
                PWMVH_ON_WL_ON//1
                break;
            
            case 2:		
                PWMUH_ON_VL_ON//2
                break;
            
            case 3:		
                PWMUH_ON_WL_ON//3
                break;
            
            case 4:		
                PWMWH_ON_UL_ON//4
                break;
            
            case 5:		
                PWMVH_ON_UL_ON//5
                break;
            
            case 6:		
                PWMWH_ON_VL_ON//6
                break;

            default:
                PWMH_OFF_PWML_OFF
                break;
        }
    }
    
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机速度输出
//  @param      void 
//  @return     void					
//  @since      每次换相的时候翻转IO，外部控制器用编码器接口直接采集即可
//              速度引脚连接外部控制器编码器接口的A通道 方向引脚连接B通道
//-------------------------------------------------------------------------------------------------------------------
void motor_speed_out(void)
{
    static uint8 speed_temp;
    speed_temp = !speed_temp;
    GPIO_WriteBit(MOTOR_SPEED_OUT_PORT, MOTOR_SPEED_OUT_PIN, (BitAction)speed_temp);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机转动方向输出
//  @param      void 
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_dir_out(void)
{
    GPIO_WriteBit(MOTOR_DIR_OUT_PORT, MOTOR_DIR_OUT_PIN, (BitAction)motor_control.dir);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机使能状态查询
//  @param      void 
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_en(void)
{
    if(GPIO_ReadInputDataBit(MOTOR_EN_STATUS_PORT, MOTOR_EN_STATUS_PIN))
    {
        motor_control.en_status = MOTOR_DISABLE;
    }
    else
    {
        motor_control.en_status = MOTOR_ENABLE;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机速度与方向输出 初始化
//  @param      void 
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_information_out_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOD, ENABLE); 
    
    GPIO_InitStructure.GPIO_Pin  =  MOTOR_SPEED_OUT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MOTOR_SPEED_OUT_PORT, &GPIO_InitStructure); 
    
    GPIO_InitStructure.GPIO_Pin  =  MOTOR_DIR_OUT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MOTOR_DIR_OUT_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  =  MOTOR_EN_STATUS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(MOTOR_EN_STATUS_PORT, &GPIO_InitStructure);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机速度曲线初始化
//  @param      void 
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void motor_speed_curve_init(void)
{
    #if BLDC_BRAKE_ENABLE==1
        motor_control.brake_flag = 1;   //刹车使能
    #else
        motor_control.brake_flag = 0;   //刹车关闭
    #endif
    motor_control.dir = FORWARD;                    //设置默认的方向
    
    motor_control.set_speed = 0;
    #if(BLDC_CLOSE_LOOP_ENABLE)
    motor_control.max_speed = BLDC_MAX_SPEED;       //设置最大正转速度
    motor_control.min_speed = -BLDC_MAX_SPEED;      //设置最大反转速度
    motor_control.increase_step = BLDC_STEP_SPEED;  //设置加速时的步进值
    #endif
}


