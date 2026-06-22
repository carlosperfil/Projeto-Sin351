#!/bin/bash
echo "=== Testando RAID-1 ==="
./build/raid1 init
./build/raid1 write 0 "Dados Espelhados e Seguros"
./build/raid1 verify
./build/raid1 status
./build/raid1 read 0 0
./build/raid1 read 0 1
echo "RAID-1 OK!"
