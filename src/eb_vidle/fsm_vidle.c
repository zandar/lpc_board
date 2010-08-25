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

void fsm_vidle_init(struct fsm *fsm, enum event event)
{
	switch (event) {
	case EVENT_ENTRY:
		DBG_ENTRY();
		break;
	case EVENT_DO:
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
		DBG_ENTRY();
		break;
	case EVENT_DO:
		break;
	case EVENT_EXIT:
		break;
	}
}
