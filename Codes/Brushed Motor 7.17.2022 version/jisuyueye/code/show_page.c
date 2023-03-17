#include "zf_common_headfile.h"
#include "math.h"

#include "button.h"
#include "common.h"
#include "zf_device_tft180.h"
#include "parameter.h"
#include "motor.h"
#include "flash.h"
#include "gps.h"
#define DATA_SHIFT 80
#define MAX_PAGE 8 // 最大页数

extern uint8 past_page = 0;
extern int curpage = 1;
Varible *current = NULL;
static int16 INT16_SCALE_TABLE[MAX_BIT + 1] = {1, 10, 100, 1000, 10000, 0, 0};
static int32 INT32_SCALE_TABLE[MAX_BIT + 1] = {1, 10, 100, 1000, 10000, 100000, 1000000};
static double FLOAT_SCALE_TABLE[MAX_BIT + 1] = {0.01, 0.1, 1.0, 10.0, 100.0, 1000.0, 10000.0};

uint32 f_to_uint_lati = 0;
uint32 f_to_uint_longi = 0;
int16 tp_micr_lati = 0;
int16 tp_micr_longi = 0;


void page_text_param(uint16 x, uint16 y, Varible *p, uint8 select);

#define GPS_POINTS_PAGE 4
#define GPS_LIVE_DATA 3
#define SERVO_PAGE 5
#define SPEED_PAGE 6
#define PID_PAGE (MAX_PAGE - 1)

void VarOP(Varible *v, uint8 bit, int8 op) // op: 1 add; -1 sub
{
    switch (v->type)
    {
    case TYPE_FLOAT:
        *(float *)(v->buff) = *(float *)(v->buff) + op * FLOAT_SCALE_TABLE[bit];
        break;
    case TYPE_INT16:
        *(int16 *)(v->buff) = *(int16 *)(v->buff) + op * INT16_SCALE_TABLE[bit];
        break;
    case TYPE_UINT16:
        *(uint16 *)(v->buff) = *(uint16 *)(v->buff) + op * INT16_SCALE_TABLE[bit];
        break;
    case TYPE_INT32:
        *(int32 *)(v->buff) = *(int32 *)(v->buff) + op * INT32_SCALE_TABLE[bit];
        break;
    case TYPE_UINT32:
        *(uint32 *)(v->buff) = *(uint32 *)(v->buff) + op * INT32_SCALE_TABLE[bit];
    default:
        break;
    }
}

void turn_page()
{
    if (key_num == 1)
    {
        curpage--;
        tft180_full(RGB565_WHITE);
    }
    else if (key_num == 4)
    {
        curpage++;
        tft180_full(RGB565_WHITE);
    }
}

void my_lcd_showint16(uint16 x, uint16 y, int16 dat, uint8 select, uint8 cur_bit)
{
#define INT16_DATA_LENGTH 7
    uint8 a[INT16_DATA_LENGTH];
    uint8 i;

    if (dat < 0)
    {
        tft180_show_char(x, y * 16, '-');
        dat = -dat;
    }
    else
    {
        tft180_show_char(x, y * 16, ' ');
    }

    a[0] = 0;
    a[1] = 0;
    a[2] = dat / 10000;
    a[3] = dat / 1000 % 10;
    a[4] = dat / 100 % 10;
    a[5] = dat / 10 % 10;
    a[6] = dat % 10;

    i = 0;
    while (i < INT16_DATA_LENGTH)
    {
        tft180_show_char(x + (8 * (i + 1)), y * 16, '0' + a[i]);
        i++;
    }
    tft180_show_char(x + (8 * (i + 2)), y * 16, 'Z');
}

void my_lcd_showint32(uint16 x, uint16 y, int32 dat, uint8 select, uint8 cur_bit)
{
#define INT32_DATA_LENGTH 7
    uint8 a[INT32_DATA_LENGTH];
    uint8 i;

    if (dat < 0)
    {
        tft180_show_char(x, y * 16, '-');
        dat = -dat;
    }
    else
    {
        tft180_show_char(x, y * 16, ' ');
    }

    a[0] = (dat / 1000000) % 10;
    a[1] = (dat / 100000) % 10;
    a[2] = (dat / 10000) % 10;
    a[3] = (dat / 1000) % 10;
    a[4] = (dat / 100) % 10;
    a[5] = (dat / 10) % 10;
    a[6] = dat % 10;

    i = 0;

    while (i < INT32_DATA_LENGTH)
    {
        tft180_show_char(x + (8 * (i + 1)), y * 16, '0' + a[i]);
        i++;
    }
    tft180_show_char(x + (8 * (i + 2)), y * 16, 'Z');
}

