#include <lpc21xx.h>                            /* LPC21xx definitions */
#include <deb_led.h>
#include <system_def.h>	
#include <can_ids.h>
#include <periph/can.h>
#include <string.h>
#include <deb_led.h>
#include "engine.h"	
#include "uar.h"
#include "def.h"
#include "vhn.h"


void vhn_init(void)
{
	init_engine_B();
	PINSEL2 &= ~(3<<2);
	IO1DIR |= (VHN_EN_A | VHN_EN_B | VHN_IN_A | VHN_IN_B);
	IO1SET = (VHN_EN_A | VHN_EN_B | VHN_IN_A | VHN_IN_B);
}

void vhn_speed(uint8_t speed, uint8_t dir) {
  
    if(speed == 0)
    {
      IO1CLR = (VHN_IN_A | VHN_IN_B);
      return;
    }
  
    if(dir == ENGINE_DIR_FW)
    {
	IO1SET = VHN_IN_A;
	IO1CLR = VHN_IN_B;
    }
    else
    {
	IO1CLR = VHN_IN_A;
	IO1SET = VHN_IN_B;
    }
    
    //IO1CLR = (VHN_EN_A | VHN_EN_A);
    IO1SET = (VHN_EN_A | VHN_EN_A);
    engine_B_pwm(100-speed);
      
}

void vhn_reset()
{
	volatile int i = 10000;
	deb_led_on(LEDR);
	IO1CLR = VHN_IN_A|VHN_IN_B;
	while(i--);
	//while (IO1PIN & (VHN_EN_A | VHN_EN_B) != (VHN_EN_A | VHN_EN_B));
	IO1SET = VHN_IN_A|VHN_IN_B;
	deb_led_off(LEDR);
}
