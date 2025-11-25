/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "board.h"
#include "ztimer.h"
#include "periph/gpio.h"

/* [TASK 1: define led0 here] */
gpio_t led0 = GPIO_PIN(1, 9);

/* [TASK 2: define button and led1 here] */
gpio_t led1 = GPIO_PIN(1, 10);
gpio_t button = GPIO_PIN(1, 2);

/* [TASK 2: write the callback function here] */
void button_callback(void *arg) {
    (void)arg; /* the argument is not used */
    if (!gpio_read(button)) {
        gpio_set(led1);
    } else {
        gpio_clear(led1);
    }
}

int main(void) {
    puts("GPIOs example.");

    /* [TASK 1: initialize and use led0 here] */
    gpio_init(led0, GPIO_OUT);
    gpio_init(led1, GPIO_OUT);
    gpio_clear(led0);
    gpio_clear(led1);

    gpio_init_int(button, GPIO_IN_PU, GPIO_BOTH, button_callback, NULL);
    while (1) {
        gpio_toggle(led0);
        ztimer_sleep(ZTIMER_MSEC, 500);
    }

    return 0;
}
