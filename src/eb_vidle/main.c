
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
#include <adc.h>


#define ADC_ISR		1

#define TIMER_IRQ_PRIORITY	5

#define SAMPLES		500
#define ADC_PERIOD	10 // ms

#define START_PIN	15		// start pin

unsigned int counter;
unsigned int data[SAMPLES];

bool start_measuring = false;


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
} 


//void fsm_vidle_init(struct fsm *fsm, enum event event);

void start_button(void)
{
	if((IO0PIN & (1<<START_PIN)) == 0)
	{
		start_measuring = true;
		lcd_gotoxy(0,1);
		lcd_print("Measurement    ");
	}
}

void blink_led()
{
	static uint32_t led_time = 0;
	
	if(timer_msec >= led_time + 500)	
	{
		led_time = timer_msec;
		deb_led_change(LEDG);
	}
}

void rotate_cursor()
{
	static uint32_t time = 0;
	
	static unsigned char cnt = 0;
	
	unsigned char cursor[4] = {'|','/','-','\\'};
	
	if(timer_msec >= time + 200)	
	{
		time = timer_msec;
		lcd_gotoxy(15,1);
		lcd_putchar(cursor[cnt++]);
		if(cnt == 3) cnt = 0;
	}
}

void save_adc()
{
	static uint32_t adc_time = 0;
	
	rotate_cursor();
	
	if(timer_msec >= adc_time + ADC_PERIOD)	
	{
		adc_time = timer_msec;
		data[counter] = adc_val[0];
		counter++;
		deb_led_change(LEDY);
	}
}

void send_message()
{
	unsigned int i = 0;
	deb_led_on(LEDB);
	
	lcd_gotoxy(0,1);
	lcd_print("Data transfer  ");
	
	while(i < SAMPLES)
	{
		blink_led();
		rotate_cursor();
		send_rs_int(i);
		send_rs_str(" ");
		send_rs_int(data[i]);
		send_rs_str("\n");
		i++;
	}
	deb_led_off(LEDB);
	lcd_gotoxy(0,1);
	lcd_print("Ready           ");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(void)
{
	counter = 0;
	
	init_periphery();
	
	SET_PIN(PINSEL0, START_PIN, PINSEL_0);		// inicializace start pinu
	IO0DIR &= ~(1<<START_PIN);
	
	lcd_init();
	lcd_print("ADC scope");
	lcd_gotoxy(0,1);
	lcd_print("Ready");
	
	while(1){
		if(start_measuring)
		{
			if(counter < SAMPLES)
			{
				save_adc();
			} else {
				counter = 0;
				start_measuring = false;
				send_message();
			}
		}
		start_button();
		blink_led();
	}
}

/** @} */
