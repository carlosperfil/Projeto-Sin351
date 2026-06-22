#ifndef COFRE_H
#define COFRE_H

#include <stdint.h>

#define MAX_ENTRIES 100
#define SALT_SIZE 32
#define KEY_SIZE 32 // 256 bits for AES
#define IV_SIZE 16

struct entry {
    char site[128];
    char username[64];
    char password[128];
};

struct vault_header {
    int magic;
    char salt[SALT_SIZE];
    unsigned char master_hash[32]; // SHA256 of master pass
    int num_entries;
};

// Autenticacao
int vault_create(const char* file, const char* master_pass);
int vault_authenticate(const char* file, const char* master_pass, unsigned char* derived_key);

// Gerenciamento
struct entry* vault_load(const char* file, const unsigned char* key, int* count);
int vault_save(const char* file, const unsigned char* key, struct entry* entries, int count);

// Wipe de seguranca
void secure_wipe(void* mem, size_t size);

#endif
