#ifndef COFRE_SENHAS_H
#define COFRE_SENHAS_H

#include <stdint.h>
#include <stddef.h>

#define LIMITE_SENHAS 100
#define TAMANHO_SAL 32
#define TAMANHO_CHAVE 32 // 256 bits para AES
#define TAMANHO_VETOR_INICIALIZACAO 16

struct registro_senha {
    char nome_site[128];
    char nome_usuario[64];
    char senha_texto[128];
};

struct cabecalho_cofre {
    int identificador_magico;
    char sal_aleatorio[TAMANHO_SAL];
    unsigned char hash_autenticacao[32]; // SHA256 da chave mestra
    int quantidade_registros;
};

// Funcoes de Autenticacao
int criar_novo_cofre(const char* arquivo, const char* senha_mestra);
int validar_acesso_cofre(const char* arquivo, const char* senha_mestra, unsigned char* chave_gerada);

// Gerenciamento dos Dados
struct registro_senha* carregar_senhas_memoria(const char* arquivo, const unsigned char* chave, int* qtd_atual);
int salvar_senhas_disco(const char* arquivo, const unsigned char* chave, struct registro_senha* vetor_registros, int qtd_atual);

// Protecao de Memoria
void limpar_memoria_sensivel(void* endereco_memoria, size_t capacidade);

#endif
