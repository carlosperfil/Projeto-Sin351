#!/bin/bash
echo "=== Testando Cofre ==="
rm -f cofre_teste.bin
echo -e "senha123\n1\ngoogle.com\nuser\npass\n3\n4\n" | ./build/cofre cofre_teste.bin
echo "Testando login..."
echo -e "senha123\n2\ngoogle.com\n4\n" | ./build/cofre cofre_teste.bin
echo "Cofre OK!"
