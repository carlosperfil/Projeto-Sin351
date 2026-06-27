#!/bin/bash
echo "============================================"
echo "  TESTE — Projeto 1: Micro Sistema VSFS"
echo "============================================"
echo ""

echo "[1/7] Formatando disco virtual..."
./vsfs format
echo ""

echo "[2/7] Exibindo informacoes do superbloco..."
./vsfs info
echo ""

echo "[3/7] Escrevendo primeiro arquivo..."
./vsfs write "Ola Mundo VSFS"
echo ""

echo "[4/7] Escrevendo segundo arquivo..."
./vsfs write "Segundo arquivo de teste"
echo ""

echo "[5/7] Listando inodes alocados..."
./vsfs list
echo ""

echo "[6/7] Lendo inode 0..."
./vsfs read 0
echo ""

echo "[7/7] Lendo inode 1..."
./vsfs read 1
echo ""

echo "============================================"
echo "  TODOS OS TESTES VSFS CONCLUIDOS!"
echo "============================================"
