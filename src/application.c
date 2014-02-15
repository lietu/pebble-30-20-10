#include <pebble.h>
#include "application.h"
#include "texts.h"

// How many msec per tick do we want
#define TICK_INTERVAL 50

// Times for min, mid and max intensity workout, in msec
#define TIME_MAX 60000
#define TIME_MID 50000
#define TIME_LOW 30000

#define FONT_NORMAL "RESOURCE_ID_GOTHIC_24"
#define FONT_TIMER "RESOURCE_ID_BITHAM_42_BOLD"

// Some variables for things we want to keep in global state
state_t             current_state = STATE_WORKING_OUT;
static AppTimer*    timer;
static TextLayer*   description_text;
static TextLayer*   timer_text;
static Window*      window;
uint32_t            workout_start = 0;
unsigned int        timer_interval;
unsigned int        workout_state = TIME_LOW;

static void process_event(event_t event) {
/*
    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "Got event %s in state %s",
        event_to_str(event),
        state_to_str(current_state)
    );
*/
    switch(current_state) {
        case STATE_DEFAULT:
            process_normal_event(event);
            break;

        case STATE_WORKING_OUT:
            process_workout_event(event);
            break;
    }
}

static void process_normal_event(event_t event) {
    switch(event) {
        case BUTTON_UP:
            APP_LOG(
                APP_LOG_LEVEL_DEBUG,
                "Ignoring event %s in state %s",
                event_to_str(event),
                state_to_str(current_state)
            );
            break;

        case BUTTON_DOWN:
            APP_LOG(
                APP_LOG_LEVEL_DEBUG,
                "Ignoring event %s in state %s",
                event_to_str(event),
                state_to_str(current_state)
            );
            break;

        case BUTTON_SELECT:
            switch_state(STATE_WORKING_OUT);
            break;

        case TIMER_EVENT:
            // APP_LOG(
            //     APP_LOG_LEVEL_DEBUG,
            //     "Ignoring event %s in state %s",
            //     event_to_str(event),
            //     state_to_str(current_state)
            // );
            break;
    }
}

static void process_workout_event(event_t event) {
    uint32_t elapsed = get_time() - workout_start;
    uint32_t remaining = 0;

    if (elapsed > TIME_MAX) {
        workout_start = get_time();
        set_workout_state(TIME_LOW);
        elapsed = 0;
    }

    switch(event) {
        case BUTTON_DOWN:
            switch_state(STATE_DEFAULT);
            return;

        case BUTTON_SELECT:
            switch_state(STATE_WORKING_OUT);
            return;

        case BUTTON_UP:
            break;

        case TIMER_EVENT:
            break;
    }


    if (elapsed >= TIME_MID) {
        set_workout_state(TIME_MAX);
        remaining = TIME_MAX - elapsed;
    } else if (elapsed >= TIME_LOW) {
        set_workout_state(TIME_MID);
        remaining = TIME_MID - elapsed;
    } else {
        set_workout_state(TIME_LOW);
        remaining = TIME_LOW - elapsed;
    }

    draw_workout_screen(remaining);
}

static void switch_state(state_t new_state) {
    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "Switching to state %s",
        state_to_str(new_state)
    );

    current_state = new_state;

    clear_timer();

    switch(new_state) {
        case STATE_DEFAULT:
            text_layer_set_text(timer_text, "");
            text_layer_set_text(description_text, text_normal);
            break;

        case STATE_WORKING_OUT:
            workout_start = get_time();
            workout_state = 0;
            workout_time_low();
            draw_workout_screen(TIME_LOW);
            set_timer(TICK_INTERVAL);
            break;
    }
}

static void draw_workout_screen(unsigned int remaining) {
    unsigned int seconds = remaining / 1000;
    unsigned int decimals = (remaining % 1000) / 100;

    static char buf[] = "99.9";
    snprintf(buf, sizeof(buf), "%d.%d", seconds, decimals);
    text_layer_set_text(timer_text, buf);
}


static void workout_time_low(void) {
    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "LOW intensity workout"
    );

    text_layer_set_text(description_text, text_low);
    vibes_short_pulse();
}

static void workout_time_mid(void) {
    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "MID intensity workout"
    );

    text_layer_set_text(description_text, text_mid);
    vibes_long_pulse();
}

static void workout_time_max(void) {
    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "MAX intensity workout"
    );

    text_layer_set_text(description_text, text_max);
    vibes_double_pulse();
}

static void set_workout_state(uint32_t state) {
    if (workout_state != state) {
        workout_state = state;

        switch (state) {
            case TIME_LOW:
                workout_time_low();
                break;

            case TIME_MID:
                workout_time_mid();
                break;

            case TIME_MAX:
                workout_time_max();
                break;
        }
    }
}

/*
 * Timer handling
 */

static void set_timer(uint32_t msec) {
    timer = app_timer_register(msec, timer_tick, NULL);
    timer_interval = msec;
}

static void clear_timer(void) {
    if (timer != NULL) {
        app_timer_cancel(timer);
        timer = NULL;
        timer_interval = 0;
    }
}

int32_t get_time(void) {
    time_t seconds;
    uint16_t millis;
    time_ms(&seconds, &millis);

    return (1000 * seconds) + millis;
}


/*
 * Initialization / deinitialization junk
 */

int main(void) {
    init();

    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "Done initializing, pushed window: %p",
        window
    );

    app_event_loop();
    deinit();
}

static void init(void) {
    window = window_create();

    window_set_click_config_provider(window, setup_click_listeners);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    const bool animated = true;
    window_stack_push(window, animated);

    switch_state(STATE_DEFAULT);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    description_text = text_layer_create(
        (GRect){ .origin = { 0, 0 }, .size = { bounds.size.w, 70 } }
    );

    timer_text = text_layer_create(
        (GRect){ .origin = { 0, 82 }, .size = { bounds.size.w, 86 } }
    );

    text_layer_set_text_alignment(description_text, GTextAlignmentCenter);
    text_layer_set_text_alignment(timer_text, GTextAlignmentCenter);

    text_layer_set_font(
        description_text,
        fonts_get_system_font(FONT_NORMAL)
    );

    text_layer_set_font(
        timer_text,
        fonts_get_system_font(FONT_TIMER)
    );

    layer_add_child(window_layer, text_layer_get_layer(description_text));
    layer_add_child(window_layer, text_layer_get_layer(timer_text));
}

static void window_unload(Window *window) {
    text_layer_destroy(description_text);
    text_layer_destroy(timer_text);
}

static void deinit(void) {
    window_destroy(window);
}


/*
 * Some boring event handlers
 */

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    process_event(BUTTON_UP);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    process_event(BUTTON_DOWN);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    process_event(BUTTON_SELECT);
}

static void timer_tick(void *data) {
    if (timer_interval > 0) {
        set_timer(timer_interval);
    }

    process_event(TIMER_EVENT);
}

static void setup_click_listeners(void *context) {
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}
