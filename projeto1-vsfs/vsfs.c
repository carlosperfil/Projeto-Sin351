#include "vsfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funcao auxiliar para checar se um bit esta livre (0) ou ocupado (1)
static int checar_bit(unsigned char byte, int posicao) {
    return (byte & (1 << posicao)) != 0;
}

// Funcao auxiliar para marcar um bit como ocupado (1)
static void ligar_bit(unsigned char *byte, int posicao) {
    *byte = *byte | (1 << posicao);
}

int formatar_disco_virtual(const char* nome_disco) {
    FILE *arquivo_disco = fopen(nome_disco, "wb");
    if (arquivo_disco == NULL) {
        return -1;
    }
    
    // Preenche o disco inteiro com zeros
    char bloco_vazio[TAMANHO_BLOCO];
    memset(bloco_vazio, 0, TAMANHO_BLOCO); // Forma diferente de zerar
    
    for (int idx = 0; idx < TOTAL_BLOCOS; idx++) {
        fwrite(bloco_vazio, 1, TAMANHO_BLOCO, arquivo_disco);
    }
    
    // Configura o super bloco na posicao 0
    struct super_bloco sb;
    sb.assinatura = ASSINATURA_DISCO;
    sb.max_descritores = (BLOCOS_PARA_DESCRITORES * TAMANHO_BLOCO) / sizeof(struct descritor_arq);
    sb.max_blocos_dados = TOTAL_BLOCOS - 3 - BLOCOS_PARA_DESCRITORES;
    sb.inicio_mapa_descritores = 1;
    sb.inicio_mapa_dados = 2;
    sb.inicio_tabela_descritores = 3;
    sb.inicio_area_dados = 3 + BLOCOS_PARA_DESCRITORES;
    
    // Volta pro comeco e salva
    fseek(arquivo_disco, 0, SEEK_SET);
    fwrite(&sb, sizeof(struct super_bloco), 1, arquivo_disco);
    fclose(arquivo_disco);
    
    return 0;
}

// Alocador simples que varre o mapa de bits
static int reservar_bloco_livre(FILE *arquivo_disco, int bloco_mapa, int limite_bits) {
    unsigned char buffer_mapa[TAMANHO_BLOCO];
    
    fseek(arquivo_disco, bloco_mapa * TAMANHO_BLOCO, SEEK_SET);
    fread(buffer_mapa, 1, TAMANHO_BLOCO, arquivo_disco);
    
    for (int bit_atual = 0; bit_atual < limite_bits; bit_atual++) {
        int indice_byte = bit_atual / 8;
        int bit_no_byte = bit_atual % 8;
        
        // Se o bit é zero, achamos um espaco livre
        if (!checar_bit(buffer_mapa[indice_byte], bit_no_byte)) {
            ligar_bit(&buffer_mapa[indice_byte], bit_no_byte);
            
            // Salva a alteracao no disco
            fseek(arquivo_disco, bloco_mapa * TAMANHO_BLOCO, SEEK_SET);
            fwrite(buffer_mapa, 1, TAMANHO_BLOCO, arquivo_disco);
            
            return bit_atual; // Retorna o ID alocado
        }
    }
    return -1; // Disco cheio
}

static void recuperar_descritor(FILE *arq, struct super_bloco *sb, int id, struct descritor_arq *desc) {
    int endereco_fisico = (sb->inicio_tabela_descritores * TAMANHO_BLOCO) + (id * sizeof(struct descritor_arq));
    fseek(arq, endereco_fisico, SEEK_SET);
    fread(desc, sizeof(struct descritor_arq), 1, arq);
}

static void salvar_descritor(FILE *arq, struct super_bloco *sb, int id, struct descritor_arq *desc) {
    int endereco_fisico = (sb->inicio_tabela_descritores * TAMANHO_BLOCO) + (id * sizeof(struct descritor_arq));
    fseek(arq, endereco_fisico, SEEK_SET);
    fwrite(desc, sizeof(struct descritor_arq), 1, arq);
}

