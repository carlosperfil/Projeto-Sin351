#!/bin/bash
echo "============================================"
echo "  TESTE — Projeto 1: Micro Sistema VSFS"
echo "============================================"
echo ""

echo "[1/7] Formatando disco virtual..."
./vsfs formatar
echo ""

echo "[2/7] Exibindo informacoes do superbloco..."
./vsfs status
echo ""

echo "[3/7] Escrevendo primeiro arquivo..."
./vsfs escrever "Ola Mundo VSFS"
echo ""

echo "[4/7] Escrevendo segundo arquivo..."
./vsfs escrever "Segundo arquivo de teste"
echo ""

echo "[5/7] Listando arquivos salvos..."
./vsfs listar
echo ""

echo "[6/7] Lendo arquivo 0..."
./vsfs ler 0
echo ""

echo "[7/7] Lendo arquivo 1..."
./vsfs ler 1
echo ""

echo "============================================"
echo "  TODOS OS TESTES VSFS CONCLUIDOS!"
echo "============================================"
