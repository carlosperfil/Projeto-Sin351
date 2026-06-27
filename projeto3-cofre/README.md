# Projeto 3 — Cofre de Senhas com Criptografia em Repouso

## Descrição

Implementação de um gerenciador de senhas local com foco em **criptografia de dados em repouso (at-rest encryption)**. O sistema utiliza **PBKDF2-HMAC-SHA256** para derivação de chave e **AES-256-CBC** para criptografia simétrica, com limpeza segura de memória RAM ao encerrar.

### Diagrama de Fluxo

```
[Usuário] -> (Senha Mestra) -> [App] -> (Derivação de Chave via PBKDF2 + Salt)
                                 |
 [Arquivo Criptografado] <--- [AES-256-CBC Encrypt/Decrypt] ---> [Buffer em RAM]
```

### Conceitos de Segurança Aplicados

| Conceito | Implementação |
|----------|---------------|
| Autenticação | Senha mestra ("o que você sabe") |
| Hashing + Salt | PBKDF2-HMAC-SHA256 com salt de 32 bytes e 10.000 iterações |
| Criptografia Simétrica | AES-256-CBC com IV aleatório a cada salvamento |
| Menor Privilégio | Dados decifrados mantidos em RAM apenas quando necessário |
| Segurança de Memória | Função `secure_wipe` sobrescreve buffers com zeros ao encerrar |

---

## Arquivos Fonte

| Arquivo | Função |
|---------|--------|
| `cofre.h` | Definições de structs (`entry`, `vault_header`) e protótipos |
| `cofre.c` | Implementação da criptografia, autenticação e I/O seguro |
| `main_cofre.c` | Interface de menu interativo (CLI) |

---

## Pré-requisitos

- **GCC** 6.0 ou superior
- **Make** (GNU Make)
- **OpenSSL** (biblioteca de desenvolvimento)
- **Sistema Operacional:** Linux (ou WSL no Windows)

### Instalar OpenSSL no Ubuntu/Debian:
```bash
sudo apt-get install libssl-dev
```

---

## Como Compilar

```bash
cd projeto3-cofre
make
```

Ou manualmente:
```bash
gcc -Wall -Wextra -std=c11 -g cofre.c main_cofre.c -o cofre -lssl -lcrypto
```

> **Nota:** As flags `-lssl -lcrypto` são obrigatórias para linkar com a biblioteca OpenSSL.

---

## Como Executar

```bash
# Abrir (ou criar) um cofre
./cofre <arquivo_do_cofre>

# Exemplo:
./cofre meu_cofre.bin
```

O programa abre um menu interativo:
```
=== COFRE DE SENHAS ===
1. Adicionar Senha
2. Buscar Senha
3. Listar Sites
4. Sair (e fechar cofre)
Escolha:
```

---

## Testes e Saídas Esperadas

### Teste 1 — Criar Cofre Novo e Adicionar Senha

**Comando:**
```bash
rm -f cofre_teste.bin
echo -e "MinhaSenha123\n1\ngoogle.com\ncarlos\nG00gl3P@ss\n3\n4" | ./cofre cofre_teste.bin
```

**Explicação dos inputs via pipe:**
| Input | Significado |
|-------|-------------|
| `MinhaSenha123` | Senha mestra (cofre será criado pois não existe) |
| `1` | Opção "Adicionar Senha" |
| `google.com` | Site |
| `carlos` | Username |
| `G00gl3P@ss` | Password |
| `3` | Opção "Listar Sites" (verificação) |
| `4` | Opção "Sair" |

**Saída esperada:**
```
Cofre não encontrado. Criando um novo.
Digite uma Senha Mestra: Cofre criado!
Autenticado! 0 entradas armazenadas.

=== COFRE DE SENHAS ===
1. Adicionar Senha
2. Buscar Senha
3. Listar Sites
4. Sair (e fechar cofre)
Escolha: Site: Username: Password: Salvo.

=== COFRE DE SENHAS ===
1. Adicionar Senha
2. Buscar Senha
3. Listar Sites
4. Sair (e fechar cofre)
Escolha: Sites salvos:
 - google.com

=== COFRE DE SENHAS ===
1. Adicionar Senha
2. Buscar Senha
3. Listar Sites
4. Sair (e fechar cofre)
Escolha: Fechando e limpando memoria RAM...
```

**O que verifica:** Criação do cofre com salt aleatório, derivação de chave via PBKDF2, inserção de entrada e salvamento criptografado com AES-256-CBC.

---

### Teste 2 — Reabrir Cofre e Buscar Senha

**Comando:**
```bash
echo -e "MinhaSenha123\n2\ngoogle.com\n4" | ./cofre cofre_teste.bin
```

**Saída esperada:**
```
Digite a Senha Mestra para abrir: Autenticado! 1 entradas armazenadas.

=== COFRE DE SENHAS ===
1. Adicionar Senha
2. Buscar Senha
3. Listar Sites
4. Sair (e fechar cofre)
Escolha: Site para buscar: -> google.com | User: carlos | Pass: G00gl3P@ss

=== COFRE DE SENHAS ===
1. Adicionar Senha
2. Buscar Senha
3. Listar Sites
4. Sair (e fechar cofre)
Escolha: Fechando e limpando memoria RAM...
```

**O que verifica:** O cofre é reaberto, a senha mestra é validada via hash SHA-256, os dados são decriptografados e a busca retorna a senha correta.

---

### Teste 3 — Acesso Negado com Senha Incorreta

**Comando:**
```bash
echo -e "SenhaErrada\n" | ./cofre cofre_teste.bin
```

**Saída esperada:**
```
Digite a Senha Mestra para abrir: Acesso Negado: Senha Incorreta!
```

**O que verifica:** Autenticação rejeita senhas inválidas — o hash derivado não coincide com o armazenado.

---

## Roteiro Completo de Testes

Execute todos os comandos **na ordem** (ou use `bash test_cofre.sh`):

```bash
# Teste 1: Criar cofre e adicionar senha
rm -f cofre_teste.bin
echo -e "MinhaSenha123\n1\ngoogle.com\ncarlos\nG00gl3P@ss\n3\n4" | ./cofre cofre_teste.bin

# Teste 2: Reabrir e buscar senha
echo -e "MinhaSenha123\n2\ngoogle.com\n4" | ./cofre cofre_teste.bin

# Teste 3: Tentar com senha errada
echo -e "SenhaErrada\n" | ./cofre cofre_teste.bin

# Limpar arquivo de teste
rm -f cofre_teste.bin
```
