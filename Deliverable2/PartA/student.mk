##### Student Makefile for the ESS deliverables #####
# Change this file to match your requirements

# Name of your project
LD_NAME = Deliverable2A

# Add lists of space separated source files
# Own sources, e.g. main.c
SRCS = main.c VirtualSerial.c Descriptors.c setup.c
# Library sources, e.g. xmc_gpio.c
LIBSRCS = USBController_XMC4000.c xmc_usbd.c CDCClassDevice.c USBTask.c Endpoint_XMC4000.c xmc4_scu.c EndpointStream_XMC4000.c memcpy.c
# Precompiled libraries, e.g. -lm for math functions
LIBLNK =

# Change this line if the path to your XMC-Library differs, it will be overwritten before grading to
# match the system the grading is performed on.
XMC_LIBDIR = /opt/XMClib/XMC_Peripheral_Library_v2.1.16

# Language dialect
SCFLAGS  = -std=gnu99
# Optimization level, remember that enabling optimization will stirr up the assembly code and thus
# debugging is more difficult
SCFLAGS += -O0
# If you like, you can enable even more warnings, e.g. -Wextra, but for grading -Wall will be used
SCFLAGS +=
