CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = raid1

all: $(TARGET)

$(TARGET): raid1.c main_raid1.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) disco_fisico_0.bin disco_fisico_1.bin registro_transacoes.bin

.PHONY: all clean
