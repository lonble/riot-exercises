/*
 * Copyright (C) 2016-17 Freie Universität Berlin
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <martine.lenders@tu-dresden.de>
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "msg.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "net/sock/async/event.h"
#include "net/ipv6/addr.h"
#include "shell.h"
#include "thread.h"

#define SERVER_BUFFER_SIZE SHELL_DEFAULT_BUFSIZE

#define START "start"
#define STOP "stop"
#define SOCK_MAX 10

// static sock_udp_t sock;
// static bool server_running = false;
static char server_buffer[SERVER_BUFFER_SIZE];
static char server_stack[THREAD_STACKSIZE_DEFAULT];
static event_queue_t queue;

static char *udps_args[2];
sock_udp_t sock_buffer[SOCK_MAX];
bool sock_used[SOCK_MAX];

// returns true if the conversion is successful, or false if any error is encountered
// "port" will only be modified on a successful conversion
static inline bool str2port(char const *str, uint16_t *port) {
    if (str == NULL || *str == '\0') {
        return false;
    }

    uintmax_t num = 0;
    for (char const *iter = str; *iter != '\0'; ++iter) {
        if (*iter >= '0' && *iter <= '9') {
            num = num * 10 + *iter - '0';
        } else {
            return false;
        }
    }

    if (num <= 65535) {
        *port = num;
        return true;
    }

    return false;
}

// return index of socket which has bound the port, or -1 if the port is unused
static inline int sock_index(uint16_t port) {
    for (int iter = 0; iter < SOCK_MAX; ++iter) {
        if (sock_used[iter] && sock_buffer[iter].local.port == port) {
            return iter;
        }
    }
    return -1;
}

/* [TASK 3: Add sock_udp event handler here] */
void sock_udp_handler(sock_udp_t *sock, sock_async_flags_t type, void *arg) {
    (void)arg; // unused

    if (type & SOCK_ASYNC_MSG_RECV) {
        // store source socket in "remote"
        sock_udp_ep_t remote;
        int res = sock_udp_recv(sock,
                                server_buffer,
                                sizeof(server_buffer),
                                0,
                                &remote);
        if (res < 0) {
            puts("Error while receiving");
        } else {
            printf("From: [");
            ipv6_addr_print((ipv6_addr_t *)remote.addr.ipv6);
            printf("]:%u, ", remote.port);
            if (res == 0) {
                puts("No data received");
            } else {
                printf("Received: %.*s\n", res, server_buffer);
            }
        }
    }
}

/* [TASK 3: Add further event handlers here] */
void udps_command_handler(event_t *event) {
    (void)event; // useless

    // check port
    uint16_t port = 0;
    if (!str2port(udps_args[1], &port)) {
        printf("\"%s\" is not a valid port\n", udps_args[1]);
        return;
    }

    if (strcmp(udps_args[0], START) == 0) {
        // check if socket is already bound
        if (port != 0 && sock_index(port) != -1) {
            puts("udp server is already running");
            return;
        }

        // find available socket
        int available = -1;
        for (int iter = 0; iter < SOCK_MAX; ++iter) {
            if (!sock_used[iter]) {
                available = iter;
            }
        }
        if (available == -1) {
            puts("you bound too many sockets");
            return;
        }
        sock_udp_t *sock = &sock_buffer[available];

        // bind socket
        sock_udp_ep_t local = {
            .family = AF_INET6,
            .addr = IPV6_ADDR_UNSPECIFIED,
            .port = port,
        };
        if (sock_udp_create(sock, &local, NULL, 0) != 0) {
            puts("unable to bind this port");
            return;
        }
        // mark the socket as used
        sock_used[available] = true;
        // print binding information
        printf("bound socket [");
        ipv6_addr_print((ipv6_addr_t *)sock->local.addr.ipv6);
        printf("]:%u\n", sock->local.port);

        // use event to handle packets to this socket
        sock_udp_event_init(sock, &queue, &sock_udp_handler, NULL);
    } else if (strcmp(udps_args[0], STOP) == 0) {
        if (port == 0) {
            // stop all udp servers
            for (int iter = 0; iter < SOCK_MAX; ++iter) {
                if (sock_used[iter]) {
                    sock_udp_event_close(&sock_buffer[iter]);
                    sock_udp_close(&sock_buffer[iter]);
                    sock_used[iter] = false;
                }
            }
            puts("all udp servers are stoped");
        } else {
            int index = sock_index(port);
            if (index == -1) {
                puts("udp server is not running");
                return;
            }
            sock_udp_event_close(&sock_buffer[index]);
            sock_udp_close(&sock_buffer[index]);
            sock_used[index] = false;
            printf("stoped udp server on port %u\n", port);
        }
    }
}

