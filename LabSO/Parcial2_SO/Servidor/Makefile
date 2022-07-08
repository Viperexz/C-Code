all:server.o archivos.o
	gcc -o server server.o archivos.o -pthread
server.o:server.c
	gcc -c -o server.o server.c
archivos.o:
	gcc -c -o archivos.o archivos.c
clean:
	rm -rf *.o
	rm -rf server
	 
	
