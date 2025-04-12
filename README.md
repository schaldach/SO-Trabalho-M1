# SO-trabalho-M1

passo-a-passo pra executar:
1° rodando o servidor
"gcc -o servidorbin servidor.c -lrt -lpthread"
"./servidorbin"

2° rodando o cliente (em um terminal separado)
"gcc -o clientebin cliente.c -lrt -lpthread
"./clientebin" (por favor usar bin como sufixo pra não ser comitado)

dai ele vai esperar para você digitar o comando (por enquanto, iremos mudar depois)
os 4 tipos de comandos são:

"INSERT id=9 nome=gabriel"
"UPDATE id=9 nome=jonas"
"SELECT id=9"
"DELETE id=9"