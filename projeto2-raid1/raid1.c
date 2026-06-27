#include "raid1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* NOME_DISCO_0 = "disco_fisico_0.bin";
static const char* NOME_DISCO_1 = "disco_fisico_1.bin";
static const char* ARQUIVO_LOG = "registro_transacoes.bin";

int inicializar_espelhamento(void) {
    char bloco_zerado[TAMANHO_BLOCO_RAID];
    memset(bloco_zerado, 0, TAMANHO_BLOCO_RAID);
    
    FILE *disco0 = fopen(NOME_DISCO_0, "wb");
    FILE *disco1 = fopen(NOME_DISCO_1, "wb");
    
    if (disco0 == NULL || disco1 == NULL) {
        return -1;
    }
    
    for (int idx = 0; idx < TOTAL_BLOCOS_RAID; idx++) {
        fwrite(bloco_zerado, 1, TAMANHO_BLOCO_RAID, disco0);
        fwrite(bloco_zerado, 1, TAMANHO_BLOCO_RAID, disco1);
    }
    
    fclose(disco0);
    fclose(disco1);
    
    // Zera o log também
    FILE *log_arq = fopen(ARQUIVO_LOG, "wb");
    if (log_arq != NULL) {
        fclose(log_arq);
    }
    
    return 0;
}

static int registrar_intencao_log(int transacao, int bloco, const char* texto, int tam) {
    FILE *arq_log = fopen(ARQUIVO_LOG, "ab");
    if (arq_log == NULL) return -1;
    
    struct registro_log novo_registro;
    memset(&novo_registro, 0, sizeof(struct registro_log));
    
    novo_registro.id_transacao = transacao;
    novo_registro.id_bloco = bloco;
    novo_registro.status = STATUS_PENDENTE;
    
    int limite = (tam > TAMANHO_BLOCO_RAID) ? TAMANHO_BLOCO_RAID : tam;
    memcpy(novo_registro.conteudo, texto, limite);
    
    fwrite(&novo_registro, sizeof(struct registro_log), 1, arq_log);
    fclose(arq_log);
    return 0;
}

static int confirmar_transacao_log(int transacao) {
    FILE *arq_log = fopen(ARQUIVO_LOG, "r+b");
    if (!arq_log) return -1;
    
    struct registro_log reg;
    int transacao_encontrada = 0;
    
    while (fread(&reg, sizeof(struct registro_log), 1, arq_log) == 1) {
        if (reg.id_transacao == transacao && reg.status == STATUS_PENDENTE) {
            reg.status = STATUS_EFETIVADO;
            
            // Volta o ponteiro para sobrescrever a struct correta
            fseek(arq_log, -((long)sizeof(struct registro_log)), SEEK_CUR);
            fwrite(&reg, sizeof(struct registro_log), 1, arq_log);
            transacao_encontrada = 1;
            break;
        }
    }
    
    fclose(arq_log);
    
    if (transacao_encontrada) {
        return 0;
    } else {
        return -1;
    }
}

static int escrever_direto_no_disco(const char* caminho, int bloco, const char* texto, int tam) {
    FILE *disco = fopen(caminho, "r+b");
    if (disco == NULL) return -1;
    
    long offset = bloco * TAMANHO_BLOCO_RAID;
    fseek(disco, offset, SEEK_SET);
    
    int limite = (tam > TAMANHO_BLOCO_RAID) ? TAMANHO_BLOCO_RAID : tam;
    fwrite(texto, 1, limite, disco);
    
    fclose(disco);
    return 0;
}

int gravar_dado_espelhado(int id_bloco, const char* texto, int tamanho) {
    // Simulador de gerador de IDs de transação
    static int gerador_tx = 1;
    int tx_atual = gerador_tx;
    gerador_tx++;
    
    // Passo 1: Anotar no Log (Write-Ahead Logging)
    if (registrar_intencao_log(tx_atual, id_bloco, texto, tamanho) != 0) {
        return -1;
    }
    
    // Passo 2: Sincronizar em disco físico 0 e 1 (Mirror)
    int falha0 = escrever_direto_no_disco(NOME_DISCO_0, id_bloco, texto, tamanho);
    int falha1 = escrever_direto_no_disco(NOME_DISCO_1, id_bloco, texto, tamanho);
    
    if (falha0 != 0 || falha1 != 0) {
        return -1; // Caiu aqui significa que o commit não será feito e será recuperado depois
    }
    
    // Passo 3: Efetivar no Log
    confirmar_transacao_log(tx_atual);
    return 0;
}

