#!/bin/bash
echo "============================================"
echo "  TESTE — Projeto 3: Cofre de Senhas"
echo "============================================"
echo ""

echo "[1/3] Criando cofre novo e adicionando senha..."
rm -f cofre_teste.bin
echo -e "MinhaSenha123\n1\ngoogle.com\ncarlos\nG00gl3P@ss\n3\n4" | ./cofre cofre_teste.bin
echo ""

echo "[2/3] Reabrindo cofre e buscando senha..."
echo -e "MinhaSenha123\n2\ngoogle.com\n4" | ./cofre cofre_teste.bin
echo ""

echo "[3/3] Testando senha incorreta..."
echo -e "SenhaErrada\n" | ./cofre cofre_teste.bin
echo ""

# Limpeza
rm -f cofre_teste.bin

echo "============================================"
echo "  TODOS OS TESTES COFRE CONCLUIDOS!"
echo "============================================"