void my_lcd_showfloat(uint16 x, uint16 y, float dat, uint8 select, uint8 cur_bit)
{
#define FLOAT_DATA_LENGTH 7
    uint8 a[FLOAT_DATA_LENGTH];
    uint8 i;
    if (dat < 0)
    {
        tft180_show_char(x, y * 16, '-');
        dat = -dat;
    }
    else
    {
        tft180_show_char(x, y * 16, ' ');
    }

    a[0] = (int16)(dat / 10000) % 10;
    a[1] = (int16)(dat / 1000) % 10;
    a[2] = (int16)(dat / 100) % 10;
    a[3] = (int16)(dat / 10) % 10;
    a[4] = (int16)(dat * 1) % 10;
    a[5] = (int32)(dat * 10) % 10;
    a[6] = (int32)(dat * 100) % 10;

    i = 0;

    int write = 0;
    while (i < FLOAT_DATA_LENGTH)
    {
        if (write != 0)
        {
            tft180_show_char(x + (8 * (i + 1)), y * 16, '0' + a[i]);
        }
        else if (a[i] != 0 && write == 0)
        {
            write = 1;
            i--;
        }
        else if (i == FLOAT_DATA_LENGTH - 2)
        {
            write = 1;
        }
        i++;
    }
}

void page_text_param(uint16 x, uint16 y, Varible *p, uint8 select)
{
    tft180_show_string(x, y*16, p->name);

    switch (p->type)
    {
    case TYPE_INT16:
    case TYPE_UINT16:
    {
        int16 data = 0;
        if (p->type == TYPE_INT16)
        {
            data = *(int16 *)p->buff;
        }
        else
        {
            data = (int16) * (uint16 *)p->buff;
        }
        my_lcd_showint16(x + DATA_SHIFT, y, data, select, 0);
        break;
    }
    case TYPE_FLOAT:
    {
        float data = *(float *)(p->buff);
        my_lcd_showfloat(x + DATA_SHIFT, y, data, select, 0);
        break;
    }
    case TYPE_INT32:
    case TYPE_UINT32:
    {
        int32 data = 0;
        if (p->type == TYPE_INT32)
        {
            data = *(int32 *)p->buff;
        }
        else
        {
            data = (int32) * (uint32 *)p->buff;
        }
        my_lcd_showint32(x + DATA_SHIFT, y, data, select, 0);
        break;
    }
    default:
        break;
    }
}

void page_show_params(int16 number, Varible *vars)
{
    uint16 x = 0;
    uint16 y = 1;
    int16 index = 0;
    tft180_full(RGB565_WHITE);

    tft180_show_string(0, 0, "Name"); //一行最多20个字母
    tft180_show_string(12 * 8, 0, "Value");
    while (index < number)
    {
        page_text_param(x, y, vars + index, true);
        index = index + 1;
        y = y + 1;
    }
}



