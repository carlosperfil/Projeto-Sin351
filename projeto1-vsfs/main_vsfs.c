#include "vsfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Como usar: %s <operacao> [argumentos]\n", argv[0]);
        printf("Operacoes suportadas:\n");
        printf("  formatar              -> Zera e cria o disco_virtual.img\n");
        printf("  escrever <texto>      -> Salva o texto como um novo arquivo\n");
        printf("  ler <id_arquivo>      -> Exibe o texto de um arquivo especifico\n");
        printf("  status                -> Exibe dados do super bloco\n");
        printf("  listar                -> Mostra arquivos gravados no disco\n");
        return 1;
    }

    const char* arquivo_disco = "disco_virtual.img";

    if (strcmp(argv[1], "formatar") == 0) {
        if (formatar_disco_virtual(arquivo_disco) == 0) {
            printf("Sucesso: '%s' foi formatado e esta pronto.\n", arquivo_disco);
        } else {
            printf("Erro grave ao tentar formatar.\n");
        }
    } 
    else if (strcmp(argv[1], "escrever") == 0) {
        if (argc < 3) { 
            printf("Faltou o texto para salvar. Tente: escrever \"meu texto\"\n"); 
            return 1; 
        }
        int id_recebido = gravar_arquivo(arquivo_disco, argv[2], strlen(argv[2]));
        if (id_recebido >= 0) {
            printf("Feito! Arquivo salvo com o ID = %d.\n", id_recebido);
        } else {
            printf("Falha na gravacao. Disco cheio?\n");
        }
    }
    else if (strcmp(argv[1], "ler") == 0) {
        if (argc < 3) { 
            printf("Faltou o ID do arquivo para ler.\n"); 
            return 1; 
        }
        int identificador = atoi(argv[2]);
        char* texto = ler_arquivo(arquivo_disco, identificador);
        
        if (texto != NULL) {
            printf("==============================\n");
            printf("Conteudo do Arquivo [%d]:\n%s\n", identificador, texto);
            printf("==============================\n");
            free(texto);
        } else {
            printf("Arquivo ID %d nao existe ou esta vazio.\n", identificador);
        }
    }
    else if (strcmp(argv[1], "status") == 0) {
        exibir_info_disco(arquivo_disco);
    }
    else if (strcmp(argv[1], "listar") == 0) {
        listar_arquivos_salvos(arquivo_disco);
    }
    else {
        printf("Operacao invalida. Digite sem argumentos para ver o menu.\n");
    }

    return 0;
}
