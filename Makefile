CC := clang
CFLAGS := -std=c2x -g -Wall -Werror -Wextra

LIBS := $(wildcard lib/*.c)
SRCS := $(wildcard tests/*.c)
OBJS := $(SRCS:.c=.o)

TARGET := test

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) -I.

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.

clean:
	rm -f $(OBJS) bin/$(TARGET)
	git clean -Xf

build_dll:
	$(CC) $(CFLAGS) -shared -o bin/libllvm.dll tests/llvm.c -I. $(LIBS)

.PHONY: all clean
.DEFAULT_GOAL := all