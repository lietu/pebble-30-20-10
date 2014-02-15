#include "states.h"

const char* state_to_str(state_t state) {
    switch (state) {
        case STATE_DEFAULT: return "STATE_DEFAULT";
        case STATE_WORKING_OUT: return "STATE_WORKING_OUT";
    }

    return "UNKNOWN";
}
