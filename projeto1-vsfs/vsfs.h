#ifndef VSFS_H
#define VSFS_H

#include <stdint.h>

#define BLOCK_SIZE 4096
#define MAGIC_NUMBER 0x12345678
#define NUM_BLOCKS 256
#define NUM_INODE_BLOCKS 5
#define NUM_DIRECT_PTRS 5

struct superblock {
    int magic;
    int num_inodes;
    int num_data_blocks;
    int inode_bitmap_start;
    int data_bitmap_start;
    int inode_table_start;
    int data_start;
};

struct inode {
    int size;          // tamanho em bytes
    int type;          // arquivo regular (1) ou diretorio (2)
    int direct[NUM_DIRECT_PTRS]; // ponteiros diretos para blocos de dados
    int indirect;      // ponteiro indireto
};

// Protótipos
int vsfs_format(const char* disk_name);
int write_file(const char* disk_name, const char* content, int size);
char* read_file(const char* disk_name, int inumber);
void vsfs_info(const char* disk_name);
void vsfs_list(const char* disk_name);

#endif // VSFS_H
