#include <lpc21xx.h>
#include <deb_led.h>
#include <system_def.h>	
#include <string.h>
#include <can_msg_def.h>
#include "uar.h"
#include "fsm.h"
#include <engine.h>
#include <stdbool.h>
#include "def.h"
#include <adc.h>

#define DBG_ENTRY() do {			\
		send_rs_str(__func__);		\
		send_rs_str(": entry\n");	\
	} while(0);

static void wait_for_cmd(struct fsm *fsm, enum event event);
static void move(struct fsm *fsm, enum event event);


void fsm_vidle_init(struct fsm *fsm, enum event event)
{
	switch (event) {
	case EVENT_ENTRY:
		DBG_ENTRY();
		fsm->can_req_position = fsm->act_pos;
		fsm->flags |= CAN_VIDLE_INITIALIZING;
		break;
	case EVENT_DO:
		/* TODO: Homing */
		fsm->flags &= ~CAN_VIDLE_INITIALIZING;
		fsm->current_state = wait_for_cmd;
		break;
	case EVENT_EXIT:
		break;
	}
}

static void stop()
{
	engine_A_pwm(0);
	engine_A_en(ENGINE_EN_OFF);
}


#define DEAD_ZONE	10
static bool do_control(struct fsm *fsm, int P)
{
	bool finished;
	int e = fsm->req_pos - fsm->act_pos;
	int action = (P*e) / 10;		// ORIGINAL: int action = P*e;

	engine_A_dir(action < 0);
//#define BANG_BANG
#ifdef BANG_BANG
	action = action > 0 ? action : -action;
	action = action > 40 ? 100 : 0;
#else
	action = action > 0 ? action : -action;
#endif
	engine_A_pwm(action);
	engine_A_en(ENGINE_EN_ON);


	if (fsm->req_target > fsm->start_pos)
		finished = fsm->act_pos > fsm->req_target - DEAD_ZONE;
	else
		finished = fsm->act_pos < fsm->req_target + DEAD_ZONE;

	return finished;
}

static void wait_for_cmd(struct fsm *fsm, enum event event)
{
	static int last_can_req_pos = 0;
	switch (event) {
	case EVENT_ENTRY:
		DBG_ENTRY();
		stop();
		break;
	case EVENT_DO:
		do_control(fsm, 2);
		if (fsm->can_req_position != last_can_req_pos &&
		    fsm->can_req_position != fsm->req_target) {
			last_can_req_pos = fsm->can_req_position;
			fsm->req_target = fsm->can_req_position;
			fsm->req_spd = fsm->can_req_spd;
			fsm->current_state = move;
		}
		break;
	case EVENT_EXIT:
		break;
	}
}

#define XMIN(a,b) ((a) < (b) ? (a) : (b))
#define XMAX(a,b) ((a) > (b) ? (a) : (b))
static void move(struct fsm *fsm, enum event event)
{
	static int counter;
	bool finished;
	switch (event) {
	case EVENT_ENTRY:
		counter = 0;
		DBG_ENTRY();
		fsm->time_start = timer_msec;
		fsm->start_pos = fsm->act_pos;
		if(fsm->req_spd == 0)
			fsm->req_pos = fsm->req_target;
		else
			fsm->req_pos = fsm->start_pos;
		break;
	case EVENT_DO:
		if (fsm->can_req_position != fsm->req_target) {
			fsm->flags |= CAN_VIDLE_TIMEOUT;
			fsm->current_state = wait_for_cmd;
		}
		if(fsm->req_spd != 0 && counter++ >= 10)
		{
			counter = 0;
			if(fsm->req_target > fsm->start_pos) {
				  fsm->req_pos = XMIN(fsm->req_pos + fsm->req_spd,fsm->req_target);
			} else {
				  fsm->req_pos = XMAX(fsm->req_pos - fsm->req_spd,fsm->req_target);
			}
		}
		if (timer_msec - fsm->time_start > (fsm->req_spd == 0 ? 1000 : 3000)) {
			fsm->flags |= CAN_VIDLE_TIMEOUT;
			fsm->can_response = fsm->req_target;
			fsm->current_state = wait_for_cmd;
			fsm->req_pos = fsm->act_pos;
		}			
		finished = do_control(fsm, fsm->req_spd ? 2 : 2);
		if (finished && fsm->req_pos == fsm->req_target) {
			fsm->can_response = fsm->req_target;
			fsm->current_state = wait_for_cmd;
		}
		break;
	case EVENT_EXIT:
		stop();
		fsm->trigger_can_send = true;;
		break;
	}
}
