#ifndef _FLASH_H
#define _FLASH_H

#include "common.h"
#include "parameter.h"

#include "gps.h"
extern int8 FLASH_WRITE_FLAG;
extern int write_flash_points;
// 搴曞眰鍑芥暟
#define FLASH_PAGE_READ(buf)	flash_read_page(FLASH_SECTION, FLASH_PAGE, buf, FLASH_LENGTH)
#define FLASH_PAGE_WRITE(buf)	flash_write_page(FLASH_SECTION, FLASH_PAGE, buf, FLASH_LENGTH)

extern float _PLACE_HOLDER_; // 鍗犱綅绗︼紝鐢ㄤ簬
 //主要是点，怎么保存呢？？ 
// Flash鍙橀噺鍒楄〃
#define FLASH_VAR_NUM	23  // 鍙傛暟涓?鏁? // 鏈�澶? 256 涓?鍙橀噺 // 256 * 4 Byte == 1KB
#define FLASH_VAR_LIST	{\
        {(void *)&gps_kp_z, (uint8 *)NULL, TYPE_FLOAT},\
	{(void *)&gps_kd_z, (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&gps_kp_w, (uint8 *)NULL, TYPE_FLOAT},\
	{(void *)&gps_kd_w, (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&gps_kp_in, (uint8 *)NULL, TYPE_FLOAT},\
	{(void *)&gps_kd_in, (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&pwm_max_angle, (uint8 *)NULL, TYPE_UINT16},\
        {(void *)&pwm_min_angle, (uint8 *)NULL, TYPE_UINT16},\
        {(void *)&pwm_mid_angle, (uint8 *)NULL, TYPE_UINT16},\
        {(void *)&g_speed_set, (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&finished_gpspoints, (uint8 *)NULL, TYPE_UINT16},\
        {(void *)&distance_each_point_direct, (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&speed_max_region[0], (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&speed_max_region[1], (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&speed_max_region[2], (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&run_sections, (uint8 *)NULL, TYPE_INT16},\
        {(void *)&distance_each_point_round, (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&direct_thres, (uint8 *)NULL, TYPE_FLOAT},\
        {(void *)&whether_delay, (uint8 *)NULL, TYPE_INT16},\
         \
}


 // 空格要清除
//会不会存在数据挤占？

// Flash鍦板潃閰嶇疆
// 0x0807F000 鍦↖AR涓?闇�瑕侀厤缃?锛岃?剧疆鐑у啓鏃舵?ゅ尯鍩熶繚鎶や笉琚?瑕嗙洊
#define FLASH_SECTION	FLASH_SECTION_126
#define FLASH_PAGE		FLASH_PAGE_0
#define FLASH_LENGTH	(FLASH_VAR_NUM)

void FlashReadParams();
void FlashWriteParams();

#endif