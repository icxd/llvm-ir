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

build_dll:
	$(CC) $(CFLAGS) -shared -o bin/libllvm.dll src/llvm.c -I. $(LIBS)

.PHONY: all clean
.DEFAULT_GOAL := all