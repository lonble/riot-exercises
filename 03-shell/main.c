/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "shell.h"
#include "board.h"

/* [TASK 2: add command handler here] */
int toggle_command(int argc, char **argv) {
    /* check that the command is called correctly (no extra arguments) */
    if (argc != 2) {
        printf("usage: %s <led_number>\n", argv[0]);
        return 1;
    }

    // convert the argument to an integer
    int number = atoi(argv[1]);

    /* toggle the LED */
    switch (number) {
    case 0:
        LED0_TOGGLE;
        break;
    case 1:
        LED1_TOGGLE;
        break;
    default:
        puts("The board has only two LEDs.");
    }
    return 0;
}

int hex_command(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s <string>\n", argv[0]);
        return 1;
    }
    int len = 0;
    for (char const *p = argv[1]; *p != '\0'; ++p) {
        int first = *p >> 4;
        int second = *p & ((1 << 4) - 1);
        printf("%x%x", first, second);
        ++len;
        if ((len & 15) != 0) {
            putchar(' ');
        } else {
            putchar('\n');
        }
    }
    if ((len & 15) != 0) {
        putchar('\n');
    }
    return 0;
}

int echo_command(int argc, char **argv) {
    /* check that the command is called correctly */
    if (argc != 2) {
        puts("usage: echo <message>");
        puts("Note: to echo multiple words wrap the message in \"\"");
        return 1;
    }

    /* print the first argument */
    puts(argv[1]);

    return 0;
}

/* [TASK 2: register your new command here] */
SHELL_COMMAND(toggle, "Toggle LED", toggle_command);
SHELL_COMMAND(echo, "Echo a message", echo_command);

int main(void) {
    /* buffer to read commands */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_command_t commands[] = {
        {
            .name = "hex",
            .desc = "Print the hexadecimal form of the string.",
            .handler = hex_command,
        },
        { NULL, NULL, NULL },
    };

    // run the first 3 commands manually
    for (int iter = 0; iter < 3; ++iter) {
        putchar('>');
        putchar(' ');
        fflush(stdout);
        int len = shell_readline(line_buf, SHELL_DEFAULT_BUFSIZE);
        if (len == EOF) {
            puts("shell: EOF reached. exiting");
            break;
        } else if (len == -ENOBUFS) {
            puts("shell: maximum line length exceeded");
            continue;
        } else {
            int rev = shell_handle_input_line(commands, line_buf);
            if (rev != 0) {
                printf("shell: return value of last command is %d\n", rev);
            }
            continue;
        }
    }

    puts("switch to built-in shell");

    /* run the shell, this will block the thread waiting for incoming commands */
    shell_run_forever(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
