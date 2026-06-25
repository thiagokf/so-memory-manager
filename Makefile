CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

TARGET = simulador
SRCS = src/main.c src/memoria_fisica.c src/processo.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
