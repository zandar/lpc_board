#ifndef FSM_H
#define FSM_H

#include <types.h>
#include <stdbool.h>

#define ADC_SAMPLES	500
#define ADC_PERIOD	10	// ms

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
	volatile bool adc_start;
	volatile bool send_samples;
	volatile bool scope_ready;
	int adc_data[2][ADC_SAMPLES];
};

void init_fsm(struct fsm *fsm, state_fcn initial_state);
void run_fsm(struct fsm *fsm);

#endif