char* ler_dado_fisico(int id_bloco, int num_disco) {
    const char* disco_alvo = (num_disco == 0) ? NOME_DISCO_0 : NOME_DISCO_1;
    
    FILE *arq = fopen(disco_alvo, "rb");
    if (!arq) return NULL;
    
    char* texto_recuperado = (char*) malloc(TAMANHO_BLOCO_RAID + 1);
    
    long offset = id_bloco * TAMANHO_BLOCO_RAID;
    fseek(arq, offset, SEEK_SET);
    
    int bytes = fread(texto_recuperado, 1, TAMANHO_BLOCO_RAID, arq);
    if (bytes <= 0) {
        free(texto_recuperado);
        fclose(arq);
        return NULL;
    }
    
    texto_recuperado[bytes] = '\0';
    fclose(arq);
    return texto_recuperado;
}

int verificar_integridade(void) {
    FILE *d0 = fopen(NOME_DISCO_0, "rb");
    FILE *d1 = fopen(NOME_DISCO_1, "rb");
    
    if (d0 == NULL || d1 == NULL) {
        if(d0) fclose(d0);
        if(d1) fclose(d1);
        return -1;
    }
    
    int qtd_erros = 0;
    char buffer_d0[TAMANHO_BLOCO_RAID];
    char buffer_d1[TAMANHO_BLOCO_RAID];
    
    for (int w = 0; w < TOTAL_BLOCOS_RAID; w++) {
        fread(buffer_d0, 1, TAMANHO_BLOCO_RAID, d0);
        fread(buffer_d1, 1, TAMANHO_BLOCO_RAID, d1);
        
        // Verifica se os espelhos sao identicos byte a byte
        if (memcmp(buffer_d0, buffer_d1, TAMANHO_BLOCO_RAID) != 0) {
            printf("Aviso: Dados incompativeis detectados no bloco %d!\n", w);
            qtd_erros++;
        }
    }
    
    fclose(d0);
    fclose(d1);
    return qtd_erros;
}

int recuperar_falhas_log(void) {
    FILE *arq_log = fopen(ARQUIVO_LOG, "rb");
    if (arq_log == NULL) return -1;
    
    struct registro_log reg;
    int operacoes_refeitas = 0;
    
    while (fread(&reg, sizeof(struct registro_log), 1, arq_log) == 1) {
        // Apenas transacoes confirmadas devem ser reaplicadas para evitar sujeira
        if (reg.status == STATUS_EFETIVADO) {
            escrever_direto_no_disco(NOME_DISCO_0, reg.id_bloco, reg.conteudo, TAMANHO_BLOCO_RAID);
            escrever_direto_no_disco(NOME_DISCO_1, reg.id_bloco, reg.conteudo, TAMANHO_BLOCO_RAID);
            operacoes_refeitas++;
        }
    }
    
    fclose(arq_log);
    return operacoes_refeitas;
}

void exibir_saude_raid(void) {
    FILE *d0 = fopen(NOME_DISCO_0, "rb");
    FILE *d1 = fopen(NOME_DISCO_1, "rb");
    
    printf("--- RELATORIO DE SAUDE DO RAID-1 ---\n");
    if (d0 != NULL) { 
        printf("Disco Físico 0: [ATIVO]\n"); 
        fclose(d0); 
    } else { 
        printf("Disco Físico 0: [FALHA]\n"); 
    }
    
    if (d1 != NULL) { 
        printf("Disco Físico 1: [ATIVO]\n"); 
        fclose(d1); 
    } else { 
        printf("Disco Físico 1: [FALHA]\n"); 
    }
    
    FILE *arq_log = fopen(ARQUIVO_LOG, "rb");
    if (arq_log != NULL) { 
        int cont_pendentes = 0;
        int cont_concluidos = 0;
        struct registro_log reg;
        
        while (fread(&reg, sizeof(struct registro_log), 1, arq_log) == 1) {
            if (reg.status == STATUS_PENDENTE) {
                cont_pendentes++;
            } else if (reg.status == STATUS_EFETIVADO) {
                cont_concluidos++;
            }
        }
        printf("Log WAL: %d transacoes esperando e %d concluidas.\n", cont_pendentes, cont_concluidos);
        fclose(arq_log);
    } else {
        printf("Log WAL: Arquivo de log não foi encontrado.\n");
    }
}
