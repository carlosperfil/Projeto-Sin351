#include "cofre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exibir_menu_principal() {
    printf("\n============================\n");
    printf("   COFRE DIGITAL SEGURO\n");
    printf("============================\n");
    printf("1 - Cadastrar nova senha\n");
    printf("2 - Buscar senha por site\n");
    printf("3 - Ver todos os sites salvos\n");
    printf("4 - Trancar cofre e Sair\n");
    printf("O que deseja fazer? ");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Sintaxe correta: %s [nome_arquivo_cofre]\n", argv[0]);
        return 1;
    }
    
    const char* caminho_cofre = argv[1];
    
    char senha_digitada[128];
    unsigned char chave_sessao[TAMANHO_CHAVE];
    int quantidade_senhas = 0;
    
    FILE *checagem = fopen(caminho_cofre, "rb");
    if (checagem == NULL) {
        printf("Este cofre não existe. Iremos criar um agora.\n");
        printf("Defina sua Senha Mestra: ");
        scanf("%127s", senha_digitada);
        
        criar_novo_cofre(caminho_cofre, senha_digitada);
        printf("Sucesso: Cofre inicializado e protegido!\n");
    } else {
        fclose(checagem);
        printf("Cofre detectado. Digite a Senha Mestra para destrancar: ");
        scanf("%127s", senha_digitada);
    }
    
    quantidade_senhas = validar_acesso_cofre(caminho_cofre, senha_digitada, chave_sessao);
    
    // Zera a string da senha imediatamente da memoria RAM (Stack)
    limpar_memoria_sensivel(senha_digitada, sizeof(senha_digitada));
    
    if (quantidade_senhas < 0) {
        printf("\nERRO CRITICO: Senha Incorreta. Acesso Negado!\n");
        return 1;
    }
    
    printf("Autenticacao OK! Temos %d site(s) guardado(s).\n", quantidade_senhas);
    
    struct registro_senha* banco_dados = carregar_senhas_memoria(caminho_cofre, chave_sessao, &quantidade_senhas);
    if (banco_dados == NULL) {
        printf("Problema ao descriptografar. O arquivo pode estar corrompido.\n");
        limpar_memoria_sensivel(chave_sessao, TAMANHO_CHAVE);
        return 1;
    }
    
    int opcao_escolhida;
    int rodando = 1;
    
    while(rodando) {
        exibir_menu_principal();
        scanf("%d", &opcao_escolhida);
        
        switch(opcao_escolhida) {
            case 1:
                if (quantidade_senhas >= LIMITE_SENHAS) {
                    printf("Aviso: O cofre atingiu seu limite maximo de senhas!\n");
                    break;
                }
                printf("Qual o nome do Site/Servico: ");
                scanf("%127s", banco_dados[quantidade_senhas].nome_site);
                
                printf("Seu nome de usuario la: ");
                scanf("%63s", banco_dados[quantidade_senhas].nome_usuario);
                
                printf("A senha: ");
                scanf("%127s", banco_dados[quantidade_senhas].senha_texto);
                
                quantidade_senhas++;
                salvar_senhas_disco(caminho_cofre, chave_sessao, banco_dados, quantidade_senhas);
                printf("Registro salvo de forma segura no disco.\n");
                break;
                
            case 2: {
                char site_procurado[128];
                printf("Digite o nome do Site para a busca: ");
                scanf("%127s", site_procurado);
                
                int encontrou = 0;
                for(int j = 0; j < quantidade_senhas; j++) {
                    if (strcmp(banco_dados[j].nome_site, site_procurado) == 0) {
                        printf("\n[ RESULTADO DA BUSCA ]\n");
                        printf("Site: %s\n", banco_dados[j].nome_site);
                        printf("Usuario: %s\n", banco_dados[j].nome_usuario);
                        printf("Senha: %s\n", banco_dados[j].senha_texto);
                        encontrou = 1;
                        break;
                    }
                }
                
                if (encontrou == 0) {
                    printf("Nenhuma credencial encontrada para esse site.\n");
                }
                break;
            }
            case 3:
                printf("\n--- SITES CADASTRADOS ---\n");
                if (quantidade_senhas == 0) {
                    printf("(Vazio)\n");
                } else {
                    for(int j = 0; j < quantidade_senhas; j++) {
                        printf("[%d] %s\n", j+1, banco_dados[j].nome_site);
                    }
                }
                break;
                
            case 4:
                printf("Protegendo dados e limpando a RAM antes de sair...\n");
                rodando = 0;
                break;
                
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    }
    
    // WIPES de seguranca - Limpa RAM dinamicamente alocada e chaves AES
    limpar_memoria_sensivel(banco_dados, sizeof(struct registro_senha) * LIMITE_SENHAS);
    free(banco_dados);
    limpar_memoria_sensivel(chave_sessao, TAMANHO_CHAVE);
    
    return 0;
}
