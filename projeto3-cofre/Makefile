CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lssl -lcrypto
TARGET = cofre

all: $(TARGET)

$(TARGET): cofre.c main_cofre.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.bin

.PHONY: all clean
