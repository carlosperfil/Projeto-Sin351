#ifndef SISTEMA_ARQUIVOS_SIMPLES_H
#define SISTEMA_ARQUIVOS_SIMPLES_H

#include <stdint.h>

#define TAMANHO_BLOCO 4096
#define ASSINATURA_DISCO 0x12345678
#define TOTAL_BLOCOS 256
#define BLOCOS_PARA_DESCRITORES 5
#define MAX_PONTEIROS_DIRETOS 5

struct super_bloco {
    int assinatura;
    int max_descritores;
    int max_blocos_dados;
    int inicio_mapa_descritores;
    int inicio_mapa_dados;
    int inicio_tabela_descritores;
    int inicio_area_dados;
};

struct descritor_arq {
    int tamanho_bytes;
    int tipo_arquivo; // 1 = regular, 2 = pasta
    int pont_diretos[MAX_PONTEIROS_DIRETOS];
    int pont_indireto;
};

// Protótipos das funções principais
int formatar_disco_virtual(const char* nome_disco);
int gravar_arquivo(const char* nome_disco, const char* conteudo, int tamanho);
char* ler_arquivo(const char* nome_disco, int id_descritor);
void exibir_info_disco(const char* nome_disco);
void listar_arquivos_salvos(const char* nome_disco);

#endif
