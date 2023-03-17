#ifndef _CMMN_H
#define _CMMN_H
#include "zf_common_headfile.h"
#include "common.h"

#define TYPE_INT16  1
#define TYPE_UINT16 2
#define TYPE_INT32  3
#define TYPE_UINT32 4
#define TYPE_FLOAT  5
#define MAX_BIT 6

#define LINE_LEFT       0
#define LINE_RIGHT      1


extern uint16 x_cursor;
extern uint16 y_cursor;
extern uint8 cur_page;
extern int8 modify_flag;
extern int8 op;
extern int curpage;
extern uint32 distance_time;          //测距的时间变量
extern uint32 total_time;

/*
#define FLASH_PAGE_READ(buf)	flash_read_page(FLASH_SECTION, FLASH_PAGE, buf, FLASH_LENGTH)
#define FLASH_PAGE_READ(buf)	tft180_full(FLASH_SECTION, FLASH_PAGE, buf, FLASH_LENGTH)
*/
typedef struct
{
    void* buff;
    uint8* name;
    uint8 type;
}Varible;

#endif
