#ifndef __ADC_H_
#define __ADC_H_


#include "common.h"

#define CHANNEL_DISABLE				    ((uint32_t)0xFFFF0000)

typedef struct
{ 
	uint16 voltage_bus;
	uint16 current_bus;
    uint16 current_bus_filter;
    uint16 current_a;
    uint16 current_b;
}adc_struct; 	

extern adc_struct adc_information; 			

void adc_init(void);
void adc_read(void);

#endif