int gravar_arquivo(const char* nome_disco, const char* conteudo, int tamanho) {
    FILE *arq = fopen(nome_disco, "r+b");
    if (!arq) return -1;
    
    struct super_bloco metadados;
    fread(&metadados, sizeof(struct super_bloco), 1, arq);
    
    int id_alocado = reservar_bloco_livre(arq, metadados.inicio_mapa_descritores, metadados.max_descritores);
    if (id_alocado < 0) { 
        fclose(arq); 
        return -1; 
    }
    
    struct descritor_arq novo_arq;
    memset(&novo_arq, 0, sizeof(struct descritor_arq)); // Zera a struct
    
    novo_arq.tamanho_bytes = tamanho;
    novo_arq.tipo_arquivo = 1; // 1 significa arquivo de texto normal
    
    for (int k = 0; k < MAX_PONTEIROS_DIRETOS; k++) {
        novo_arq.pont_diretos[k] = -1;
    }
    novo_arq.pont_indireto = -1;
    
    int total_gravado = 0;
    int ptr_atual = 0;
    
    while (total_gravado < tamanho && ptr_atual < MAX_PONTEIROS_DIRETOS) {
        int id_bloco_dados = reservar_bloco_livre(arq, metadados.inicio_mapa_dados, metadados.max_blocos_dados);
        if (id_bloco_dados < 0) break;
        
        novo_arq.pont_diretos[ptr_atual] = id_bloco_dados;
        ptr_atual++;
        
        int qtd_escrever = tamanho - total_gravado;
        if (qtd_escrever > TAMANHO_BLOCO) {
            qtd_escrever = TAMANHO_BLOCO;
        }
        
        int offset_dados = (metadados.inicio_area_dados + id_bloco_dados) * TAMANHO_BLOCO;
        fseek(arq, offset_dados, SEEK_SET);
        fwrite(conteudo + total_gravado, 1, qtd_escrever, arq);
        
        total_gravado += qtd_escrever;
    }
    
    salvar_descritor(arq, &metadados, id_alocado, &novo_arq);
    fclose(arq);
    
    return id_alocado;
}

char* ler_arquivo(const char* nome_disco, int id_descritor) {
    FILE *arq = fopen(nome_disco, "rb");
    if (arq == NULL) return NULL;
    
    struct super_bloco metadados;
    fread(&metadados, sizeof(struct super_bloco), 1, arq);
    
    struct descritor_arq info_arq;
    recuperar_descritor(arq, &metadados, id_descritor, &info_arq);
    
    if (info_arq.tamanho_bytes == 0) { 
        fclose(arq); 
        return NULL; 
    }
    
    char* texto_saida = (char*) malloc(info_arq.tamanho_bytes + 1);
    int total_lido = 0;
    
    for (int k = 0; k < MAX_PONTEIROS_DIRETOS; k++) {
        if (info_arq.pont_diretos[k] != -1) {
            int qtd_ler = info_arq.tamanho_bytes - total_lido;
            if (qtd_ler > TAMANHO_BLOCO) {
                qtd_ler = TAMANHO_BLOCO;
            }
            
            int offset = (metadados.inicio_area_dados + info_arq.pont_diretos[k]) * TAMANHO_BLOCO;
            fseek(arq, offset, SEEK_SET);
            fread(texto_saida + total_lido, 1, qtd_ler, arq);
            
            total_lido += qtd_ler;
        }
    }
    
    texto_saida[total_lido] = '\0'; // Finaliza a string
    fclose(arq);
    
    return texto_saida;
}

void exibir_info_disco(const char* nome_disco) {
    FILE *arq = fopen(nome_disco, "rb");
    if (!arq) { 
        printf("Falha ao montar o disco virtual.\n"); 
        return; 
    }
    
    struct super_bloco metadados;
    fread(&metadados, sizeof(struct super_bloco), 1, arq);
    
    printf("--- SUPER BLOCO ---\n");
    printf("Assinatura: 0x%X\n", metadados.assinatura);
    printf("Capacidade de Arquivos (Descritores): %d\n", metadados.max_descritores);
    printf("Capacidade de Blocos de Dados: %d\n", metadados.max_blocos_dados);
    fclose(arq);
}

void listar_arquivos_salvos(const char* nome_disco) {
    FILE *arq = fopen(nome_disco, "rb");
    if (!arq) { 
        printf("Falha ao acessar disco virtual.\n"); 
        return; 
    }
    
    struct super_bloco metadados;
    fread(&metadados, sizeof(struct super_bloco), 1, arq);
    
    fseek(arq, metadados.inicio_mapa_descritores * TAMANHO_BLOCO, SEEK_SET);
    unsigned char mapa[TAMANHO_BLOCO];
    fread(mapa, 1, TAMANHO_BLOCO, arq);
    
    printf("--- ARQUIVOS PRESENTES ---\n");
    for (int j = 0; j < metadados.max_descritores; j++) {
        int indice_byte = j / 8;
        int bit_no_byte = j % 8;
        
        if (checar_bit(mapa[indice_byte], bit_no_byte)) {
            struct descritor_arq arq_temp;
            recuperar_descritor(arq, &metadados, j, &arq_temp);
            printf("Arquivo ID %d - Ocupando %d bytes no disco\n", j, arq_temp.tamanho_bytes);
        }
    }
    fclose(arq);
}
