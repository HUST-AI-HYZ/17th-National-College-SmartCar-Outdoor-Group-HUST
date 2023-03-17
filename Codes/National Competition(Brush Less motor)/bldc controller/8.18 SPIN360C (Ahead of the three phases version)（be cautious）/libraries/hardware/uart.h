#ifndef _uart_h
#define _uart_h



#include "common.h"


void uart_init(uint32 baud);
void write_byte(uint8 data);
void write_str(int8 *str);
void write_buffer(uint8 *buffer, uint32 length);





#endif
