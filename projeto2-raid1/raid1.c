#include "raid1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* DISK0 = "disk0.bin";
static const char* DISK1 = "disk1.bin";
static const char* LOGFILE = "raid_log.bin";

int raid1_init(void) {
    char empty[BLOCK_SIZE] = {0};
    
    FILE *d0 = fopen(DISK0, "wb");
    FILE *d1 = fopen(DISK1, "wb");
    if (!d0 || !d1) return -1;
    
    for(int i=0; i<NUM_BLOCKS; i++) {
        fwrite(empty, 1, BLOCK_SIZE, d0);
        fwrite(empty, 1, BLOCK_SIZE, d1);
    }
    fclose(d0);
    fclose(d1);
    
    FILE *l = fopen(LOGFILE, "wb");
    if (l) fclose(l);
    
    return 0;
}

static int log_transaction(int tx_id, int block_id, const char* buffer, int size) {
    FILE *l = fopen(LOGFILE, "ab");
    if (!l) return -1;
    
    struct log_entry tx = {0};
    tx.transaction_id = tx_id;
    tx.block_id = block_id;
    tx.status = TX_PENDING;
    memcpy(tx.data, buffer, size > BLOCK_SIZE ? BLOCK_SIZE : size);
    
    fwrite(&tx, sizeof(struct log_entry), 1, l);
    fclose(l);
    return 0;
}

static int commit_transaction(int tx_id) {
    FILE *l = fopen(LOGFILE, "r+b");
    if (!l) return -1;
    
    struct log_entry tx;
    int found = 0;
    while(fread(&tx, sizeof(struct log_entry), 1, l) == 1) {
        if(tx.transaction_id == tx_id && tx.status == TX_PENDING) {
            tx.status = TX_COMMITTED;
            fseek(l, -sizeof(struct log_entry), SEEK_CUR);
            fwrite(&tx, sizeof(struct log_entry), 1, l);
            found = 1;
            break;
        }
    }
    fclose(l);
    return found ? 0 : -1;
}

static int write_to_disk(const char* disk, int block_id, const char* buffer, int size) {
    FILE *f = fopen(disk, "r+b");
    if (!f) return -1;
    fseek(f, block_id * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, 1, size > BLOCK_SIZE ? BLOCK_SIZE : size, f);
    fclose(f);
    return 0;
}

int raid1_write(int block_id, const char* buffer, int size) {
    static int tx_counter = 1;
    int current_tx = tx_counter++;
    
    // 1. Logar intencao de escrita
    if(log_transaction(current_tx, block_id, buffer, size) != 0) return -1;
    
    // 2. Escrever nos discos
    if(write_to_disk(DISK0, block_id, buffer, size) != 0) return -1;
    if(write_to_disk(DISK1, block_id, buffer, size) != 0) return -1;
    
    // 3. Confirmar a transacao
    commit_transaction(current_tx);
    return 0;
}

char* raid1_read(int block_id, int disk_num) {
    const char* target = disk_num == 0 ? DISK0 : DISK1;
    FILE *f = fopen(target, "rb");
    if (!f) return NULL;
    
    char* buf = malloc(BLOCK_SIZE + 1);
    fseek(f, block_id * BLOCK_SIZE, SEEK_SET);
    int read_bytes = fread(buf, 1, BLOCK_SIZE, f);
    if(read_bytes <= 0) {
        free(buf);
        fclose(f);
        return NULL;
    }
    buf[read_bytes] = '\0';
    fclose(f);
    return buf;
}

int raid1_verify(void) {
    FILE *d0 = fopen(DISK0, "rb");
    FILE *d1 = fopen(DISK1, "rb");
    if(!d0 || !d1) return -1;
    
    int divergences = 0;
    char b0[BLOCK_SIZE], b1[BLOCK_SIZE];
    
    for(int i=0; i<NUM_BLOCKS; i++) {
        fread(b0, 1, BLOCK_SIZE, d0);
        fread(b1, 1, BLOCK_SIZE, d1);
        
        if(memcmp(b0, b1, BLOCK_SIZE) != 0) {
            printf("Divergencia encontrada no bloco %d!\n", i);
            divergences++;
        }
    }
    
    fclose(d0);
    fclose(d1);
    return divergences;
}

int raid1_recover(void) {
    FILE *l = fopen(LOGFILE, "rb");
    if(!l) return -1;
    
    struct log_entry tx;
    int recovered = 0;
    while(fread(&tx, sizeof(struct log_entry), 1, l) == 1) {
        if(tx.status == TX_COMMITTED) {
            // Reaplica nos 2 discos caso algum tenha falhado silenciomente.
            write_to_disk(DISK0, tx.block_id, tx.data, BLOCK_SIZE);
            write_to_disk(DISK1, tx.block_id, tx.data, BLOCK_SIZE);
            recovered++;
        }
    }
    fclose(l);
    return recovered;
}

void raid1_status(void) {
    FILE *d0 = fopen(DISK0, "rb");
    FILE *d1 = fopen(DISK1, "rb");
    if(d0) { printf("Disco 0: Online\n"); fclose(d0); }
    else { printf("Disco 0: Offline\n"); }
    
    if(d1) { printf("Disco 1: Online\n"); fclose(d1); }
    else { printf("Disco 1: Offline\n"); }
    
    FILE *l = fopen(LOGFILE, "rb");
    if(l) { 
        int pending=0, committed=0;
        struct log_entry tx;
        while(fread(&tx, sizeof(struct log_entry), 1, l) == 1) {
            if(tx.status == TX_PENDING) pending++;
            else if(tx.status == TX_COMMITTED) committed++;
        }
        printf("WAL Log: %d pendentes, %d executados.\n", pending, committed);
        fclose(l);
    } else {
        printf("WAL Log: Não inicializado.\n");
    }
}
