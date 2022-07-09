all:server.o
	gcc -o server server.o -pthread
server.o:server.c
	gcc -c -o server.o server.c
clean:
	rm -rf *.o
	rm -rf server cliente 

