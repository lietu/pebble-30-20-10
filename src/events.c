#include "events.h"

const char* event_to_str(event_t event) {
    switch (event) {
        case BUTTON_UP: return "BUTTON_UP";
        case BUTTON_DOWN: return "BUTTON_DOWN";
        case BUTTON_SELECT: return "BUTTON_SELECT";
        case TIMER_EVENT: return "TIMER_EVENT";
    }

    return "UNKNOWN";
}
