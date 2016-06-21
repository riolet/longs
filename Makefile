#!/bin/make


PTHREAD=-D NOPE_THREADS=$(THREADS) -pthread


ifdef DEBUG
	ifeq ($(DEBUG),0)
		DEBUG_OPT=-ggdb
	else
		DEBUG_OPT=-D NOPE_DEBUG -ggdb
	endif
endif

ifdef PROCESSES
	PROCESSES_OPT=-D NOPE_PROCESSES=$(PROCESSES) -ggdb
endif

ifdef LOOP
	ifeq ($(LOOP),epoll)
		LOOP_OPT=-D NOPE_EPOLL
	endif
endif

ifdef MAX_CON_CONS
	MAX_CON_CONS_OPT=-D NOPE_MAX_CON_CONS=$(MAX_CON_CONS)
endif

ifndef CC
	CC=gcc
endif

EXT_OPTIONS=$(PTHREAD) $(DEBUG_OPT) $(PROCESSES_OPT) $(LOOP_OPT) $(MAX_CON_CONS_OPT)

AR=ar
CFLAGS=-W -Wall -O2 -Wno-unused-parameter -g $(EXT_OPTIONS)
LIBLONG_OBJ=wafer.o waferapi.o sqlite3.o cencode.o cdecode.o
LIBLONG=liblong.a
MODULES=longs
OPTIONS=-ldl

all: $(MODULES)

# rule to build modules
%: %.c $(LIBLONG)
	$(CC) $(CFLAGS) -o $@ $^ $(OPTIONS)

$(LIBLONG): $(LIBLONG_OBJ)
	$(AR) r $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(LIBLONG_OBJ) $(MODULES)

distclean:
	rm -f $(LIBLONG) $(LIBNOPE_OBJ) $(MODULES)