event_t udps_command_event = { .handler = &udps_command_handler };

/* [TASK 3: add shell handlers here] */
int udps_command(int argc, char **argv) {
    // check arguments count
    if (argc != 3
        || (strcmp(argv[1], START) != 0
            && strcmp(argv[1], STOP) != 0)) {
        puts("usage: udps " START "|" STOP " <port>");
        return 1;
    }

    udps_args[0] = argv[1];
    udps_args[1] = argv[2];
    event_post(&queue, &udps_command_event);

    return 0;
}

/* [TASK 3: add thread handler here] */
void *udps(void *arg) {
    (void)arg; // unused

    // an emotionless event handling doormat
    event_queue_init(&queue);
    event_loop(&queue);

    // never reached
    return NULL;
}

/* [TASK 2: add thread handler here] */
// void *udps(void *arg) {
//     // check port
//     uint16_t port = 0;
//     if (!str2port(arg, &port)) {
//         printf("\"%s\" is not a valid port\n", (char *)arg);
//         return NULL;
//     }

//     // bind socket
//     sock_udp_ep_t local = {
//         .family = AF_INET6,
//         .addr = IPV6_ADDR_UNSPECIFIED,
//         .port = port,
//     };
//     if (sock_udp_create(&sock, &local, NULL, 0) != 0) {
//         puts("unable to bind this port");
//         return NULL;
//     }
//     server_running = true;
//     printf("bound socket [");
//     ipv6_addr_print((ipv6_addr_t *)sock.local.addr.ipv6);
//     printf("]:%u\n", sock.local.port);

//     while (true) {
//         // wait for udp packet, store source socket in "remote"
//         sock_udp_ep_t remote;
//         int res = sock_udp_recv(&sock,
//                                 server_buffer,
//                                 sizeof(server_buffer),
//                                 SOCK_NO_TIMEOUT,
//                                 &remote);
//         if (res < 0) {
//             puts("Error while receiving");
//         } else {
//             printf("From: [");
//             ipv6_addr_print((ipv6_addr_t *)remote.addr.ipv6);
//             printf("]:%u, ", remote.port);
//             if (res == 0) {
//                 puts("No data received");
//             } else {
//                 printf("Received: %.*s\n", res, server_buffer);
//             }
//         }
//     }

//     // never reached
//     sock_udp_close(&sock);
//     return NULL;
// }

/* [TASK 1 & 2: add shell handlers] */
int udp_command(int argc, char **argv) {
    // check arguments count
    if (argc != 3) {
        puts("usage: udp <IPv6 address>:<port> <message>");
        puts("Note: to send multiple words wrap the message in \"\"");
        return 1;
    }

    // parse dest socket
    sock_udp_ep_t remote = { 0 };
    if (sock_udp_str2ep(&remote, argv[1]) < 0) {
        puts("Unable to parse destination address");
        return 1;
    }

    // send udp packet
    int res = sock_udp_send(NULL, argv[2], strlen(argv[2]), &remote);
    if (res < 0) {
        puts("could not send");
        return 1;
    } else {
        printf("Success: send %u byte to %s\n", (unsigned)res, argv[1]);
        return 0;
    }

    return 0;
}

// int udps_command(int argc, char **argv) {
//     // check arguments count
//     if (argc != 2) {
//         puts("usage: udps <port>");
//         return 1;
//     }

//     if (server_running) {
//         puts("udp server is already running");
//         return 1;
//     } else {
//         kernel_pid_t pid = thread_create(server_stack,
//                                          sizeof(server_stack),
//                                          THREAD_PRIORITY_MAIN + 1,
//                                          0,
//                                          &udps,
//                                          argv[1],
//                                          "udps");
//         if (pid < 0) {
//             puts("failed to create udps thread");
//             return 1;
//         }
//     }

//     return 0;
// }

int main(void) {
    puts("sock example.\n");

    /* [TASK 3: You might want to create the thread now here] */
    kernel_pid_t pid = thread_create(server_stack,
                                     sizeof(server_stack),
                                     THREAD_PRIORITY_MAIN + 1,
                                     0,
                                     &udps,
                                     NULL,
                                     "udps");
    if (pid < 0) {
        puts("failed to create udps thread");
        return 1;
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_command_t commands[] = {
        {
            .name = "udp",
            .desc = "Send a udp packet over IPv6",
            .handler = &udp_command,
        },
        {
            .name = "udps",
            .desc = "Start a udp server on the given port over IPv6",
            .handler = &udps_command,
        },
        { NULL, NULL, NULL },
    };
    shell_run_forever(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

/** @} */
