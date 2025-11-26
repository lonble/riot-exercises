/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "periph/gpio.h"
#include "board.h"
#include "thread.h"
#include "event.h"

/* [TASK 2: include Event Thread header here] */
#include "event/thread.h"

#include "ztimer.h"

/* [TASK 2: create event handler here] */
void event_handler(event_t *event) {
    (void)event; /* Not used */

    ztimer_now_t start = ztimer_now(ZTIMER_MSEC);

    /* wait until 2000 ms have passed */
    while (ztimer_now(ZTIMER_MSEC) - start < 2000) { }

    LED0_TOGGLE;
    puts("Done");
}

/* [TASK 2: instantiate queue and event here] */
event_t event = { .handler = event_handler };

// void button_callback(void *arg) {
//     (void)arg; /* Not used */

//     /* [TASK 1 and 2: implement interrupt routing here] */
//     /* get the current time */
//     ztimer_now_t start = ztimer_now(ZTIMER_MSEC);

//     /* wait until 2000 ms have passed */
//     while (ztimer_now(ZTIMER_MSEC) - start < 2000) { }

//     LED0_TOGGLE;
//     puts("Done");
// }

void button_callback(void *arg) {
    (void)arg; /* Not used */

    event_post(EVENT_PRIO_LOWEST, &event);
}

int main(void) {
    puts("Threads and event queue example.");

    /* Setup button callback */
    if (gpio_init_int(BTN0_PIN, BTN0_MODE, GPIO_FALLING, button_callback, NULL) < 0) {
        puts("[FAILED] init BTN0!");
        return 1;
    }

    while (1) {
        puts("Main");
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }

    /* Should never reach here */

    return 0;
}
