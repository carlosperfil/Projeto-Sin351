#ifndef RAID1_H
#define RAID1_H

#include <stdint.h>

#define BLOCK_SIZE 4096
#define NUM_BLOCKS 256
#define NUM_DISKS 2

typedef enum {
    TX_PENDING = 0,
    TX_COMMITTED = 1
} log_status_t;

struct log_entry {
    int transaction_id;
    int block_id;
    log_status_t status;
    char data[BLOCK_SIZE];
};

struct raid_inode {
    int file_id;
    int logical_block_id; // mapeado pro mesmo offset nos 2 discos físicos
    int size;
    int type;
};

int raid1_init(void);
int raid1_write(int block_id, const char* buffer, int size);
char* raid1_read(int block_id, int disk_num);
int raid1_verify(void);
int raid1_recover(void);
void raid1_status(void);

#endif
