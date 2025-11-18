/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "ztimer.h"
#include "phydat.h"
#include "saul_reg.h"
#include "board.h"

#define TEMPERATURE_THRESHOLD 2500 /* factor of 10^-2 */
#define Z_FLIPPED -1000
#define ACCEL_JITTER 100

int main(void)
{
    ztimer_sleep(ZTIMER_MSEC, 1000);    // wait pyterm for connection
    puts("SAUL example application");

    /* start by finding a temperature sensor in the system */
    saul_reg_t *temp_sensor = saul_reg_find_type(SAUL_SENSE_TEMP);
    if (!temp_sensor) {
        puts("No temperature sensor present");
        return 1;
    }
    else {
        printf("Found temperature device: %s\n", temp_sensor->name);
    }

    /* [TASK 3: find your device here] */
    saul_reg_t *accel_sensor = saul_reg_find_type(SAUL_SENSE_ACCEL);
    if (!accel_sensor) {
        puts("No accelerometer present");
        return 1;
    }
    else {
        printf("Found accelerometer device: %s\n", accel_sensor->name);
    }

    /* record the starting time */
    ztimer_now_t last_wakeup = ztimer_now(ZTIMER_MSEC);

    while (1) {
        /* read a temperature value from the sensor */
        phydat_t temperature;
        int dimensions = saul_reg_read(temp_sensor, &temperature);
        if (dimensions < 1) {
            puts("Error reading a value from the temperature sensor");
            break;
        }

        phydat_t acceleration;
        int acc_dim = saul_reg_read(accel_sensor, &acceleration);
        if (acc_dim < 1) {
            puts("Error reading a value from the accelerometer");
            break;
        }

        /* dump the read value to STDIO */
        phydat_dump(&temperature, dimensions);

        /* [TASK 3: perform the acceleration read here ] */
        phydat_dump(&acceleration, acc_dim);

        /* check if the temperature value is above the threshold */
        if (temperature.val[0] >= TEMPERATURE_THRESHOLD) {
            LED0_ON;
        }
        else {
            LED0_OFF;
        }

        // check if the board has been flipped 180 °
        if (acceleration.val[2] > Z_FLIPPED - ACCEL_JITTER
            && acceleration.val[2] < Z_FLIPPED + ACCEL_JITTER) {
            LED1_ON;
        }
        else {
            LED1_OFF;
        }

        /* wait for 500 ms */
        ztimer_periodic_wakeup(ZTIMER_MSEC, &last_wakeup, 500);
    }

    return 0;
}
