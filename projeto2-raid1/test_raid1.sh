#!/bin/bash
echo "============================================"
echo "  TESTE — Projeto 2: Gerenciador RAID-1"
echo "============================================"
echo ""

echo "[1/8] Inicializando array RAID-1..."
./raid1 init
echo ""

echo "[2/8] Escrita espelhada no bloco 0..."
./raid1 write 0 "Dados espelhados RAID"
echo ""

echo "[3/8] Escrita espelhada no bloco 5..."
./raid1 write 5 "Bloco 5 com mais dados"
echo ""

echo "[4/8] Leitura do Disco 0, Bloco 0..."
./raid1 read 0 0
echo ""

echo "[5/8] Leitura do Disco 1, Bloco 0 (espelho)..."
./raid1 read 0 1
echo ""

echo "[6/8] Verificando integridade..."
./raid1 verify
echo ""

echo "[7/8] Status do array..."
./raid1 status
echo ""

echo "[8/8] Recuperacao via WAL..."
./raid1 recover
echo ""

echo "============================================"
echo "  TODOS OS TESTES RAID-1 CONCLUIDOS!"
echo "============================================"
