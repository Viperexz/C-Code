all: main.o
	gcc -o versions main.o
main.o: main.c
	gcc -c main.o main.c

clean:
	rm -f versions main.o

install: all
	sudo cp versions /usr/local/bin

uninstall:
	sudo rm -f /usr/local/bin/versions