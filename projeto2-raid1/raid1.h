#ifndef ESPELHAMENTO_RAID1_H
#define ESPELHAMENTO_RAID1_H

#include <stdint.h>

#define TAMANHO_BLOCO_RAID 4096
#define TOTAL_BLOCOS_RAID 256
#define QTD_DISCOS_FISICOS 2

typedef enum {
    STATUS_PENDENTE = 0,
    STATUS_EFETIVADO = 1
} status_transacao_t;

struct registro_log {
    int id_transacao;
    int id_bloco;
    status_transacao_t status;
    char conteudo[TAMANHO_BLOCO_RAID];
};

struct inode_espelhado {
    int id_arquivo;
    int bloco_logico; // mapeia para os mesmos offsets fisicos
    int tamanho_bytes;
    int tipo_arquivo;
};

int inicializar_espelhamento(void);
int gravar_dado_espelhado(int id_bloco, const char* texto, int tamanho);
char* ler_dado_fisico(int id_bloco, int num_disco);
int verificar_integridade(void);
int recuperar_falhas_log(void);
void exibir_saude_raid(void);

#endif
