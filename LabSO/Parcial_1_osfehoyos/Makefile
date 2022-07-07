all: main.o
	gcc -o versions main.o

main.o: main.c
	gcc -c -o main.o main.c

clean:
	rm -rf versions main.o .versions

install: all
	sudo cp versions /usr/local/bin

uninstall:
	sudo rm -f /usr/local/bin/versions