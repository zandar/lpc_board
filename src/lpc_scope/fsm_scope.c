#include <lpc21xx.h>
#include <deb_led.h>
#include <system_def.h>	
#include <string.h>
#include "fsm.h"
#include <stdbool.h>
#include "def.h"
#include <adc.h>
#include "lcd.h"
#include <uar.h>
#include <types.h>

static void wait_for_cmd(struct fsm *fsm, enum event event);
static void scope_capture_data(struct fsm *fsm, enum event event);
static void scope_send_data(struct fsm *fsm, enum event event);

void fsm_scope_init(struct fsm *fsm, enum event event)
{
	switch (event) {
	case EVENT_ENTRY:
		lcd_print("LPC-2119 scope");
		lcd_gotoxy(0,1);
		lcd_print("Ready");
		break;
	case EVENT_DO:
		fsm->adc_start = false;
		fsm->send_samples = false;
		fsm->scope_ready = true;
		fsm->current_state = wait_for_cmd;
		break;
	case EVENT_EXIT:
		break;
	}
}

static void wait_for_cmd(struct fsm *fsm, enum event event)
{
	switch (event) {
	case EVENT_ENTRY:
		break;
	case EVENT_DO:
		/* waiting to start adc sampling */
		if(fsm->adc_start)
			 fsm->current_state = scope_capture_data;
		break;
	case EVENT_EXIT:
		fsm->scope_ready = false;
		break;
	}
}

void rotate_cursor()
{
	static uint32_t time = 0;
	
	static unsigned char counter = 0;
	
	unsigned char cursor[4] = {'|','/','-','\\'};
	
	if(timer_msec >= time + 200)
	{
		time = timer_msec;
		lcd_gotoxy(15,1);
		lcd_putchar(cursor[counter++]);
		if(counter == 3) counter = 0;
	}
}

static void save_samples(struct fsm *fsm)
{
	static uint32_t adc_time = 0;
	static uint16_t time_base = 0;
	static uint16_t counter = 0;
	
	rotate_cursor();
	
	if(timer_usec >= adc_time + ADC_PERIOD)		// save ADC sample every ADC_PERIOD us
	{
		if(counter == 0)	// when first sample, time = 0
		  time_base = 0;
		else
		  time_base = timer_usec - adc_time;	// else time = time betwen samples
		
		adc_time = timer_usec;
		
		fsm->adc_data[ADC_DATA][counter] = adc_val[0];
		fsm->adc_data[ADC_TIME][counter] = time_base;
		counter++;
		
		if(counter == ADC_SAMPLES)
		{
			counter = 0;
			fsm->send_samples = true;
		}
	}
}

static void scope_capture_data(struct fsm *fsm, enum event event)
{
	switch (event) {
	case EVENT_ENTRY:
		deb_led_on(LEDY);
		lcd_gotoxy(0,1);
		lcd_print("Measurement    ");
		break;
	case EVENT_DO:
		save_samples(fsm);
		if(fsm->send_samples)
			fsm->current_state = scope_send_data;
		break;
	case EVENT_EXIT:
		deb_led_off(LEDY);
		fsm->adc_start = false;
		break;
	}
}

static void send_samples(struct fsm *fsm)
{
	static uint32_t rs_time = 0;
	
	static uint16_t counter = 0;
	
	if(timer_usec >= rs_time + 1000)
	{
		deb_led_on(LEDB);
		rs_time = timer_usec;
		rotate_cursor();
		send_rs_int(fsm->adc_data[0][counter]);
		send_rs_str(" ");
		send_rs_int(fsm->adc_data[1][counter]);
		send_rs_str("\n");
		counter++;
		
		if(counter == ADC_SAMPLES)
		{
			counter = 0;
			fsm->scope_ready = true;
		}
		deb_led_off(LEDB);
	}
}

static void scope_send_data(struct fsm *fsm, enum event event)
{
	switch (event) {
	case EVENT_ENTRY:
		lcd_gotoxy(0,1);
		lcd_print("Data transfer  ");
		break;
	case EVENT_DO:
		send_samples(fsm);
		if(fsm->scope_ready)
			fsm->current_state = wait_for_cmd;
		break;
	case EVENT_EXIT:
		lcd_gotoxy(0,1);
		lcd_print("Ready           ");
		fsm->send_samples = false;
		break;
	}
}