#include "zf_common_headfile.h"
#include "gps.h"
#include <stdlib.h>
void buffer_prepare(char *sdcard_write_buffer);
void data_transfer(char *sdcard_write_buffer , double data,int lenth);
// **************************** 代码区域 ****************************
char sdcard_write_buffer[SDCARD_BLOCK_SIZE];
static int8 condition_tag = 0;

#define NORMAL 0
#define UHANGED 1
#define DRIVEERR 2

    char ch = '0';
    volatile bool failedFlag = false;

    UINT bytesWritten;
    UINT bytesRead;

    FATFS fs;                                                                   // 文件系统结构体
    const TCHAR fs_drv[] = "1:/";                                               // 文件系统根目录 默认是两个部分 SD 卡在 1 部分

    FRESULT error;                                                              // 文件系统操作状态
    DIR directory;                                                              // 目录结构体变量
    static FIL g_fileObject;                                                    // 文件结构体变量

    
void sd_init()
{
  
  
    // 此处编写用户代码 例如外设初始化代码等
    static int8 hang_ok = 0;
   while(hang_ok == 0)
   {
    if( !f_mount(&fs, fs_drv ,1) && hang_ok == 0)
    {                                                                          // 挂载文件系统
         condition_tag =  UHANGED; 
         //发生错误直接弹出                                // 如果失败了就输出错误信息
    }
    else
    {
        hang_ok = 1;
    }
   }
  
  #if (FF_FS_RPATH >= 2U)                                                         // 默认两个部分
    error = f_chdrive((char const *)&fs_drv[0U]);                               // 切换目录
    if (error)
    {
         condition_tag =  DRIVEERR;                                            //驱动失败
    }
#endif
   
    error = f_mkdir("/dir_1");                                                  // 创建目录 dir_1
    if (error)
    {
        if (error == FR_EXIST)
        {
                                                                                //目录已经存在
        }
        else
        {
            condition_tag = 3;
        }
    }
}
int sd_card_write (void)
{

    
    


    char addr[40] = "";
    sprintf(addr , "/dir_1/%s.csv" , StartTime);
                                                                                      //当前目录创建文件
    error = f_open(&g_fileObject, addr, (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));   // 读写权限创建文件
    if (error)
    {
        if (error == FR_EXIST)
        {
                                                                           // 文件已存在 直接打开
        }
        else
        {
            condition_tag = 4;
            return 0; 
        }
    }
    
    f_lseek(&g_fileObject,SEEK_END);
    
    // 数据填充写入 buffer
    //准备buffer函数
    buffer_prepare(sdcard_write_buffer);
     strcat(sdcard_write_buffer,"\n");


	    error = f_write(&g_fileObject, sdcard_write_buffer, sizeof(sdcard_write_buffer), &bytesWritten);    // 写入缓冲区数据到文件中
	    if ((error) )           // 检索是否出错
	    {
            condition_tag = 5; 
            return 0; 
      
            
            }
      for(int i = 0;i < SDCARD_BLOCK_SIZE; i ++)
      {
          sdcard_write_buffer[i] = '\0';
      }
    

    if (f_close(&g_fileObject))                                                 // 关闭文件
    {
       condition_tag = 6;
       return 0; 
    }
  return 1;
}

void buffer_prepare(char *sdcard_write_buffer)
{

    data_transfer(sdcard_write_buffer , Save_Data.f_longi, 10);
    data_transfer(sdcard_write_buffer,Save_Data.f_lati,10);

    data_transfer(sdcard_write_buffer,(double)now_area,3);
    data_transfer(sdcard_write_buffer,(double)area_tag,6);
    data_transfer(sdcard_write_buffer,(double)passed_sections,6);

    data_transfer(sdcard_write_buffer,(double)Data_Gyro.angle_sum_once,6);
    data_transfer(sdcard_write_buffer,(double)Data_Gyro.angle_sum_area,6);

    data_transfer(sdcard_write_buffer,Save_Data.f_ground_direction,6);
    data_transfer(sdcard_write_buffer,Save_Data.f_ground_speed,6);
    
    data_transfer(sdcard_write_buffer,Use_Data.TwoDotsDis,6);
    data_transfer(sdcard_write_buffer, Use_Data.TwoDotsDirec,6);

    data_transfer(sdcard_write_buffer, Use_Data.angle_next_heading_now,6);
    data_transfer(sdcard_write_buffer, Use_Data.distan_next_point_now,6);
    data_transfer(sdcard_write_buffer, Use_Data.gps_angle_error_now,6);
    data_transfer(sdcard_write_buffer, Use_Data.change_angle_delta,6);

    data_transfer(sdcard_write_buffer, (double)distance_from_lastpoint,6);
    data_transfer(sdcard_write_buffer, (double)duoji,6);
    data_transfer(sdcard_write_buffer, (double)g_car_speed,6);
    data_transfer(sdcard_write_buffer, (double)duty_mid,8);
    data_transfer(sdcard_write_buffer, (double)total_time,8);
   
}

char* doubleToString(double num)
{
    char str[12];
    sprintf(str, "%lf", num);
    char* result = str;
    return result;
}

//数据转换
void data_transfer(char *sdcard_write_buffer , double data,int lenth)
{
    int p,c; 
    char *bf = doubleToString(data) ;
    strcat(bf,",");
    strcat(sdcard_write_buffer,bf);
}