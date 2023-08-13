CC := clang
CFLAGS := -std=c2x

LIBS := $(wildcard lib/*.c)
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

TARGET := main

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) -I.

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.

clean:
	rm -f $(OBJS) bin/$(TARGET)

.PHONY: all clean
.DEFAULT_GOAL := all