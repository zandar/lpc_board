#ifndef FSM_H
#define FSM_H

#include <types.h>
#include <stdbool.h>

// events of each state of state machine
enum event {
	EVENT_ENTRY,
	EVENT_DO,
	EVENT_EXIT
}; 

struct fsm;

typedef void (*state_fcn)(struct fsm *fsm, enum event my_event);//pointer to function returning void and two input parametr

struct fsm {
	state_fcn current_state; 	        // current state
	state_fcn last_state;			// last state
	int32_t act_pos;			// actual position
	int32_t req_pos;			// requested position
	int32_t req_spd;
	int32_t req_target;
	volatile int32_t can_req_spd;
	volatile uint32_t can_req_position;	// next requested position
	int32_t start_pos;
	uint32_t can_response; 			// when the move is done, the value here equals to the req_pos
	uint8_t flags;	 //< CAN flags bits (defined in can_msg_def.h)
	uint32_t time_start;	/* For timeout detection */
	bool trigger_can_send;
};

void init_fsm(struct fsm *fsm, state_fcn initial_state);
void run_fsm(struct fsm *fsm);

#endif