
/**
 * @file main.c
 * 
 *
 * @author Bc. Jiri Kubias, jiri.kubias@gmail.com
 * @author Michal Sojka <sojkam1@fel.cvut.cz>
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


#include <lpc21xx.h>                            /* LPC21xx definitions */
#include <types.h>
#include <deb_led.h>
#include <system_def.h>	
#include <can_ids.h>
#include <periph/can.h>
#include <string.h>
#include <deb_led.h>
#include "engine.h"	
#include "uar.h"
#include <can_msg_def.h>
#include "fsm.h"
#include "def.h"
#include <adc.h>
#include "vhn.h"

#define	CAN_SPEED 	1000000 	//< CAN bus speed
#define CAN_ISR		0

#define ADC_ISR		1

#define TIMER_IRQ_PRIORITY	5


struct fsm fsm_vidle;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




void init_motors(void){
  
  	init_engine_A();			// initialization of PWM unit
	engine_A_en(ENGINE_EN_ON);		//enable motor A
	engine_A_dir(ENGINE_DIR_FW);		//set direction 
	engine_A_pwm(0);			// STOP pwm is in percent, range 0~100~200
/* 	vhn_init(); */
}

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

#define START_PIN	15		// start pin
#define START_SEND_PRIOD_FAST	50	/* [miliseconds] */
#define START_SEND_PRIOD_SLOW	300	/* [miliseconds] */
#define START_SEND_FAST_COUNT	10	/* How many times to send start with small period (after a change) */