void page_spin()
{
    uint8 i = 0;
    
    static int change = 0; //判断有没有发生变化
    
    static int new_page = 0;
    key_num = Button_get();

    Button_Control();
    
    Flash_Control();
    
    //控制一些动态页面的刷新
    if (curpage==GPS_LIVE_DATA|| curpage==GPS_POINTS_PAGE)
    {
        new_page++;
        
        if (new_page > 32000)
        {
          new_page = 0;
        }
    }
    else
    {
       new_page = 2;
    }
    
    //要不要加一个变量用于页面刷新呢？？
    if (curpage != past_page || change == 1 || new_page % 5000 == 1)
    {
        change = 0;
        switch (curpage)
        {
        case 0:
        {
            // 返回最后一页
            curpage = MAX_PAGE;
            break;
        }
        //舵机PWM控制
        case 1:
        {
            static Varible vars[6] = {
                // 变量地址、变量显示名、变量类型
                //车辆模式
                 {(void *)&car_mode, "car_mode", TYPE_INT16},
                //导航模式
                {(void *)&guiding_mode, "guid_mode", TYPE_INT16},
                //gps使用模式
                 {(void *)&gps_mode, "gps_mode", TYPE_INT16},
                  {(void *)&area_tag, "area_tag", TYPE_INT16},
                  {(void *)&whether_delay, "w_delay", TYPE_INT16},
                
            };

            current = vars;
            past_page = 1;
            page_show_params(5, vars);
            break;
        }
        //编码器相关，获得左右轮距离，速度PI调节
        case 2:
        {

            static Varible vars[6] = {
                {(void *)&g_speed_set, "g_spd_set", TYPE_FLOAT},
                 //改动duty_mid时一定要注意，比赛出发前程序每个地方都要弄清楚，不留死角
                {(void *)&duty_mid, "duty_mid", TYPE_INT32},
                 {(void *)&distance_each_point_direct, "dis_e_p_d", TYPE_FLOAT},
                 {(void *)&distance_each_point_round, "dis_e_p_r", TYPE_FLOAT},
                 
                 
            };

            current = vars;
            past_page = 2;
            page_show_params(4, vars);
            break;
        }
         //duoji不要动
        case GPS_LIVE_DATA:
        {
            static Varible vars[5] = {
                {(void *)&f_to_uint_lati, "f_lati", TYPE_UINT32},
                {(void *)&f_to_uint_longi, "f_longi", TYPE_UINT32},
                //误差距离
                {(void *)&error_lenth_s_p, "lent_err", TYPE_FLOAT},
                //误差方向
                {(void *)&error_direction, "dir_err", TYPE_FLOAT},
                {(void *)&passed_sections, "pssed_sect", TYPE_INT16},
            };

            current = vars;
            past_page = GPS_LIVE_DATA;
            page_show_params(5, vars);
            break;
        }
        case GPS_POINTS_PAGE:
        {
            //观测有关当前坐标点的信息
            static Varible vars[6] = {

                {(void *)&point_selected_gps, "pnt_num", TYPE_UINT16},
                {(void *)&point_latitude_selected, "lati", TYPE_UINT32},
                {(void *)&point_longitude_selected, "lonti", TYPE_UINT32},
                {(void *)&point_type_pageread, "pnt_tprd", TYPE_INT16},
                {(void *)&finished_gpspoints, "finish_p", TYPE_UINT16},
                {(void *)&area_tag, "area_tag", TYPE_INT16},
                
            };

            current = vars;
            past_page = GPS_POINTS_PAGE;
            page_show_params(6, vars);
            break;
        }
        case SERVO_PAGE:
        {
            static Varible vars[6] = {
                {(void *)&pwm_max_angle, "Pwm_mx_ag", TYPE_UINT16},
                {(void *)&pwm_min_angle, "Pwm_mn_ag", TYPE_UINT16},
                {(void *)&pwm_mid_angle, "Pwm_md_ag", TYPE_UINT16},
                {(void *)&duoji, "duoji", TYPE_FLOAT},
                
            };

            current = vars;
            past_page = 5;
            page_show_params(4, vars);
            break;
        }
        case SPEED_PAGE:
        {
            static Varible vars[3] = {
                {(void *)&speed_max_region[0], "spd_r0_z", TYPE_FLOAT},
                {(void *)&speed_max_region[1], "spd_r1_w", TYPE_FLOAT},
                {(void *)&speed_max_region[2], "spd_r2_in", TYPE_FLOAT},
            };

            current = vars;
            past_page = 6;
            page_show_params(3, vars);
            break;
        }
        case PID_PAGE:
        {
            static Varible vars[6] = {
                 {(void *)&gps_kp_z, "KPZ", TYPE_FLOAT},
                {(void *)&gps_kd_z, "KDZ", TYPE_FLOAT},
                {(void *)&gps_kp_w, "KPW", TYPE_FLOAT},
                {(void *)&gps_kd_w, "KDW", TYPE_FLOAT},
                {(void *)&gps_kp_in, "KPI", TYPE_FLOAT},
                {(void *)&gps_kd_in, "KDI", TYPE_FLOAT},
            };

            current = vars;
            past_page = MAX_PAGE - 1;
            page_show_params(6, vars);

            break;
        }
        case MAX_PAGE:
        {
            
            // 微操与路径(记得对应的区划)
            static Varible vars[6] = {
                {(void *)&route_write, "route_w", TYPE_UINT16},
                {(void *)&micro_area, "micro_area", TYPE_INT16},
                {(void *)&tp_micr_lati, "mc_lati", TYPE_INT16},
                {(void *)&tp_micr_longi, "mc_longi", TYPE_INT16},
                 {(void *)&run_sections, "run_sec", TYPE_INT16},
                 {(void *)&direct_thres , "dir_thres", TYPE_FLOAT},
                
            };
             
            current = vars;
            past_page = MAX_PAGE;
            page_show_params(6, vars);

            break;
        }
        case MAX_PAGE + 1:
        {
            // 返回第一页
            curpage = 1;
            break;
        }
        default:
        {
            curpage = 1;
            break;
        }
        }
        for (i = 0; i < 8; i++)
            tft180_draw_point(x_cursor + i, y_cursor, RGB565_BLACK);
    }
    //限制幅度 
    if (micro_area < 0)
    {
      micro_area = 0;
    }
    else if (micro_area >= ZONE)
    {
       micro_area = ZONE -1;
    }
    
    /*************特殊页面**************/
    //读取经纬度数据  //如果在GPS页面
    if (past_page == GPS_POINTS_PAGE)
    {   
        //如果序号超出了上限
        //防止数组越界
        if (point_selected_gps >= ALL_POINTS)
        {
            point_selected_gps = ALL_POINTS;
        }
      
        //读取当前这个点的纬度
        point_latitude_selected =  (int32)(Points_Stored[point_selected_gps].lat_keypoint * 1000000);
        point_longitude_selected =  (int32)(Points_Stored[point_selected_gps].lon_keypoint * 1000000);
        
        //暂时只支持读取
        point_type_pageread =  Points_Stored[point_selected_gps].type_point;       
        
    }
    else if (past_page == GPS_LIVE_DATA)
    {
        f_to_uint_lati = Save_Data.f_lati * 1000000;
        f_to_uint_longi = Save_Data.f_longi * 1000000;
    }
    
    
 


    //如果出现了差异
    if (route_read != route_write)
    {
        //防止越界
        if (route_write < 0)
        {
            route_write = 0;
        }
        else if (route_write > 2)
        {
            route_write = 2;
        }

        route_read = route_write;
        finished_gpspoints = 0;
        
        
        FlashReadPoints();
        if (route_write == 0)
        {
            Points_Stored[0].type_point = 3;
            Points_Stored[9].lat_keypoint = ( Points_Stored[8].lat_keypoint +  Points_Stored[10].lat_keypoint)/ 2.0; 
            Points_Stored[9].lon_keypoint = ( Points_Stored[8].lon_keypoint +  Points_Stored[10].lon_keypoint)/ 2.0; 
            Points_Stored[32].lon_keypoint = 114.304626; 
            Points_Stored[33].lon_keypoint = 114.304565; 
            Points_Stored[32].lat_keypoint = (Points_Stored[31].lat_keypoint + Points_Stored[33].lat_keypoint)/2.0;
          
        }     
        
        tft180_show_string(0, 7*16, "READ");
        for (int i = 0 ; i < ALL_POINTS ; i++)
        {
            //数据有效性检验
            if (Points_Stored[i].lat_keypoint > 5.0)
            {
               finished_gpspoints++;
            }
            else
            {
               break;
            }
        }
        
        run_sections = finished_gpspoints + 1;
        
        //round_in_out();
        //再次读取
        tp_micr_lati = micro_op_lati[route_read][micro_area];
        tp_micr_longi = micro_op_longi[route_read][micro_area];
        
        
    }



    //修改模式
    if (modify_flag == 1 && current != NULL)
    {

        if (op != 0)
            change = 1;

        if (17 - x_cursor / 8 <= MAX_BIT)
            VarOP((current + y_cursor / 16 - 2), 17 - x_cursor / 8, op);

        if (y_cursor == 48 && past_page == 2) //mid部分
        {
            change_motor(duty_mid, 0);
        }
        else if (past_page == 5 && y_cursor == 80) //舵机模式
        {

            if (duoji > pwm_max_angle)
                duoji = pwm_max_angle;
            else if (duoji < pwm_min_angle)
                duoji = pwm_min_angle;

            pwm_set_duty(SERVO_PIN, duoji);
        }
        //数据写回(如果上一个微操段与现在一致，就考虑写回一次)
        else if (past_page == MAX_PAGE && last_micro_area == micro_area)
        {
            micro_op_lati[route_read][micro_area] = tp_micr_lati;
            micro_op_longi[route_read][micro_area] = tp_micr_longi;   
        }
        //数据读取
        else if (past_page == MAX_PAGE && last_micro_area != micro_area)
        {
            tp_micr_lati = micro_op_lati[route_read][micro_area];
            tp_micr_longi = micro_op_longi[route_read][micro_area];
            last_micro_area = micro_area;
        }
                                    

        modify_flag = 0;
        op = 0;

        current = NULL;
    }

    if (FLASH_WRITE_FLAG == 1 )
    {
        FlashWriteParams();
        FLASH_WRITE_FLAG = 0;
        if (write_flash_points != 5)
        {
            tft180_show_string(0, 7*16, "SAVEDNOPNT");
        }
        else
        {
            tft180_show_string(0, 7*16, "SAVEDALL");
        }
        
    }
    
  
}
