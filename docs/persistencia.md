Com base nos conceitos de **persistência**, **algoritmos de agendamento** e **estruturas de inodes** apresentados nas fontes, implementem os projetos abaixo.

---

### Projeto 1: Micro Sistema de Arquivos Baseado em vsfs (Very Simple File System)

Este projeto foca na implementação das estruturas fundamentais de um sistema de arquivos em um arquivo binário que simula um disco.

**Arquitetura:**
A estrutura segue a organização do **vsfs**, dividindo o "disco" em blocos de 4 KB.
1.  **Superbloco:** Contém metadados sobre o sistema (número de inodes, blocos de dados, número mágico).
2.  **Bitmaps:** Um para inodes e outro para dados, rastreando blocos livres/ocupados.
3.  **Tabela de Inodes:** Array de estruturas de inodes.
4.  **Região de Dados:** Blocos onde o conteúdo dos arquivos é armazenado.

**Diagrama de Organização do Disco:**
```text
[S | i | d | I | I | I | I | I | D | D | ... | D]
 S: Superbloco
 i: Inode Bitmap
 d: Data Bitmap
 I: Tabela de Inodes
 D: Região de Dados
```

**Código Inicial (C):**
```c
#define BLOCK_SIZE 4096

struct superblock {
    int magic;
    int num_inodes;
    int num_data_blocks;
};

struct inode {
    int size;       // tamanho em bytes
    int type;       // arquivo regular ou diretório
    int direct; // ponteiros diretos para blocos de dados
    int indirect;   // ponteiro indireto
};

// Exemplo de cálculo de endereço de inode
int get_inode_sector(int inumber, int inode_start_addr) {
    return ((inumber * sizeof(struct inode)) + inode_start_addr) / 512;
}
```

**Roteiro de Implementação:**
1.  **Formatação:** Crie um programa para inicializar o arquivo "disco.img" com o superbloco e bitmaps zerados.
2.  **Gerenciamento de Espaço:** Implemente funções para buscar e marcar bits livres nos bitmaps de inodes e dados.
3.  **Operações de Inode:** Desenvolva rotinas para ler e escrever inodes no disco simulado.
4.  **Acesso a Arquivos:** Implemente uma função `write_file` que aloca blocos de dados, atualiza o bitmap e vincula os endereços ao inode.

---

### Projeto 2: Gerenciador RAID-1 (Mirroring) com Inodes de Metadados

Este projeto foca na redundância e confiabilidade, utilizando o conceito de **RAID-1** e o gerenciamento de inodes para mapear arquivos em múltiplos discos.

**Arquitetura:**
1.  **Discos Virtuais:** Dois arquivos representando os discos espelhados.
2.  **Mapeador Lógico-Físico:** Um sistema que usa **inodes** para identificar arquivos, mas realiza escritas paralelas em ambos os discos virtuais.
3.  **Tratamento de Consistência:** Uso de um log de transação para evitar o problema de atualização inconsistente (*consistent-update problem*) caso o sistema caia entre as duas escritas.

**Diagrama de Mirroring:**
```text
Escrita Lógica (Arquivo A)
      |
      V
[ Log de Escrita ] (Inicia Transação)
      |
  +---+---+
  |       |
[Disco 0] [Disco 1] (Escritas em Paralelo)
  |       |
  +---+---+
      |
[ Log de Escrita ] (Finaliza Transação)
```

**Código Inicial (C):**
```c
// Estrutura de escrita espelhada
void raid1_write(int block_id, char *buffer) {
    // 1. Logar intenção de escrita (Write-Ahead Logging)
    log_transaction(block_id, buffer);
    
    // 2. Escrever no Disco 0
    write_to_virtual_disk("disk0.bin", block_id, buffer);
    
    // 3. Escrever no Disco 1
    write_to_virtual_disk("disk1.bin", block_id, buffer);
    
    // 4. Marcar transação como completa no log
    commit_transaction();
}
```

**Roteiro de Implementação:**
1.  **Configuração de Espelhamento:** Crie dois arquivos de tamanho fixo para representar os discos $N$ e $N+1$.
2.  **Sistema de Inodes:** Implemente inodes que armazenam o `block_id` lógico, que será mapeado para o mesmo offset em ambos os discos.
3.  **Algoritmo de Escrita:** Garanta que a escrita lógica só retorne sucesso após ambos os blocos físicos serem confirmados.
4.  **Verificação de Integridade:** Implemente uma função que compare os blocos dos dois discos baseada no inode; se houver divergência, use o log para corrigir o espelho desatualizado.