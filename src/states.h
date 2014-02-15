#ifndef STATES_H
#define STATES_H

typedef enum {
    STATE_DEFAULT,
    STATE_WORKING_OUT
} state_t;

const char* state_to_str(state_t state);

#endif
