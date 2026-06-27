#include "vsfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int get_bit(unsigned char byte, int bit) {
    return (byte >> bit) & 1;
}

static void set_bit(unsigned char *byte, int bit) {
    *byte |= (1 << bit);
}

int vsfs_format(const char* disk_name) {
    FILE *f = fopen(disk_name, "wb");
    if (!f) return -1;
    
    // Zera o arquivo de tamanho total
    char empty_block[BLOCK_SIZE] = {0};
    for(int i=0; i<NUM_BLOCKS; i++) {
        fwrite(empty_block, 1, BLOCK_SIZE, f);
    }
    
    // Superbloco (Bloco 0)
    struct superblock sb;
    sb.magic = MAGIC_NUMBER;
    sb.num_inodes = (NUM_INODE_BLOCKS * BLOCK_SIZE) / sizeof(struct inode);
    sb.num_data_blocks = NUM_BLOCKS - 3 - NUM_INODE_BLOCKS;
    sb.inode_bitmap_start = 1;
    sb.data_bitmap_start = 2;
    sb.inode_table_start = 3;
    sb.data_start = 3 + NUM_INODE_BLOCKS;
    
    fseek(f, 0, SEEK_SET);
    fwrite(&sb, sizeof(struct superblock), 1, f);
    fclose(f);
    return 0;
}

// Simples alocador de bit linear
static int alloc_bit(FILE *f, int bitmap_start, int max_bits) {
    fseek(f, bitmap_start * BLOCK_SIZE, SEEK_SET);
    unsigned char block[BLOCK_SIZE];
    fread(block, 1, BLOCK_SIZE, f);
    for(int i = 0; i < max_bits; i++) {
        int byte_idx = i / 8;
        int bit_idx = i % 8;
        if (get_bit(block[byte_idx], bit_idx) == 0) {
            set_bit(&block[byte_idx], bit_idx);
            fseek(f, bitmap_start * BLOCK_SIZE, SEEK_SET);
            fwrite(block, 1, BLOCK_SIZE, f);
            return i;
        }
    }
    return -1;
}

static void read_inode(FILE *f, struct superblock *sb, int inumber, struct inode *in) {
    int inode_offset = (sb->inode_table_start * BLOCK_SIZE) + (inumber * sizeof(struct inode));
    fseek(f, inode_offset, SEEK_SET);
    fread(in, sizeof(struct inode), 1, f);
}

static void write_inode(FILE *f, struct superblock *sb, int inumber, struct inode *in) {
    int inode_offset = (sb->inode_table_start * BLOCK_SIZE) + (inumber * sizeof(struct inode));
    fseek(f, inode_offset, SEEK_SET);
    fwrite(in, sizeof(struct inode), 1, f);
}

int write_file(const char* disk_name, const char* content, int size) {
    FILE *f = fopen(disk_name, "r+b");
    if (!f) return -1;
    
    struct superblock sb;
    fread(&sb, sizeof(struct superblock), 1, f);
    
    int inumber = alloc_bit(f, sb.inode_bitmap_start, sb.num_inodes);
    if (inumber < 0) { fclose(f); return -1; }
    
    struct inode in = {0};
    in.size = size;
    in.type = 1; // regular file
    for(int i=0; i<NUM_DIRECT_PTRS; i++) in.direct[i] = -1;
    in.indirect = -1;
    
    int bytes_written = 0;
    int ptr_idx = 0;
    
    while(bytes_written < size && ptr_idx < NUM_DIRECT_PTRS) {
        int dblock = alloc_bit(f, sb.data_bitmap_start, sb.num_data_blocks);
        if (dblock < 0) break;
        in.direct[ptr_idx++] = dblock;
        
        int to_write = size - bytes_written;
        if(to_write > BLOCK_SIZE) to_write = BLOCK_SIZE;
        
        fseek(f, (sb.data_start + dblock) * BLOCK_SIZE, SEEK_SET);
        fwrite(content + bytes_written, 1, to_write, f);
        bytes_written += to_write;
    }
    
    write_inode(f, &sb, inumber, &in);
    fclose(f);
    return inumber;
}

char* read_file(const char* disk_name, int inumber) {
    FILE *f = fopen(disk_name, "rb");
    if (!f) return NULL;
    
    struct superblock sb;
    fread(&sb, sizeof(struct superblock), 1, f);
    
    struct inode in;
    read_inode(f, &sb, inumber, &in);
    
    if (in.size == 0) { fclose(f); return NULL; }
    
    char* buffer = malloc(in.size + 1);
    int bytes_read = 0;
    
    for(int i=0; i<NUM_DIRECT_PTRS; i++) {
        if(in.direct[i] != -1) {
            int to_read = in.size - bytes_read;
            if(to_read > BLOCK_SIZE) to_read = BLOCK_SIZE;
            
            fseek(f, (sb.data_start + in.direct[i]) * BLOCK_SIZE, SEEK_SET);
            fread(buffer + bytes_read, 1, to_read, f);
            bytes_read += to_read;
        }
    }
    buffer[bytes_read] = '\0';
    fclose(f);
    return buffer;
}

void vsfs_info(const char* disk_name) {
    FILE *f = fopen(disk_name, "rb");
    if (!f) { printf("Erro ao abrir disco.\n"); return; }
    struct superblock sb;
    fread(&sb, sizeof(struct superblock), 1, f);
    printf("--- VSFS Superbloco Info ---\n");
    printf("Magic Number: 0x%X\n", sb.magic);
    printf("Max Inodes: %d\n", sb.num_inodes);
    printf("Max Blocos de Dados: %d\n", sb.num_data_blocks);
    fclose(f);
}

void vsfs_list(const char* disk_name) {
    FILE *f = fopen(disk_name, "rb");
    if (!f) { printf("Erro ao abrir disco.\n"); return; }
    struct superblock sb;
    fread(&sb, sizeof(struct superblock), 1, f);
    
    fseek(f, sb.inode_bitmap_start * BLOCK_SIZE, SEEK_SET);
    unsigned char ibmap[BLOCK_SIZE];
    fread(ibmap, 1, BLOCK_SIZE, f);
    
    printf("--- VSFS Arquivos (Inodes) ---\n");
    for(int i=0; i<sb.num_inodes; i++) {
        if(get_bit(ibmap[i/8], i%8)) {
            struct inode in;
            read_inode(f, &sb, i, &in);
            printf("Inode %d: Tamanho %d bytes\n", i, in.size);
        }
    }
    fclose(f);
}
