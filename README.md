<p align="center">
  <img src="https://img.shields.io/badge/Disciplina-SIN351-blue?style=for-the-badge" alt="Disciplina SIN351"/>
  <img src="https://img.shields.io/badge/Linguagem-C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="Linguagem C"/>
  <img src="https://img.shields.io/badge/Status-Em%20Desenvolvimento-yellow?style=for-the-badge" alt="Status"/>
</p>

<h1 align="center">📂 Projeto SIN351 — Persistência e Segurança</h1>

<p align="center">
  <b>Projetos práticos de implementação de sistemas de arquivos, RAID e segurança de dados</b><br/>
  <i>Universidade Federal de Viçosa</i>
</p>

---

## 📋 Sumário

- [Sobre o Projeto](#-sobre-o-projeto)
- [Estrutura do Repositório](#-estrutura-do-repositório)
- [Projetos de Persistência](#-projetos-de-persistência)
  - [Projeto 1 — Micro Sistema de Arquivos (vsfs)](#projeto-1--micro-sistema-de-arquivos-vsfs)
  - [Projeto 2 — Gerenciador RAID-1 (Mirroring)](#projeto-2--gerenciador-raid-1-mirroring)
- [Projeto de Segurança](#-projeto-de-segurança)
  - [Projeto 3 — Cofre de Senhas com Criptografia em Repouso](#projeto-3--cofre-de-senhas-com-criptografia-em-repouso)
- [Tecnologias Utilizadas](#-tecnologias-utilizadas)
- [Como Compilar e Executar](#-como-compilar-e-executar)
- [Autores](#-autores)
- [Licença](#-licença)

---

## 🎯 Sobre o Projeto

Este repositório contém os projetos práticos desenvolvidos para a disciplina **SIN351 — Sistemas Operacionais**, abordando dois eixos fundamentais:

| Eixo | Foco | Conceitos |
|------|------|-----------|
| **Persistência** | Armazenamento e organização de dados em disco | Sistemas de arquivos, inodes, bitmaps, RAID-1 |
| **Segurança** | Proteção e integridade de dados | Criptografia AES, hashing com salt, autenticação |

Os projetos simulam cenários reais de implementação, desde a construção de um sistema de arquivos simplificado até a criação de um cofre de senhas com criptografia simétrica.

---

## 📁 Estrutura do Repositório

```
Projeto-Sin351/
│
├── 📄 README.md                  # Documentação principal
├── 📄 .gitignore                 # Arquivos ignorados pelo Git
│
└── docs/                         # Documentação detalhada
    ├── 📄 persistencia.md        # Enunciados dos projetos de persistência
    └── 📄 seguranca.md           # Enunciado do projeto de segurança
```

---

## 💾 Projetos de Persistência

### Projeto 1 — Micro Sistema de Arquivos (vsfs)

Implementação das estruturas fundamentais de um **Very Simple File System (vsfs)** em um arquivo binário que simula um disco.

**Organização do Disco Simulado:**

```
[S | i | d | I | I | I | I | I | D | D | ... | D]
 S: Superbloco          i: Inode Bitmap
 d: Data Bitmap         I: Tabela de Inodes
 D: Região de Dados
```

**Funcionalidades:**
- ✅ Formatação do disco virtual (`disco.img`) com superbloco e bitmaps
- ✅ Gerenciamento de espaço livre via bitmaps de inodes e dados
- ✅ Operações CRUD de inodes no disco simulado
- ✅ Alocação de blocos de dados com ponteiros diretos e indiretos
- ✅ Função `write_file` com atualização completa de metadados

**Estruturas Principais:**

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
    int direct;     // ponteiros diretos para blocos de dados
    int indirect;   // ponteiro indireto
};
```

---

### Projeto 2 — Gerenciador RAID-1 (Mirroring)

Implementação de um sistema de **espelhamento RAID-1** com inodes de metadados e log de transações para garantir consistência.

**Fluxo de Escrita Espelhada:**

```
Escrita Lógica (Arquivo A)
      |
      V
[ Log de Escrita ] ──── Inicia Transação
      |
  +---+---+
  |       |
[Disco 0] [Disco 1] ── Escritas em Paralelo
  |       |
  +---+---+
      |
[ Log de Escrita ] ──── Finaliza Transação
```

**Funcionalidades:**
- ✅ Configuração de dois discos virtuais espelhados
- ✅ Sistema de inodes com mapeamento lógico-físico
- ✅ Write-Ahead Logging (WAL) para consistência transacional
- ✅ Verificação de integridade entre os discos espelhados
- ✅ Recuperação automática usando log em caso de divergência

---

## 🔐 Projeto de Segurança

### Projeto 3 — Cofre de Senhas com Criptografia em Repouso

Implementação de um gerenciador de senhas local com foco em **criptografia de dados em repouso (at-rest encryption)**.

**Diagrama de Fluxo:**

```
[Usuário] -> (Senha Mestra) -> [App] -> (Derivação de Chave via Hash/Salt)
                                 |
 [Arquivo Criptografado] <--- [AES Encrypt/Decrypt] ---> [Buffer em RAM]
```

**Conceitos de Segurança Aplicados:**

| Conceito | Implementação |
|----------|---------------|
| Autenticação | Senha mestra ("o que você sabe") |
| Hashing + Salt | SHA-256/SHA-3 com salt aleatório |
| Criptografia Simétrica | AES-128 ou AES-256 |
| Menor Privilégio | Dados decifrados mantidos em RAM apenas quando necessário |
| Segurança de Memória | Sobrescrita de dados sensíveis com zeros ao encerrar |

**Funcionalidades:**
- ✅ Autenticação segura via senha mestra com hash e salt
- ✅ Armazenamento criptografado (AES) em arquivo binário
- ✅ CRUD de entradas (site, usuário, senha)
- ✅ Limpeza segura de memória RAM ao encerrar

---

## 🛠 Tecnologias Utilizadas

| Tecnologia | Finalidade |
|------------|------------|
| **C (C99/C11)** | Linguagem principal de implementação |
| **GCC / Make** | Compilação e automação de build |
| **OpenSSL / Libsodium** | Biblioteca de criptografia (Projeto 3) |
| **Git / GitHub** | Versionamento e colaboração |

---

## 🚀 Como Compilar e Executar

### Pré-requisitos

- GCC 9+ ou Clang
- Make (GNU Make)
- OpenSSL (para o Projeto de Segurança)

### Compilação

```bash
# Clonar o repositório
git clone https://github.com/carlosperfil/Projeto-Sin351.git
cd Projeto-Sin351

# Compilar um projeto específico (quando implementado)
gcc -o vsfs src/persistence/vsfs.c -Wall -Wextra
gcc -o raid1 src/persistence/raid1.c -Wall -Wextra
gcc -o cofre src/security/cofre.c -lssl -lcrypto -Wall -Wextra
```

### Execução

```bash
# Micro Sistema de Arquivos
./vsfs

# Gerenciador RAID-1
./raid1

# Cofre de Senhas
./cofre
```

---

## 👥 Autores

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/carlosperfil">
        <img src="https://github.com/carlosperfil.png" width="100px;" alt="Carlos"/><br />
        <sub><b>Carlos</b></sub>
      </a>
    </td>
  </tr>
</table>

---

## 📄 Licença

Este projeto é de uso acadêmico, desenvolvido para a disciplina **SIN351** da Universidade Federal de Viçosa.

---

<p align="center">
  <i>Feito com 💻 para a disciplina SIN351 — Sistemas Operacionais</i>
</p>
