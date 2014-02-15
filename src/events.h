#ifndef EVENTS_H
#define EVENTS_H

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_SELECT,
    TIMER_EVENT

} event_t;

const char* event_to_str(event_t);

#endif
