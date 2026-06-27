CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = raid1

all: $(TARGET)

$(TARGET): raid1.c main_raid1.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) disk0.bin disk1.bin raid_log.bin

.PHONY: all clean
