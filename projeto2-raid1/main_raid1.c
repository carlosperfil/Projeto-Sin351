#include "raid1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Como usar: %s <operacao> [argumentos]\n", argv[0]);
        printf("Operacoes disponiveis:\n");
        printf("  formatar                    -> Cria os discos e o arquivo de log do zero\n");
        printf("  gravar <bloco> <texto>      -> Salva os dados de forma espelhada\n");
        printf("  ler <bloco> <disco>         -> Visualiza dados crús do disco 0 ou 1\n");
        printf("  auditoria                   -> Checa se os dois discos estao 100%% iguais\n");
        printf("  recuperar                   -> Roda o log WAL para consertar problemas\n");
        printf("  status                      -> Mostra se os discos estao online\n");
        return 1;
    }

    if (strcmp(argv[1], "formatar") == 0) {
        if (inicializar_espelhamento() == 0) {
            printf("RAID-1 inicializado. Discos criados.\n");
        } else {
            printf("Erro critico ao criar arquivos de disco.\n");
        }
    } 
    else if (strcmp(argv[1], "gravar") == 0) {
        if (argc < 4) { 
            printf("Esta faltando argumentos. Ex: gravar 5 \"texto aqui\"\n"); 
            return 1; 
        }
        int identificador_bloco = atoi(argv[2]);
        char* texto = argv[3];
        
        if (gravar_dado_espelhado(identificador_bloco, texto, strlen(texto)) == 0) {
            printf("Transacao concluida! Gravado em ambos os discos no bloco %d.\n", identificador_bloco);
        } else {
            printf("Houve uma falha transacional (pendente no Log).\n");
        }
    }
    else if (strcmp(argv[1], "ler") == 0) {
        if (argc < 4) { 
            printf("Forneça o bloco e o numero do disco fisico (0 ou 1).\n"); 
            return 1; 
        }
        int identificador_bloco = atoi(argv[2]);
        int qual_disco = atoi(argv[3]);
        
        char *texto_lido = ler_dado_fisico(identificador_bloco, qual_disco);
        if (texto_lido != NULL) {
            printf("=================================\n");
            printf("Dados (Disco Físico %d, Bloco %d):\n%s\n", qual_disco, identificador_bloco, texto_lido);
            printf("=================================\n");
            free(texto_lido);
        } else {
            printf("Nao foi possivel ler os dados.\n");
        }
    }
    else if (strcmp(argv[1], "auditoria") == 0) {
        int divergencias = verificar_integridade();
        if (divergencias == 0) {
            printf("Array 100%% Saudavel! Nenhuma corrupção detectada entre os discos.\n");
        } else {
            printf("ALERTA GRAVE: %d blocos nao batem entre os discos!\n", divergencias);
        }
    }
    else if (strcmp(argv[1], "recuperar") == 0) {
        int recuperados = recuperar_falhas_log();
        printf("Acao de recuperacao finalizada. %d transacoes foram garantidas.\n", recuperados);
    }
    else if (strcmp(argv[1], "status") == 0) {
        exibir_saude_raid();
    }
    else {
        printf("Comando não reconhecido.\n");
    }

    return 0;
}
