#include "led.h"
#include "zf_common_headfile.h"




void led_init()
{
  gpio_init(LED0, GPO, GPIO_HIGH, GPO_PUSH_PULL);
  gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);
  gpio_init(LED2, GPO, GPIO_HIGH, GPO_PUSH_PULL);
  gpio_init(LED3, GPO, GPIO_HIGH, GPO_PUSH_PULL);
  
  gpio_set(LED0 , 0);
  gpio_set(LED1 , 0);
  gpio_set(LED2 , 0);
  gpio_set(LED3 , 0);
 // systick_delay_ms(300);
  
  //gpio_set(LED0 , 0);
 // gpio_set(LED1 , 0);
 // gpio_set(LED2 , 0);
 // gpio_set(LED3 , 0);
  
}