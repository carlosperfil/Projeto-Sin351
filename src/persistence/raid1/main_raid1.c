#include "raid1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <comando> [argumentos]\n", argv[0]);
        printf("Comandos:\n");
        printf("  init                      Formata os dois discos e log\n");
        printf("  write <block_id> <texto>  Grava dado em ambos os discos\n");
        printf("  read <block_id> <disk>    Lê dado de um bloco de um disco (0 ou 1)\n");
        printf("  verify                    Verifica integridade entre os discos\n");
        printf("  recover                   Reaplica o WAL Log para correcao\n");
        printf("  status                    Status de saude da array RAID\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        if(raid1_init() == 0) printf("RAID-1 formatado e pronto.\n");
        else printf("Erro ao iniciar.\n");
    } 
    else if (strcmp(argv[1], "write") == 0) {
        if (argc < 4) { printf("Forneca bloco e string.\n"); return 1; }
        int b_id = atoi(argv[2]);
        if(raid1_write(b_id, argv[3], strlen(argv[3])) == 0) {
            printf("Sucesso! Escrita espelhada em disk0 e disk1 no bloco %d.\n", b_id);
        } else {
            printf("Falha na transacao!\n");
        }
    }
    else if (strcmp(argv[1], "read") == 0) {
        if (argc < 4) { printf("Forneca bloco e disco (0 ou 1).\n"); return 1; }
        int b_id = atoi(argv[2]);
        int disk = atoi(argv[3]);
        char *d = raid1_read(b_id, disk);
        if(d) {
            printf("Conteudo (Disco %d, Bloco %d):\n%s\n", disk, b_id, d);
            free(d);
        } else {
            printf("Erro de leitura.\n");
        }
    }
    else if (strcmp(argv[1], "verify") == 0) {
        int errs = raid1_verify();
        if(errs == 0) printf("Array Saudavel! Sem divergencias.\n");
        else printf("ALERTA: %d bloco(s) corrompido(s) ou divergente(s)!\n", errs);
    }
    else if (strcmp(argv[1], "recover") == 0) {
        int c = raid1_recover();
        printf("Recuperacao finalizada. %d transacoes garantidas.\n", c);
    }
    else if (strcmp(argv[1], "status") == 0) {
        raid1_status();
    }
    else {
        printf("Comando desconhecido.\n");
    }

    return 0;
}
