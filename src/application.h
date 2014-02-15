#ifndef APPLICATION_H
#define APPLICATION_H

#include "events.h"
#include "states.h"

int main(void);
int32_t get_time(void);
static void clear_timer(void);
static void deinit(void);
static void down_click_handler(ClickRecognizerRef, void*);
static void draw_workout_screen(unsigned int);
static void init(void);
static void process_event(event_t);
static void process_normal_event(event_t);
static void process_workout_event(event_t);
static void select_click_handler(ClickRecognizerRef, void*);
static void set_timer(uint32_t);
static void set_workout_state(uint32_t state);
static void setup_click_listeners(void*);
static void switch_state(state_t);
static void timer_tick(void*);
static void up_click_handler(ClickRecognizerRef, void*);
static void window_load(Window*);
static void window_unload(Window*);
static void workout_time_low(void);
static void workout_time_max(void);
static void workout_time_mid(void);

#endif
