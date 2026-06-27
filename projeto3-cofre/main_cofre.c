#include "cofre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_menu() {
    printf("\n=== COFRE DE SENHAS ===\n");
    printf("1. Adicionar Senha\n");
    printf("2. Buscar Senha\n");
    printf("3. Listar Sites\n");
    printf("4. Sair (e fechar cofre)\n");
    printf("Escolha: ");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s [arquivo_do_cofre]\n", argv[0]);
        return 1;
    }
    const char* file = argv[1];
    
    char master[128];
    unsigned char session_key[KEY_SIZE];
    int count = 0;
    
    FILE *f = fopen(file, "rb");
    if (!f) {
        printf("Cofre não encontrado. Criando um novo.\nDigite uma Senha Mestra: ");
        scanf("%127s", master);
        vault_create(file, master);
        printf("Cofre criado!\n");
    } else {
        fclose(f);
        printf("Digite a Senha Mestra para abrir: ");
        scanf("%127s", master);
    }
    
    count = vault_authenticate(file, master, session_key);
    secure_wipe(master, sizeof(master)); // limpa a master pass do stack
    
    if (count < 0) {
        printf("Acesso Negado: Senha Incorreta!\n");
        return 1;
    }
    
    printf("Autenticado! %d entradas armazenadas.\n", count);
    
    struct entry* db = vault_load(file, session_key, &count);
    if (!db) {
        printf("Erro ao decifrar. Arquivo corrompido?\n");
        secure_wipe(session_key, KEY_SIZE);
        return 1;
    }
    
    int opt;
    while(1) {
        print_menu();
        scanf("%d", &opt);
        if(opt == 1) {
            if(count >= MAX_ENTRIES) {
                printf("Cofre cheio!\n"); continue;
            }
            printf("Site: "); scanf("%127s", db[count].site);
            printf("Username: "); scanf("%63s", db[count].username);
            printf("Password: "); scanf("%127s", db[count].password);
            count++;
            vault_save(file, session_key, db, count);
            printf("Salvo.\n");
        } 
        else if(opt == 2) {
            char busca[128];
            printf("Site para buscar: "); scanf("%127s", busca);
            int achou = 0;
            for(int i=0; i<count; i++) {
                if(strcmp(db[i].site, busca) == 0) {
                    printf("-> %s | User: %s | Pass: %s\n", db[i].site, db[i].username, db[i].password);
                    achou = 1; break;
                }
            }
            if(!achou) printf("Site não encontrado.\n");
        }
        else if(opt == 3) {
            printf("Sites salvos:\n");
            for(int i=0; i<count; i++) {
                printf(" - %s\n", db[i].site);
            }
        }
        else if(opt == 4) {
            printf("Fechando e limpando memoria RAM...\n");
            break;
        }
    }
    
    // WIPES de seguranca
    secure_wipe(db, sizeof(struct entry) * MAX_ENTRIES);
    free(db);
    secure_wipe(session_key, KEY_SIZE);
    
    return 0;
}
