# Gerenciador RAID-1 (Mirroring)

A implementação do **RAID-1** simula a redundância e a integridade numa configuração de armazenamento espelhado.

## Estrutura

- **Disk 0 e Disk 1**: Dois arquivos `.bin` que são mantidos como espelhos perfeitos. O que é gravado no bloco lógico 5 é escrito no bloco físico 5 em ambos os discos.
- **WAL (Write-Ahead Log)**: Um arquivo chamado `raid_log.bin` armazena a "intenção de escrever" antes da transação de fato afetar os discos.

## Algoritmo de Consistência (WAL)

Para evitar problemas de *consistent-update* onde uma queda de energia desincroniza os discos, implementamos o fluxo seguro:
1. Registra no log (PENDING).
2. Escreve no Disco 0.
3. Escreve no Disco 1.
4. Marca no log (COMMITTED).

Se ocorrer falha e o passo 4 não rodar, `raid1 recover` reexecutará as transações `COMMITTED` ou descartará parciais, protegendo o espelho.

## CLI

- `init`: Inicializa os discos virtuais.
- `write <bloco> <texto>`: Teste de escrita transacional.
- `read <bloco> <0|1>`: Ler o conteúdo cru de um disco para provar o espelhamento.
- `verify`: Varre byte-a-byte ambos os discos em busca de qualquer anomalia.
- `recover`: Usa o log para resolver falhas detectadas no verify.
- `status`: Exibe informações vitais do Array e do arquivo de Log.