void start_button(void)
{
	can_msg_t msg;
	bool start_condition;
	static bool last_start_condition = 0;

	static int count = 0;
	static uint32_t next_send = 0;

	
	start_condition = (IO0PIN & (1<<START_PIN)) == 0;

	if (start_condition != last_start_condition) {
		last_start_condition = start_condition;
		count = 0;
		next_send = timer_msec; /* Send now */
	}

	if (timer_msec >= next_send) {
		msg.id = CAN_ROBOT_CMD;
		msg.flags = 0;
		msg.dlc = 1;
		msg.data[0] = start_condition;
		/*while*/ (can_tx_msg(&msg));

		if (count < START_SEND_FAST_COUNT) {
			count++;
			next_send = timer_msec + START_SEND_PRIOD_FAST;
		} else
			next_send = timer_msec + START_SEND_PRIOD_SLOW;
	}

		
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CAN_rx(can_msg_t *msg) {
	uint32_t spd;
	can_msg_t rx_msg;
	uint32_t req =0;
	memcpy(&rx_msg, msg, sizeof(can_msg_t));//make copy of message
	
	
	deb_led_on(LEDB);

	switch (rx_msg.id) 
	{		
		case CAN_VIDLE_CMD:
			deb_led_on(LEDB);
			req = ((rx_msg.data[0]<<8) | (rx_msg.data[1]));
			spd = rx_msg.data[2];
			
			if (req >= 0x150 && req <= 0x3e0) {
				fsm_vidle.flags &= ~CAN_VIDLE_OUT_OF_BOUNDS;
				fsm_vidle.can_req_position = req;// save new req position of lift
				fsm_vidle.can_req_spd = spd;// save new req spd of lift
			} else
				fsm_vidle.flags |= CAN_VIDLE_OUT_OF_BOUNDS;
		break;
		default:break;
	}

	deb_led_off(LEDB);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void init_periphery(void){
  
	can_init_baudrate(CAN_SPEED, CAN_ISR, CAN_rx);//initialization of CAN bus	
	init_motors();

	/* init timer0 */
	init_timer0(1, CPU_APB_HZ/100000);
	set_irq_handler(4 /*timer0*/, TIMER_IRQ_PRIORITY, timer0_irq);

	init_uart();
	init_adc(ADC_ISR);
	
	
} 
/*********************************************************/
void can_send_status(void)
{
	can_msg_t tx_msg;
	tx_msg.id = CAN_VIDLE_STATUS;
	tx_msg.dlc = 5;
	tx_msg.flags = 0;
	tx_msg.data[0] = (fsm_vidle.act_pos  >> 8) & 0xFF;
	tx_msg.data[1] = fsm_vidle.act_pos & 0xFF;
	tx_msg.data[2] = (fsm_vidle.can_response  >> 8) & 0xFF;
	tx_msg.data[3] = fsm_vidle.can_response & 0xFF;
	tx_msg.data[4] = fsm_vidle.flags; 
	/*while*/(can_tx_msg(&tx_msg)); /* CAN erratum workaround */
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void dbg_print_time()
{
	char str[10];
	unsigned t = timer_usec, i;
	memset(str, ' ', sizeof(str));
	str[9] = 0;
	str[8] = '\n';
	for (i=7; t > 0; i--) {
		str[i] = t%10 + '0';
		t /= 10;
	}
	send_rs_str(str);
}

void fsm_vidle_init(struct fsm *fsm, enum event event);


void blink_led()
{
	static uint32_t led_time = 0;
	
	if(timer_msec >= led_time + 500)	
	{
		led_time = timer_msec;
		/*  static int up;
		    if (up == 0)
		    fsm_vidle.can_req_position = 0x380;
		    if (up == 6)
		    fsm_vidle.can_req_position = 0x1e0;
		    up = (up+1)%12;
		*/
		deb_led_change(LEDG);
		send_rs_int(fsm_vidle.act_pos);
		send_rs_str("\n");
	}
}


#define BUMPER_PIN	17		// bumper pin  (SCK1/P0_17)
#define COLOR_PIN	3		// change color of dress pin  (SDA1/P0_3)

#define BUMPER_LEFT 19 	// left bumper MOSI1/P0_19
#define BUMPER_RIGHT 9		// right bumper RXD1/P0_9	

void handle_bumper()
{
	static uint32_t bumper_time = 0;
	char sw = 0;

	if (timer_msec >= bumper_time + 100)	
	{
		can_msg_t tx_msg;

		bumper_time = timer_msec;
		
			
			
		if (IO0PIN & (1<<BUMPER_PIN))
			sw &= ~CAN_SWITCH_BUMPER;
		else
			sw |= CAN_SWITCH_BUMPER;

		if (IO0PIN & (1<<COLOR_PIN))
			sw |= CAN_SWITCH_COLOR;
		else
			sw &= ~CAN_SWITCH_COLOR;

		if (IO0PIN & (1<<BUMPER_LEFT))
			sw &= ~CAN_SWITCH_LEFT;
		else
			sw |= CAN_SWITCH_LEFT;

		if (IO0PIN & (1<<BUMPER_RIGHT))
			sw &= ~CAN_SWITCH_RIGHT;
		else
			sw |= CAN_SWITCH_RIGHT;

		if (sw & CAN_SWITCH_COLOR)
			deb_led_off(LEDY);
		else
			deb_led_on(LEDY);
		
		if (sw & (CAN_SWITCH_BUMPER|CAN_SWITCH_LEFT|CAN_SWITCH_RIGHT))
			deb_led_on(LEDR);
		else
			deb_led_off(LEDR);
		
		/* TODO: Put color to the second bit */
			
		tx_msg.id = CAN_ROBOT_SWITCHES;
		tx_msg.dlc = 1;
		tx_msg.flags = 0;
		tx_msg.data[0] = sw;
		
		can_tx_msg(&tx_msg);
	}
}

void test_vhn()
{
	vhn_init();
	
	char dir = ENGINE_DIR_BW;
	int spd = 0;
	while(1) {
		spd = (spd+1)%100;
		if (spd == 0) {
			dir = !dir;
			vhn_reset();
		}
		vhn_speed(spd, dir);
		send_rs_int(spd);
		uart_send_char(' ');
		send_rs_int(dir);
		send_rs_str(" Vidle started\n");
/* 		char ch = uart_get_char(); */
/* 		switch (ch) { */
/* 		case '-': */
/* 			dir = !dir; */
/* 			break; */
/* 		case '0': */
/* 			vhn_speed(0, dir); */
/* 			break; */
/* 		case '1': */
/* 			vhn_speed(0x40, dir); */
/* 			break; */
/* 		case '2': */
/* 			vhn_speed(0x80, dir); */
/* 			break; */
/* 		case '3': */
/* 			vhn_speed(0xa0, dir); */
/* 			break; */
/* 		default: */
/* 			ch='?'; */
/* 		} */
/* 		uart_send_char(ch); */
//		blink_led();
	}

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(void)
{
	uint32_t main_time = timer_usec;
	uint32_t status_time = timer_usec;
	

	init_periphery();
	
	/* TODO: Add comment FIXME: zkusit smazat, mam moct ze to melo neco spojeneho s chelae z eb09  */
	//SET_PIN(PINSEL1, 1, PINSEL_0);
	//SET_PIN(PINSEL1, 3, PINSEL_0);
	
	
	
	
	SET_PIN(PINSEL0, START_PIN, PINSEL_0);		// inicializace start pinu
	SET_PIN(PINSEL0, COLOR_PIN, PINSEL_0);
	SET_PIN(PINSEL1, 1, PINSEL_0);		// inicializace bumper pinu (FIXME SET_PIN je BLBA implemetace, musim ji nekdy opravit)

 	SET_PIN(PINSEL1, 3, PINSEL_0); 
 	SET_PIN(PINSEL0, BUMPER_RIGHT, PINSEL_0); 

	
	IO0DIR &= ~((1<<START_PIN) | (1<<BUMPER_RIGHT) | (1 << COLOR_PIN));
	IO0DIR &= ~((1<<BUMPER_PIN) | (1<<BUMPER_LEFT));
	
	//IO1DIR &= ~(3<<20);

	send_rs_str("Vidle started\n");
	// The above send_rs_str is importat - we wait for the first AD conversion to be finished
	fsm_vidle.act_pos = adc_val[0];
	init_fsm(&fsm_vidle, &fsm_vidle_init);

/* 	test_vhn(); */
/* 	return; */
	
	while(1){
		if(timer_usec >= main_time + 1000)
		{
			main_time = timer_usec;

			//dbg_print_time();

			fsm_vidle.act_pos = adc_val[0];
			
		    
			run_fsm(&fsm_vidle);
		}

		if (timer_msec >= status_time + 100 || //repeat sending message every 100 ms
		    fsm_vidle.trigger_can_send) {   //or when something important happen
			fsm_vidle.trigger_can_send = false;
			status_time = timer_msec; //save new time, when message was sent
			can_send_status();

		}

		start_button();
		handle_bumper();

		blink_led();
	}
}

/** @} */