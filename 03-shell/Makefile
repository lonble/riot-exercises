# name of your application
APPLICATION = shell

# If no BOARD is found in the environment, use this default:
BOARD ?= adafruit-feather-nrf52840-sense

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# use the shell module
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

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
