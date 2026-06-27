#include "cofre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#define VAULT_MAGIC 0x5641554C // 'VAUL'

// Implementacao dummy para ambiente nao-otimizado (em prod usar memset_s ou pragma nativo)
void secure_wipe(void* mem, size_t size) {
    volatile unsigned char* p = (volatile unsigned char*)mem;
    while (size--) *p++ = 0;
}

static void derive_key(const char* pass, const char* salt, unsigned char* out_key) {
    PKCS5_PBKDF2_HMAC(pass, strlen(pass), (unsigned char*)salt, SALT_SIZE,
                      10000, EVP_sha256(), KEY_SIZE, out_key);
}

int vault_create(const char* file, const char* master_pass) {
    FILE *f = fopen(file, "wb");
    if (!f) return -1;

    struct vault_header h;
    h.magic = VAULT_MAGIC;
    h.num_entries = 0;
    
    RAND_bytes((unsigned char*)h.salt, SALT_SIZE);
    
    unsigned char master_key[KEY_SIZE];
    derive_key(master_pass, h.salt, master_key);
    
    // Hash da propria chave pra comparar depois
    SHA256(master_key, KEY_SIZE, h.master_hash);
    
    fwrite(&h, sizeof(h), 1, f);
    fclose(f);
    
    secure_wipe(master_key, KEY_SIZE);
    return 0;
}

int vault_authenticate(const char* file, const char* master_pass, unsigned char* derived_key) {
    FILE *f = fopen(file, "rb");
    if (!f) return -1;
    
    struct vault_header h;
    if (fread(&h, sizeof(h), 1, f) != 1 || h.magic != VAULT_MAGIC) {
        fclose(f); return -1;
    }
    fclose(f);
    
    derive_key(master_pass, h.salt, derived_key);
    
    unsigned char check_hash[32];
    SHA256(derived_key, KEY_SIZE, check_hash);
    
    if (CRYPTO_memcmp(h.master_hash, check_hash, 32) != 0) {
        secure_wipe(derived_key, KEY_SIZE);
        return -1; // Autenticacao falhou
    }
    return h.num_entries; // Sucesso
}

int vault_save(const char* file, const unsigned char* key, struct entry* entries, int count) {
    FILE *f = fopen(file, "r+b");
    if (!f) return -1;
    
    struct vault_header h;
    fread(&h, sizeof(h), 1, f);
    h.num_entries = count;
    fseek(f, 0, SEEK_SET);
    fwrite(&h, sizeof(h), 1, f);
    
    if (count > 0) {
        unsigned char iv[IV_SIZE];
        RAND_bytes(iv, IV_SIZE);
        fwrite(iv, 1, IV_SIZE, f);
        
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
        
        int in_len = count * sizeof(struct entry);
        int out_len, ciphertext_len = 0;
        unsigned char* ciphertext = malloc(in_len + EVP_MAX_BLOCK_LENGTH);
        
        EVP_EncryptUpdate(ctx, ciphertext, &out_len, (unsigned char*)entries, in_len);
        ciphertext_len += out_len;
        EVP_EncryptFinal_ex(ctx, ciphertext + out_len, &out_len);
        ciphertext_len += out_len;
        
        fwrite(ciphertext, 1, ciphertext_len, f);
        
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
    }
    fclose(f);
    return 0;
}

struct entry* vault_load(const char* file, const unsigned char* key, int* count) {
    FILE *f = fopen(file, "rb");
    if (!f) return NULL;
    
    struct vault_header h;
    fread(&h, sizeof(h), 1, f);
    *count = h.num_entries;
    
    if (*count <= 0) {
        fclose(f);
        return malloc(sizeof(struct entry) * MAX_ENTRIES);
    }
    
    unsigned char iv[IV_SIZE];
    fread(iv, 1, IV_SIZE, f);
    
    fseek(f, 0, SEEK_END);
    int total_sz = ftell(f);
    int cipher_sz = total_sz - sizeof(h) - IV_SIZE;
    
    fseek(f, sizeof(h) + IV_SIZE, SEEK_SET);
    unsigned char* ciphertext = malloc(cipher_sz);
    fread(ciphertext, 1, cipher_sz, f);
    fclose(f);
    
    struct entry* entries = malloc(sizeof(struct entry) * MAX_ENTRIES);
    
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    
    int out_len, plaintext_len = 0;
    EVP_DecryptUpdate(ctx, (unsigned char*)entries, &out_len, ciphertext, cipher_sz);
    plaintext_len += out_len;
    EVP_DecryptFinal_ex(ctx, (unsigned char*)entries + out_len, &out_len);
    plaintext_len += out_len;
    
    EVP_CIPHER_CTX_free(ctx);
    free(ciphertext);
    
    return entries;
}
