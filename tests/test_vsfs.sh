#!/bin/bash
echo "=== Testando VSFS ==="
./build/vsfs format
./build/vsfs info
./build/vsfs write "Ola Mundo do arquivo"
./build/vsfs list
./build/vsfs read 0
echo "VSFS OK!"
