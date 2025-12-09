# name of your application
APPLICATION = gnrc_netapi

# If no BOARD is found in the environment, use this default:
BOARD ?= adafruit-feather-nrf52840-sense

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# enable default networking devides in the platform
USEMODULE += netdev_default

# automatically initialize network interfaces for enabled devices
USEMODULE += auto_init_gnrc_netif

# enable single interface optimization.
# remove this if more than one interface is present
# USEMODULE += gnrc_netif_single

# add IPv6 support
USEMODULE += gnrc_ipv6_default

# add module to use provided network utils
USEMODULE += netutils

# add module to hex dump data
USEMODULE += od
USEMODULE += ztimer
USEMODULE += ztimer_sec

# add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_cmds_default

# disable echo of shell command
CFLAGS += -DCONFIG_SHELL_NO_ECHO=1
# default STDIO_RX_BUFSIZE is 64, increase it to match SHELL_DEFAULT_BUFSIZE
CFLAGS += -DSTDIO_RX_BUFSIZE=128

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
