CC = gcc
CFLAGS = -O2 -Wall
DEPS = colors.h icons.h

all: disk battery temp load time

%: %.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<
