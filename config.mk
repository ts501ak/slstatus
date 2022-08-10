# customize below to fit your system
USE_SWAY  = 1
USE_PULSE = 1 

# paths
PREFIX = /usr/local

# compiler and linker
CC = cc

# flags
CPPFLAGS = -D_DEFAULT_SOURCE 
CFLAGS   = -std=c99 -pedantic -Wall -Wextra -Os
LDFLAGS  = -s  
LDLIBS   = 

ifdef USE_SWAY 
CPPFLAGS += -DUSE_SWAY
LDFLAGS  += -ljson-c
endif

ifdef USE_PULSE 
CPPFLAGS += -DUSE_PULSE
LDFLAGS	 += -lpulse 
endif

