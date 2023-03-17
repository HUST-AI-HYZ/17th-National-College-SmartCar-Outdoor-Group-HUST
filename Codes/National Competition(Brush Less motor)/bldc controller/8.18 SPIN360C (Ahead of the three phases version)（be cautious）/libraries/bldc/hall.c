
#include "HAL_conf.h"
#include "move_filter.h"
#include "motor.h"
#include "gpio.h"
#include "hall.h"



uint8 hall_value_now;     //当前霍尔的值
uint8 hall_value_last;    //上次霍尔的值

uint8 next_hall_value;    //下一次霍尔的值

//霍尔硬件顺序为2 3 1 5 4 6
uint8 hall_steps_normal[3][8] = 
{
    //数据中的0并没有作用，只是用于占位
    {0,3,6,2,5,1,4,0},  //正转的时候霍尔顺序
    {0,5,3,1,6,4,2,0},  //反转的时候霍尔顺序
};

uint8 hall_steps_advance[3][8] = 
{
    //数据中的0并没有作用，只是用于占位
    {0,2,4,6,1,3,5,0},  //正转的时候，超前换相霍尔顺序
    {0,4,1,5,2,6,3,0},  //反转的时候，超前换相霍尔顺序
};

uint8 hall_steps_break[8] = 
{
    //数据中的0并没有作用，只是用于占位
    0,1,2,3,4,5,6,0,  //正转的时候，超前换相霍尔顺序
};

uint8 hall_steps_advance1[2][8] = 
{
    {0,6,5,4,3,2,1,0},  
    {0,6,5,4,3,2,1,0},  
};

uint8  hall_index = 0;
uint16 commutation_time_save[6] = {COMMUTATION_TIMEOUT, COMMUTATION_TIMEOUT, COMMUTATION_TIMEOUT, COMMUTATION_TIMEOUT, COMMUTATION_TIMEOUT, COMMUTATION_TIMEOUT};  //换相时间 保存6次
uint32 commutation_time_sum = 0;    //六次换相时间总和
uint16 commutation_time = COMMUTATION_TIMEOUT;    //统计本次换相所需时间
uint32 commutation_delay = COMMUTATION_TIMEOUT;   //延迟换相延时时间
uint16 commutation_delay_ratio = 6; //换相延时时间 = commutation_delay_ratio*commutation_time_sum>>7 数值越小则换相越超前


//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取当前霍尔值
//  @param      void   
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
void read_hall_value(void)
{
	uint8 hall_a;
	uint8 hall_b;
	uint8 hall_c;
	
	hall_a = GPIO_ReadInputDataBit(HALL_PORT, HALL_A_PIN);
	hall_b = GPIO_ReadInputDataBit(HALL_PORT, HALL_B_PIN);
	hall_c = GPIO_ReadInputDataBit(HALL_PORT, HALL_C_PIN);
	hall_value_now = hall_a*4 + hall_b*2 + hall_c; 
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      计算当前的速度
//  @param      void   
//  @return     void					
//  @since      
//-------------------------------------------------------------------------------------------------------------------
inline void calc_speed(void)
{
    //转速计算
    uint8 i;
    int32 speed;
    commutation_time_sum = 0;
    for(i=0;i<6;i++)
    {
        commutation_time_sum += commutation_time_save[i];
    }
    
    //电机转速计算说明
    //2.commutation_time_sum是统计电机换相6次会进入多少次ADC中断
        //2.1 由于ADC是定时器1的PWM周期触发的，因为ADC中断频率与PWM周期是一样的频率40khz
        //2.2 函数调用关系ADC1_IRQHandler->scan_hall_status->calc_speed
        //2.3 commutation_time为统计换相时间的变量
    //3.通常电机转速我们都用RPM表示，RPM表示每分钟电机的转速
    //3.电机转一圈需要换相的次数等于 电机极对数*6
    //4.因此电机转速等于60*ADC中断频率/电机极对数/commutation_time_sum，这样可以得到电机每分钟的转速
    
    speed = ADC_NUM_PM/POLEPAIRS/commutation_time_sum;
    
    if(REVERSE == motor_control.dir)//电机反转的时候需要对速度取反
    {
        speed = -speed;
    }
    move_filter_calc(&speed_filter, speed);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      检查霍尔值
//  @param      void   
//  @return     void					
//  @since      检查霍尔值、调用速度计算函数、得出下次期望的霍尔值
//-------------------------------------------------------------------------------------------------------------------
void scan_hall_status(void)
{
	read_hall_value();
	
    commutation_time++;

    //换相超时保护
    //如果换相超过了250ms则认为出现堵转，应该及时将速度设置为0
    if(commutation_time >= COMMUTATION_TIMEOUT)		
	{
		commutation_time = COMMUTATION_TIMEOUT;
		commutation_time_save[0] = COMMUTATION_TIMEOUT;
		commutation_time_save[1] = COMMUTATION_TIMEOUT;
		commutation_time_save[2] = COMMUTATION_TIMEOUT;
		commutation_time_save[3] = COMMUTATION_TIMEOUT;
		commutation_time_save[4] = COMMUTATION_TIMEOUT;
		commutation_time_save[5] = COMMUTATION_TIMEOUT;
		
        //滑动平均滤波初始化
        move_filter_init(&speed_filter);

        //输出的速度清零

	}
    
    if( (hall_value_now != hall_value_last) &&
        !commutation_delay)
    {
        
        hall_index++;
        if(hall_index >= 6)
        {
            hall_index = 0;
        }
        commutation_time_save[hall_index] = commutation_time;
        commutation_time = 0;
        
        //转子速度
        calc_speed();
        
        //每次完成换相，速度输出引脚翻转一次电平状态
        motor_speed_out();
        
				//如果速度够快
				if((speed_filter.data_average > 16000) || (speed_filter.data_average < -16000))
        {
            next_hall_value = hall_steps_advance1[motor_control.dir][hall_value_now];        
        }
        else if((speed_filter.data_average > 4000) || (speed_filter.data_average < -4000))
        {
            //速度大于一定的时候，霍尔延迟较大
            //因此采用超前换相加延时的方式去匹配最佳的换相点
            next_hall_value = hall_steps_advance[motor_control.dir][hall_value_now];
            commutation_delay = (commutation_delay_ratio*commutation_time_sum)>>7;
        }
        else
        {
            //速度较低无需超前换相
            next_hall_value = hall_steps_normal[motor_control.dir][hall_value_now];
            commutation_delay = 0;
        }
        
        hall_value_last = hall_value_now;
	}
}

void hall_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
    
    GPIO_InitStructure.GPIO_Pin  =  HALL_A_PIN | HALL_B_PIN | HALL_C_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(HALL_PORT, &GPIO_InitStructure);
    
    //读取一下当前的霍尔值
    read_hall_value();
}

