
#include "motor.h"
#include "move_filter.h"

move_filter_struct speed_filter;


//-------------------------------------------------------------------------------------------------------------------
//  @brief      滑动平均滤波计算
//  @param      void   
//  @return     void					
//  @since      主要用于对数据滤波，存储目标数据最近的n个数据，并求出平均值
//-------------------------------------------------------------------------------------------------------------------
void move_filter_calc(move_filter_struct *move_filter, int32 new_data)
{
    //加上新的数值 减去最末尾的数值 求得最新的和
    speed_filter.data_sum = speed_filter.data_sum + new_data - speed_filter.data_buffer[speed_filter.index];
    //重新求平均值
    speed_filter.data_average = speed_filter.data_sum / speed_filter.buffer_size;
    
    //将数据写入缓冲区
    speed_filter.data_buffer[speed_filter.index] = new_data;
    speed_filter.index++;
    if(speed_filter.buffer_size <= speed_filter.index)
    {
        speed_filter.index = 0;
    }
}



//-------------------------------------------------------------------------------------------------------------------
//  @brief      滑动平均滤波初始化
//  @param      void   
//  @return     void					
//  @since      主要用于对数据滤波，存储目标数据最近的n个数据，并求出平均值
//-------------------------------------------------------------------------------------------------------------------
void move_filter_init(move_filter_struct *move_filter)
{
    speed_filter.data_average = 0;
    speed_filter.data_sum = 0;
    speed_filter.index = 0;
    //设置缓冲区大小
    speed_filter.buffer_size = MOVE_AVERAGE_SIZE;  
    
    uint8 i;
    for(i=0; i < speed_filter.buffer_size; i++)
	{
		speed_filter.data_buffer[i] = 0;
	}
}






