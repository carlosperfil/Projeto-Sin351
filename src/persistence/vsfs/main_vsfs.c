#include "vsfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <comando> [argumentos]\n", argv[0]);
        printf("Comandos:\n");
        printf("  format                  Formata o disco.img\n");
        printf("  write <string>          Escreve string como arquivo\n");
        printf("  read <inumber>          Lê arquivo pelo inode\n");
        printf("  info                    Mostra informacoes do superbloco\n");
        printf("  list                    Lista inodes alocados\n");
        return 1;
    }

    const char* disk = "disco.img";

    if (strcmp(argv[1], "format") == 0) {
        if(vsfs_format(disk) == 0) {
            printf("Disco '%s' formatado com sucesso.\n", disk);
        } else {
            printf("Erro ao formatar.\n");
        }
    } 
    else if (strcmp(argv[1], "write") == 0) {
        if (argc < 3) { printf("Forneca a string para escrever.\n"); return 1; }
        const char* text = argv[2];
        int inumber = write_file(disk, text, strlen(text));
        if (inumber >= 0) {
            printf("Arquivo gravado no Inode %d.\n", inumber);
        } else {
            printf("Erro ao gravar arquivo (disco cheio?).\n");
        }
    }
    else if (strcmp(argv[1], "read") == 0) {
        if (argc < 3) { printf("Forneca o inumber.\n"); return 1; }
        int inumber = atoi(argv[2]);
        char* content = read_file(disk, inumber);
        if (content) {
            printf("Conteudo (Inode %d):\n%s\n", inumber, content);
            free(content);
        } else {
            printf("Erro ao ler inode %d.\n", inumber);
        }
    }
    else if (strcmp(argv[1], "info") == 0) {
        vsfs_info(disk);
    }
    else if (strcmp(argv[1], "list") == 0) {
        vsfs_list(disk);
    }
    else {
        printf("Comando desconhecido.\n");
    }

    return 0;
}
