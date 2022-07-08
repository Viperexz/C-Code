all:cliente.o archivos.o
	gcc -o cliente cliente.o archivos.o
cliente.o:cliente.c
	gcc -c -o cliente.o cliente.c 
archivos.o:
	gcc -c -o archivos.o archivos.c
clean:
	rm -rf *.o
	rm -rf server cliente 

