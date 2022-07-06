all:cliente.o server.o
	gcc -o server server.o -pthread
	gcc -o cliente cliente.o
cliente.o:cliente.c
	gcc -c -o cliente.o cliente.c
server.o:server.c
	gcc -c -o server.o server.c
clean:
	rm -rf *.o
	rm -rf server cliente 

