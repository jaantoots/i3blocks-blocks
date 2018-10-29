CC = gcc
CFLAGS = -O2 -Wall
DEPS = colors.h icons.h

all: time

%: %.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<
