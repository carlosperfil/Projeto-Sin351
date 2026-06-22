CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lssl -lcrypto

BUILD_DIR = build
SRC_DIR = src

# Targets para os executáveis
VSFS_BIN = $(BUILD_DIR)/vsfs
RAID1_BIN = $(BUILD_DIR)/raid1
COFRE_BIN = $(BUILD_DIR)/cofre

all: clean dirs $(VSFS_BIN) $(RAID1_BIN) $(COFRE_BIN)

dirs:
	mkdir -p $(BUILD_DIR)

# Projeto 1: vsfs
$(VSFS_BIN): $(SRC_DIR)/persistence/vsfs/vsfs.c $(SRC_DIR)/persistence/vsfs/main_vsfs.c
	$(CC) $(CFLAGS) $^ -o $@

# Projeto 2: raid1
$(RAID1_BIN): $(SRC_DIR)/persistence/raid1/raid1.c $(SRC_DIR)/persistence/raid1/main_raid1.c
	$(CC) $(CFLAGS) $^ -o $@

# Projeto 3: cofre (com OpenSSL)
$(COFRE_BIN): $(SRC_DIR)/security/cofre/cofre.c $(SRC_DIR)/security/cofre/main_cofre.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) *.bin *.img

.PHONY: all dirs clean vsfs raid1 cofre
