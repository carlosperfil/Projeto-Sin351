# Cofre de Senhas Criptografado

O **Cofre de Senhas** protege "dados em repouso" (no disco) por meio de uma arquitetura estrita de criptografia simétrica aliada à segurança de acesso à memória RAM.

## Fluxo de Autenticação e Derivação

O sistema nunca armazena a senha do usuário.
1. Uma master password é submetida na CLI.
2. É gerado um `salt` de 32 bytes usando o URANDOM do OpenSSL.
3. Usamos **PBKDF2-HMAC-SHA256** com 10.000 iterações para obter a chave AES `session_key`.
4. Um hash SHA256 do `session_key` é guardado no arquivo para comprovar se a senha digita está correta sem precisar tentar descriptografar dados.

## Criptografia do Cofre
A área de dados é inteiramente protegida com o OpenSSL `EVP_aes_256_cbc`. Um vetor de inicialização (IV) novo é criado aleatoriamente *cada vez* que o cofre é salvo, garantindo segurança mesmo se a senha permanecer idêntica.

## Segurança da Memória (Wipe)
Sempre que uma informação crítica não for mais necessária (senha limpa em texto, buffers decifrados e chaves derivadas da stack), invocamos uma função customizada `secure_wipe` que força um sobrescrevimento com zeros e anula vestígios deixados na memória volátil (RAM).

## Como compilar / usar
Para compilar: `make cofre`.
O programa requer links à biblioteca Crypto (`-lssl -lcrypto`).

`./build/cofre minhasenhas.bin` abre o console interativo protegido.
