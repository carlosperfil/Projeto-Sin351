# Projeto 2 — Gerenciador RAID-1 (Mirroring)

## Descrição

Implementação de um sistema de **espelhamento RAID-1** que mantém dois discos virtuais idênticos com suporte a **Write-Ahead Logging (WAL)** para garantir consistência transacional.

### Diagrama de Escrita Espelhada

```
Escrita Lógica (Dados)
      |
      V
[ Log de Escrita ] ── Inicia Transação (PENDING)
      |
  +---+---+
  |       |
[Disco 0] [Disco 1] ── Escritas em Paralelo
  |       |
  +---+---+
      |
[ Log de Escrita ] ── Finaliza Transação (COMMITTED)
```

### Algoritmo de Consistência (WAL)

1. Registra no log (status `PENDING`)
2. Escreve no Disco 0 (`disk0.bin`)
3. Escreve no Disco 1 (`disk1.bin`)
4. Marca no log (status `COMMITTED`)

Se ocorrer falha antes do passo 4, o comando `recover` reaplica as transações commitadas para garantir sincronização.

---

## Arquivos Fonte

| Arquivo | Função |
|---------|--------|
| `raid1.h` | Definições de structs (`log_entry`, `raid_inode`) e protótipos |
| `raid1.c` | Implementação do espelhamento, WAL e verificação |
| `main_raid1.c` | Interface de linha de comando (CLI) |

### Arquivos Gerados em Execução

| Arquivo | Descrição |
|---------|-----------|
| `disk0.bin` | Disco virtual 0 (1 MB) |
| `disk1.bin` | Disco virtual 1 — espelho do disco 0 |
| `raid_log.bin` | Write-Ahead Log com histórico de transações |

---

## Pré-requisitos

- **GCC** 6.0 ou superior
- **Make** (GNU Make)
- **Sistema Operacional:** Linux (ou WSL no Windows)

---

## Como Compilar

```bash
cd projeto2-raid1
make
```

Ou manualmente:
```bash
gcc -Wall -Wextra -std=c11 -g raid1.c main_raid1.c -o raid1
```

---

## Como Executar

```bash
# Inicializar os dois discos virtuais e o log
./raid1 init

# Escrever dados em ambos os discos (espelhado)
./raid1 write <block_id> "<texto>"

# Ler dados de um disco específico (0 ou 1)
./raid1 read <block_id> <disco>

# Verificar integridade entre os dois discos
./raid1 verify

# Recuperar usando o WAL Log
./raid1 recover

# Exibir status do array RAID
./raid1 status
```

---

## Testes e Saídas Esperadas

### Teste 1 — Inicializar o Array RAID-1

**Comando:**
```bash
./raid1 init
```

**Saída esperada:**
```
RAID-1 formatado e pronto.
```

**O que verifica:** Criação dos arquivos `disk0.bin`, `disk1.bin` (1 MB cada) e `raid_log.bin`.

---

### Teste 2 — Escrita Espelhada no Bloco 0

**Comando:**
```bash
./raid1 write 0 "Dados espelhados RAID"
```

**Saída esperada:**
```
Sucesso! Escrita espelhada em disk0 e disk1 no bloco 0.
```

**O que verifica:** Escrita transacional com WAL — o dado é gravado em ambos os discos.

---

### Teste 3 — Escrita Espelhada no Bloco 5

**Comando:**
```bash
./raid1 write 5 "Bloco 5 com mais dados"
```

**Saída esperada:**
```
Sucesso! Escrita espelhada em disk0 e disk1 no bloco 5.
```

**O que verifica:** Escritas em blocos diferentes funcionam independentemente.

---

### Teste 4 — Leitura do Disco 0

**Comando:**
```bash
./raid1 read 0 0
```

**Saída esperada:**
```
Conteudo (Disco 0, Bloco 0):
Dados espelhados RAID
```

**O que verifica:** Leitura direta do disco 0 retorna o conteúdo original.

---

### Teste 5 — Leitura do Disco 1 (Espelho)

**Comando:**
```bash
./raid1 read 0 1
```

**Saída esperada:**
```
Conteudo (Disco 1, Bloco 0):
Dados espelhados RAID
```

**O que verifica:** O espelho (disco 1) contém exatamente os mesmos dados — comprovação do RAID-1.

---

### Teste 6 — Verificação de Integridade

**Comando:**
```bash
./raid1 verify
```

**Saída esperada:**
```
Array Saudavel! Sem divergencias.
```

**O que verifica:** Comparação byte-a-byte dos 256 blocos de ambos os discos confirma que são idênticos.

---

### Teste 7 — Status do Array

**Comando:**
```bash
./raid1 status
```

**Saída esperada:**
```
Disco 0: Online
Disco 1: Online
WAL Log: 0 pendentes, 2 executados.
```

**O que verifica:** Ambos os discos acessíveis e o log possui 2 transações commitadas.

---

### Teste 8 — Recuperação via WAL

**Comando:**
```bash
./raid1 recover
```

**Saída esperada:**
```
Recuperacao finalizada. 2 transacoes garantidas.
```

**O que verifica:** O sistema relê o log e reaplica as transações em ambos os discos.

---

## Roteiro Completo de Testes

Execute todos os comandos **na ordem** (ou use `bash test_raid1.sh`):

```bash
./raid1 init
./raid1 write 0 "Dados espelhados RAID"
./raid1 write 5 "Bloco 5 com mais dados"
./raid1 read 0 0
./raid1 read 0 1
./raid1 verify
./raid1 status
./raid1 recover
```

**Saída completa esperada:**
```
RAID-1 formatado e pronto.
Sucesso! Escrita espelhada em disk0 e disk1 no bloco 0.
Sucesso! Escrita espelhada em disk0 e disk1 no bloco 5.
Conteudo (Disco 0, Bloco 0):
Dados espelhados RAID
Conteudo (Disco 1, Bloco 0):
Dados espelhados RAID
Array Saudavel! Sem divergencias.
Disco 0: Online
Disco 1: Online
WAL Log: 0 pendentes, 2 executados.
Recuperacao finalizada. 2 transacoes garantidas.
```
