# Micro Sistema de Arquivos (VSFS)

O **VSFS** (Very Simple File System) implementado neste projeto é uma abstração de disco simulado num arquivo chamado `disco.img`. 

## Estrutura Físca Simulada

O disco tem o tamanho padronizado de 1 MB (256 blocos de 4096 bytes). Ele se divide nas seguintes regiões:

- **Bloco 0 (Superbloco):** Contém os metadados principais (Magic Number, tamanho das áreas).
- **Bloco 1 (Inode Bitmap):** Um bitmap de alocação de Inodes.
- **Bloco 2 (Data Bitmap):** Um bitmap para blocos de dados.
- **Blocos 3 a 7 (Tabela de Inodes):** Os blocos dedicados à tabela de Inodes (total de 5 blocos, permitindo 512 Inodes no máximo).
- **Blocos 8 a 255 (Dados):** Região onde o conteúdo dos arquivos é de fato salvo.

## Como Usar a CLI

O executável `vsfs` gerado permite formatar e operar o disco virtual.

- `vsfs format` — Inicializa e formata o `disco.img` limpando tudo.
- `vsfs write <texto>` — Cria um Inode e aloca blocos de dados para o `<texto>`, retornando o ID do Inode alocado.
- `vsfs read <id>` — Lê e imprime o conteúdo do arquivo alocado ao Inode `<id>`.
- `vsfs list` — Exibe todos os Inodes atualmente em uso.
- `vsfs info` — Exibe informações gerais sobre os limites do superbloco.

## Detalhes de Implementação
A estrutura `inode` tem suporte nativo para até 5 ponteiros diretos de bloco (permitindo arquivos de até 20 KB de forma simples).
Foi utilizado algoritmos simples de manipulação de bitwise shifts (`>>` e `<<`) para a escrita do bitmap.
