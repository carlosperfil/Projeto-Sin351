#include "cofre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#define ASSINATURA_COFRE 0x5641554C // 'VAUL'

// Sobrescreve a memoria volatil para evitar que senhas fiquem na RAM
void limpar_memoria_sensivel(void* endereco_memoria, size_t capacidade) {
    volatile unsigned char* ponteiro = (volatile unsigned char*) endereco_memoria;
    for (size_t iter = 0; iter < capacidade; iter++) {
        ponteiro[iter] = 0;
    }
}

// Derivador de chave via PBKDF2
static void gerar_chave_mestra_pbkdf2(const char* senha_digitada, const char* sal, unsigned char* chave_saida) {
    PKCS5_PBKDF2_HMAC(senha_digitada, strlen(senha_digitada), 
                      (unsigned char*)sal, TAMANHO_SAL,
                      10000, EVP_sha256(), TAMANHO_CHAVE, chave_saida);
}

int criar_novo_cofre(const char* arquivo, const char* senha_mestra) {
    FILE *arq = fopen(arquivo, "wb");
    if (arq == NULL) return -1;

    struct cabecalho_cofre cabecalho;
    cabecalho.identificador_magico = ASSINATURA_COFRE;
    cabecalho.quantidade_registros = 0;
    
    // Gera um sal aleatorio de 32 bytes
    RAND_bytes((unsigned char*)cabecalho.sal_aleatorio, TAMANHO_SAL);
    
    unsigned char chave_temporaria[TAMANHO_CHAVE];
    gerar_chave_mestra_pbkdf2(senha_mestra, cabecalho.sal_aleatorio, chave_temporaria);
    
    // Hash da chave derivada para validacao futura sem descriptografar os dados
    SHA256(chave_temporaria, TAMANHO_CHAVE, cabecalho.hash_autenticacao);
    
    fwrite(&cabecalho, sizeof(struct cabecalho_cofre), 1, arq);
    fclose(arq);
    
    limpar_memoria_sensivel(chave_temporaria, TAMANHO_CHAVE);
    return 0;
}

int validar_acesso_cofre(const char* arquivo, const char* senha_mestra, unsigned char* chave_gerada) {
    FILE *arq = fopen(arquivo, "rb");
    if (!arq) return -1;
    
    struct cabecalho_cofre cabecalho;
    int leu_cabecalho = fread(&cabecalho, sizeof(struct cabecalho_cofre), 1, arq);
    fclose(arq);
    
    if (leu_cabecalho != 1 || cabecalho.identificador_magico != ASSINATURA_COFRE) {
        return -1;
    }
    
    gerar_chave_mestra_pbkdf2(senha_mestra, cabecalho.sal_aleatorio, chave_gerada);
    
    unsigned char hash_teste[32];
    SHA256(chave_gerada, TAMANHO_CHAVE, hash_teste);
    
    // Compara o hash derivado com o que esta salvo no arquivo
    if (CRYPTO_memcmp(cabecalho.hash_autenticacao, hash_teste, 32) != 0) {
        limpar_memoria_sensivel(chave_gerada, TAMANHO_CHAVE); // Seguranca: falhou, apaga chave
        return -1; // Falhou a validacao
    }
    
    return cabecalho.quantidade_registros; // Autenticacao com sucesso
}

int salvar_senhas_disco(const char* arquivo, const unsigned char* chave, struct registro_senha* vetor_registros, int qtd_atual) {
    FILE *arq = fopen(arquivo, "r+b");
    if (!arq) return -1;
    
    struct cabecalho_cofre cabecalho;
    fread(&cabecalho, sizeof(struct cabecalho_cofre), 1, arq);
    cabecalho.quantidade_registros = qtd_atual;
    
    // Atualiza o cabecalho
    fseek(arq, 0, SEEK_SET);
    fwrite(&cabecalho, sizeof(struct cabecalho_cofre), 1, arq);
    
    if (qtd_atual > 0) {
        unsigned char vetor_init[TAMANHO_VETOR_INICIALIZACAO];
        RAND_bytes(vetor_init, TAMANHO_VETOR_INICIALIZACAO);
        fwrite(vetor_init, 1, TAMANHO_VETOR_INICIALIZACAO, arq);
        
        EVP_CIPHER_CTX *contexto_aes = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(contexto_aes, EVP_aes_256_cbc(), NULL, chave, vetor_init);
        
        int tamanho_bytes_entrada = qtd_atual * sizeof(struct registro_senha);
        int bytes_processados;
        int tamanho_total_cifrado = 0;
        
        unsigned char* dados_cifrados = (unsigned char*) malloc(tamanho_bytes_entrada + EVP_MAX_BLOCK_LENGTH);
        
        EVP_EncryptUpdate(contexto_aes, dados_cifrados, &bytes_processados, (unsigned char*)vetor_registros, tamanho_bytes_entrada);
        tamanho_total_cifrado += bytes_processados;
        
        EVP_EncryptFinal_ex(contexto_aes, dados_cifrados + bytes_processados, &bytes_processados);
        tamanho_total_cifrado += bytes_processados;
        
        fwrite(dados_cifrados, 1, tamanho_total_cifrado, arq);
        
        free(dados_cifrados);
        EVP_CIPHER_CTX_free(contexto_aes);
    }
    
    fclose(arq);
    return 0;
}

struct registro_senha* carregar_senhas_memoria(const char* arquivo, const unsigned char* chave, int* qtd_atual) {
    FILE *arq = fopen(arquivo, "rb");
    if (!arq) return NULL;
    
    struct cabecalho_cofre cabecalho;
    fread(&cabecalho, sizeof(struct cabecalho_cofre), 1, arq);
    *qtd_atual = cabecalho.quantidade_registros;
    
    if (*qtd_atual <= 0) {
        fclose(arq);
        return (struct registro_senha*) malloc(sizeof(struct registro_senha) * LIMITE_SENHAS);
    }
    
    unsigned char vetor_init[TAMANHO_VETOR_INICIALIZACAO];
    fread(vetor_init, 1, TAMANHO_VETOR_INICIALIZACAO, arq);
    
    fseek(arq, 0, SEEK_END);
    int tamanho_total_arquivo = ftell(arq);
    int tamanho_bloco_cifrado = tamanho_total_arquivo - sizeof(struct cabecalho_cofre) - TAMANHO_VETOR_INICIALIZACAO;
    
    fseek(arq, sizeof(struct cabecalho_cofre) + TAMANHO_VETOR_INICIALIZACAO, SEEK_SET);
    unsigned char* pacote_cifrado = (unsigned char*) malloc(tamanho_bloco_cifrado);
    fread(pacote_cifrado, 1, tamanho_bloco_cifrado, arq);
    fclose(arq);
    
    struct registro_senha* registros_decifrados = (struct registro_senha*) malloc(sizeof(struct registro_senha) * LIMITE_SENHAS);
    
    EVP_CIPHER_CTX *contexto_aes = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(contexto_aes, EVP_aes_256_cbc(), NULL, chave, vetor_init);
    
    int bytes_processados;
    int tamanho_texto_limpo = 0;
    
    EVP_DecryptUpdate(contexto_aes, (unsigned char*)registros_decifrados, &bytes_processados, pacote_cifrado, tamanho_bloco_cifrado);
    tamanho_texto_limpo += bytes_processados;
    
    EVP_DecryptFinal_ex(contexto_aes, (unsigned char*)registros_decifrados + bytes_processados, &bytes_processados);
    tamanho_texto_limpo += bytes_processados;
    
    EVP_CIPHER_CTX_free(contexto_aes);
    free(pacote_cifrado);
    
    return registros_decifrados;
}
