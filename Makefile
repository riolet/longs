#!/bin/make

ifdef THREADS
	NTHREAD=-D NOPE_THREADS=$(THREADS)
endif


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

EXT_OPTIONS=$(NTHREAD) $(DEBUG_OPT) $(PROCESSES_OPT) $(LOOP_OPT) $(MAX_CON_CONS_OPT) -pthread

AR=ar
CFLAGS=-W -Wall -O2 -Wno-unused-parameter -g $(EXT_OPTIONS)
LIBLONG_OBJ=wafer.o waferapi.o sqlite3.o
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

test:
	$(CC) $(CFLAGS) -o longs_test longs_test.c  $(LIBLONG) $(OPTIONS)

clean:
	rm -f $(LIBLONG_OBJ) $(MODULES)

distclean:
	rm -f $(LIBLONG) $(LIBNOPE_OBJ) $(MODULES)

