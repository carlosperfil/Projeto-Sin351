<p align="center">
  <img src="https://img.shields.io/badge/Disciplina-SIN351-blue?style=for-the-badge" alt="Disciplina SIN351"/>
  <img src="https://img.shields.io/badge/Linguagem-C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="Linguagem C"/>
  <img src="https://img.shields.io/badge/Projetos-3-green?style=for-the-badge" alt="Projetos"/>
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
- [Projeto 1 — Micro Sistema de Arquivos (VSFS)](#-projeto-1--micro-sistema-de-arquivos-vsfs)
- [Projeto 2 — Gerenciador RAID-1 (Mirroring)](#-projeto-2--gerenciador-raid-1-mirroring)
- [Projeto 3 — Cofre de Senhas com Criptografia](#-projeto-3--cofre-de-senhas-com-criptografia)
- [Como Compilar Tudo](#-como-compilar-tudo)
- [Tecnologias Utilizadas](#-tecnologias-utilizadas)
- [Autores](#-autores)

---

## 🎯 Sobre o Projeto

Este repositório contém os **3 projetos práticos** desenvolvidos para a disciplina **SIN351 — Sistemas Operacionais**, abordando dois eixos fundamentais:

| Eixo | Projetos | Conceitos |
|------|----------|-----------|
| **Persistência** | VSFS + RAID-1 | Sistemas de arquivos, inodes, bitmaps, espelhamento, WAL |
| **Segurança** | Cofre de Senhas | Criptografia AES-256, PBKDF2, hashing com salt, wipe de RAM |

Cada projeto é **auto-contido** em sua própria pasta com README, Makefile, código-fonte e script de testes.

---

## 📁 Estrutura do Repositório

```
Projeto-Sin351/
│
├── README.md                      # Esta documentação
│
├── projeto1-vsfs/                 # PERSISTÊNCIA: Micro Sistema de Arquivos
│   ├── README.md                  # Instruções + testes + saídas esperadas
│   ├── Makefile                   # Compilação independente
│   ├── vsfs.h                     # Definições e protótipos
│   ├── vsfs.c                     # Implementação do sistema de arquivos
│   ├── main_vsfs.c                # CLI de interação
│   └── test_vsfs.sh               # Script de testes automatizado
│
├── projeto2-raid1/                # PERSISTÊNCIA: Gerenciador RAID-1
│   ├── README.md                  # Instruções + testes + saídas esperadas
│   ├── Makefile                   # Compilação independente
│   ├── raid1.h                    # Definições e protótipos
│   ├── raid1.c                    # Implementação do espelhamento + WAL
│   ├── main_raid1.c               # CLI de interação
│   └── test_raid1.sh              # Script de testes automatizado
│
├── projeto3-cofre/                # SEGURANÇA: Cofre de Senhas Criptografado
│   ├── README.md                  # Instruções + testes + saídas esperadas
│   ├── Makefile                   # Compilação independente
│   ├── cofre.h                    # Definições e protótipos
│   ├── cofre.c                    # Implementação da criptografia e auth
│   ├── main_cofre.c               # Menu interativo
│   └── test_cofre.sh              # Script de testes automatizado
│
└── docs/                          # Enunciados originais do professor
    ├── persistencia.md
    ├── seguranca.md
    ├── vsfs.md
    ├── raid1.md
    └── cofre.md
```

---

## 💾 Projeto 1 — Micro Sistema de Arquivos (VSFS)

> 📂 **Pasta:** [`projeto1-vsfs/`](projeto1-vsfs/) — [README completo](projeto1-vsfs/README.md)

Simulação de um **Very Simple File System** em um arquivo binário (`disco.img`) de 1 MB com blocos de 4 KB.

```
[S | i | d | I | I | I | I | I | D | D | ... | D]
 S: Superbloco   i: Inode Bitmap   d: Data Bitmap   I: Inodes   D: Dados
```

**Compilar e testar rapidamente:**
```bash
cd projeto1-vsfs
make
./vsfs format
./vsfs write "Hello World"
./vsfs read 0
```

---

## 💾 Projeto 2 — Gerenciador RAID-1 (Mirroring)

> 📂 **Pasta:** [`projeto2-raid1/`](projeto2-raid1/) — [README completo](projeto2-raid1/README.md)

Espelhamento de dois discos virtuais com **Write-Ahead Logging** para consistência transacional.

```
Escrita → [WAL Log] → [Disco 0] + [Disco 1] → [Commit]
```

**Compilar e testar rapidamente:**
```bash
cd projeto2-raid1
make
./raid1 init
./raid1 write 0 "Dados espelhados"
./raid1 read 0 0
./raid1 read 0 1
./raid1 verify
```

---

## 🔐 Projeto 3 — Cofre de Senhas com Criptografia

> 📂 **Pasta:** [`projeto3-cofre/`](projeto3-cofre/) — [README completo](projeto3-cofre/README.md)

Gerenciador de senhas com **AES-256-CBC**, derivação de chave via **PBKDF2** e limpeza segura de memória.

```
[Usuário] → (Senha Mestra) → [PBKDF2 + Salt] → [AES Encrypt/Decrypt] → [Arquivo .bin]
```

**Compilar e testar rapidamente:**
```bash
cd projeto3-cofre
make
./cofre meu_cofre.bin
```

> **Requer:** `sudo apt-get install libssl-dev`

---

## 🚀 Como Compilar Tudo

### Pré-requisitos

| Dependência | Para qual projeto | Como instalar |
|-------------|-------------------|---------------|
| GCC 6+ | Todos | `sudo apt-get install build-essential` |
| Make | Todos | `sudo apt-get install make` |
| OpenSSL dev | Projeto 3 (Cofre) | `sudo apt-get install libssl-dev` |

### Compilar cada projeto individualmente

```bash
# Projeto 1
cd projeto1-vsfs && make && cd ..

# Projeto 2
cd projeto2-raid1 && make && cd ..

# Projeto 3
cd projeto3-cofre && make && cd ..
```

### Executar todos os testes

```bash
cd projeto1-vsfs && bash test_vsfs.sh && cd ..
cd projeto2-raid1 && bash test_raid1.sh && cd ..
cd projeto3-cofre && bash test_cofre.sh && cd ..
```

---

## 🛠 Tecnologias Utilizadas

| Tecnologia | Finalidade |
|------------|------------|
| **C (C11)** | Linguagem principal de implementação |
| **GCC / Make** | Compilação e automação de build |
| **OpenSSL** | Biblioteca de criptografia (Projeto 3) |
| **Git / GitHub** | Versionamento e colaboração |

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
    <td align="center">
      <a href="https://github.com/VictorVieira540">
        <img src="https://github.com/VictorVieira540.png" width="100px;" alt="Victor"/><br />
        <sub><b>Victor</b></sub>
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
