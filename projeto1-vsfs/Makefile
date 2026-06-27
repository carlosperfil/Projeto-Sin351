CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = vsfs

all: $(TARGET)

$(TARGET): vsfs.c main_vsfs.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) disco.img

.PHONY: all clean
