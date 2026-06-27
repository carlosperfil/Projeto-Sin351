# Projeto 1 — Micro Sistema de Arquivos (VSFS)

## Descrição

Implementação de um **Very Simple File System (vsfs)** que simula um disco virtual em um arquivo binário (`disco.img`). O sistema organiza o disco em blocos de 4 KB com superbloco, bitmaps de alocação, tabela de inodes e região de dados.

### Estrutura do Disco Simulado

```
[S | i | d | I | I | I | I | I | D | D | ... | D]
 S: Superbloco (Bloco 0)        — metadados do sistema
 i: Inode Bitmap (Bloco 1)      — bitmap de alocação de inodes
 d: Data Bitmap (Bloco 2)       — bitmap de alocação de dados
 I: Tabela de Inodes (Blocos 3–7) — até 640 inodes
 D: Região de Dados (Blocos 8–255) — 248 blocos para conteúdo
```

**Tamanho total do disco:** 1 MB (256 blocos × 4096 bytes).

---

## Arquivos Fonte

| Arquivo | Função |
|---------|--------|
| `vsfs.h` | Definições de structs (`superblock`, `inode`) e protótipos |
| `vsfs.c` | Implementação das operações do sistema de arquivos |
| `main_vsfs.c` | Interface de linha de comando (CLI) |

---

## Pré-requisitos

- **GCC** 6.0 ou superior
- **Make** (GNU Make)
- **Sistema Operacional:** Linux (ou WSL no Windows)

---

## Como Compilar

```bash
cd projeto1-vsfs
make
```

Ou manualmente:
```bash
gcc -Wall -Wextra -std=c11 -g vsfs.c main_vsfs.c -o vsfs
```

---

## Como Executar

```bash
# Formatar o disco virtual
./vsfs format

# Escrever um arquivo (string) no disco
./vsfs write "<texto>"

# Ler um arquivo pelo número do inode
./vsfs read <inumber>

# Exibir informações do superbloco
./vsfs info

# Listar todos os inodes alocados
./vsfs list
```

---

## Testes e Saídas Esperadas

### Teste 1 — Formatar o Disco

**Comando:**
```bash
./vsfs format
```

**Saída esperada:**
```
Disco 'disco.img' formatado com sucesso.
```

**O que verifica:** Criação do arquivo `disco.img` (1 MB) com superbloco inicializado e bitmaps zerados.

---

### Teste 2 — Exibir Informações do Superbloco

**Comando:**
```bash
./vsfs info
```

**Saída esperada:**
```
--- VSFS Superbloco Info ---
Magic Number: 0x12345678
Max Inodes: 640
Max Blocos de Dados: 248
```

**O que verifica:** Leitura correta do superbloco com o Magic Number e contadores de inodes/blocos.

---

### Teste 3 — Escrever Primeiro Arquivo

**Comando:**
```bash
./vsfs write "Ola Mundo VSFS"
```

**Saída esperada:**
```
Arquivo gravado no Inode 0.
```

**O que verifica:** Alocação do primeiro inode (ID 0), alocação de bloco de dados, escrita do conteúdo e atualização dos bitmaps.

---

### Teste 4 — Escrever Segundo Arquivo

**Comando:**
```bash
./vsfs write "Segundo arquivo de teste"
```

**Saída esperada:**
```
Arquivo gravado no Inode 1.
```

**O que verifica:** Alocação sequencial de inodes — o segundo arquivo recebe inode 1.

---

### Teste 5 — Listar Inodes em Uso

**Comando:**
```bash
./vsfs list
```

**Saída esperada:**
```
--- VSFS Arquivos (Inodes) ---
Inode 0: Tamanho 14 bytes
Inode 1: Tamanho 24 bytes
```

**O que verifica:** Bitmap de inodes corretamente marcado e leitura dos metadados de tamanho.

---

### Teste 6 — Ler Arquivo pelo Inode

**Comando:**
```bash
./vsfs read 0
```

**Saída esperada:**
```
Conteudo (Inode 0):
Ola Mundo VSFS
```

**O que verifica:** Leitura do inode, resolução do ponteiro direto para o bloco de dados e recuperação do conteúdo original.

---

### Teste 7 — Ler Segundo Arquivo

**Comando:**
```bash
./vsfs read 1
```

**Saída esperada:**
```
Conteudo (Inode 1):
Segundo arquivo de teste
```

**O que verifica:** Isolamento entre arquivos — cada inode aponta para blocos de dados diferentes.

---

## Roteiro Completo de Testes

Execute todos os comandos **na ordem** (ou use `bash test_vsfs.sh`):

```bash
./vsfs format
./vsfs info
./vsfs write "Ola Mundo VSFS"
./vsfs write "Segundo arquivo de teste"
./vsfs list
./vsfs read 0
./vsfs read 1
```

**Saída completa esperada:**
```
Disco 'disco.img' formatado com sucesso.
--- VSFS Superbloco Info ---
Magic Number: 0x12345678
Max Inodes: 640
Max Blocos de Dados: 248
Arquivo gravado no Inode 0.
Arquivo gravado no Inode 1.
--- VSFS Arquivos (Inodes) ---
Inode 0: Tamanho 14 bytes
Inode 1: Tamanho 24 bytes
Conteudo (Inode 0):
Ola Mundo VSFS
Conteudo (Inode 1):
Segundo arquivo de teste
```
