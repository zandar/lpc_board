
/**
 * @file main.c
 * 
 *
 *
 * @addtogroup fork
 */


/**
 * @defgroup fork Vidle (fork) application
 */
/**
 * @ingroup fork
 * @{
 */


/**
  Aplication for measuring signal on ADC 0 input and send samples to UART
  
  signal input		conector J16	pin 14
  start measuring	conector J17	pin 18
  
  number of samples	SAMPLES
  measuring period	ADC_PERIOD

*/

#include <lpc21xx.h>                            /* LPC21xx definitions */
#include <types.h>
#include <system_def.h>	
//#include <string.h>
#include <deb_led.h>	
#include "uar.h"
#include <stdbool.h>
#include "def.h"
#include "lcd.h"
#include "fsm.h"
#include <adc.h>


#define ADC_ISR		1

#define TIMER_IRQ_PRIORITY	5

#define START_PIN	15		// start pin

//unsigned int counter;
//unsigned int data[SAMPLES];

//bool start_measuring = false;

struct fsm fsm_scope;

void fsm_scope_init(struct fsm *fsm, enum event event);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void set_irq_handler(uint8_t source, uint8_t irq_vect, void (*handler)()) {
	/* set interrupt vector */
	((uint32_t*)&VICVectAddr0)[irq_vect] = (uint32_t)handler;
	((uint32_t*)&VICVectCntl0)[irq_vect] = 0x20 | source;
	/* enable interrupt */
	VICIntEnable = 1<<source;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** timer0 & ISR **/

void timer0_irq() __attribute__((interrupt));
volatile uint32_t timer_msec = 0, timer_usec = 0;

void init_timer0(uint32_t prescale, uint32_t period) {
	T0TCR = 0x2; /* reset */
	T0PR = prescale - 1;
	T0MR0 = period;
	T0MCR = 0x3; /* match0: reset & irq */
	T0EMR = 0; /* no ext. match */
	T0CCR = 0x0; /* no capture */
	T0TCR = 0x1; /* go! */
}

void timer0_irq() {
	static unsigned cnt1khz = 0;
	
	/* reset timer irq */
	T0IR = -1;

	/* increment timer_usec */
	timer_usec += 10;
	/* increment msec @1kHz */
	if (++cnt1khz == 100) {
		cnt1khz = 0;
		++timer_msec;
	}
	
	/* int acknowledge */
	VICVectAddr = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void init_periphery(void){

	/* init timer0 */
	init_timer0(1, CPU_APB_HZ/100000);
	set_irq_handler(4 /*timer0*/, TIMER_IRQ_PRIORITY, timer0_irq);

	init_uart();
	init_adc(ADC_ISR);
	lcd_init();
} 

void start_button(void)
{
	if((IO0PIN & (1<<START_PIN)) == 0)
	{
		fsm_scope.adc_start = true;
	}
}

void blink_status_led()
{
	static uint32_t led_time = 0;
	
	if(timer_msec >= led_time + 500)	
	{
		led_time = timer_msec;
		deb_led_change(LEDG);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(void)
{
	uint32_t main_time = timer_usec;
	
	//counter = 0;
	
	init_periphery();
	
	SET_PIN(PINSEL0, START_PIN, PINSEL_0);		// inicializace start pinu
	IO0DIR &= ~(1<<START_PIN);
	
	init_fsm(&fsm_scope, &fsm_scope_init);
	
	while(1){
		if(timer_usec >= main_time + 100)
		{
			main_time = timer_usec;
			run_fsm(&fsm_scope);
		}
		start_button();
		blink_status_led();
	}
}

/** @} */
