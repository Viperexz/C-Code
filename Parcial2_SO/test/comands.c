
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "split.h"


int finished;

void handle_sigterm(int);

int main(int argc, char * argv[])
{
	finished = 0;
	char comando[BUFSIZ];
	struct sigaction  act;
	struct sigaction oldact;
	printf("Tamano Bufsize: %i ",BUFSIZ);
	memset(&act,0,sizeof(struct sigaction));
	memset(&oldact,0,sizeof(struct sigaction));
	act.sa_handler = handler_sigterm;
	signaction(SIGTERM,&act,&oldact);
	while(!finished)
	{
		printf("Ingrese el comando. (help =  ayuda)\n");
		printf(">");
		//Rellenar con 0 'Limpiar'
		memset(comando,0,BUFSIZ);
		//Lee maximo BUFSIZ-1 Caracteres de la entrada estandar
		fgets(comando,BUFSIZ,stdin);
		printf("Leido: %s\n",comando);
		split_list * l; 
		l = split(comando, " \n\r\t");
		if(l->parts[0], "exit")
		{
			continue;
		}
		if(strcmp(l->parts[0],"exit")==0)
		{
			finished =1;
		}
		else if(strcmp(l->parts[0],"get")==0) && l->count == 2)
		{
			printf("Obtener, %s\n",l->parts[1]);
		}
	}
	exit(EXIT_SUCCESS);
}

void handle_sigterm(int sig)
{
	printf("SIGTERM Recibida %i\n",sig);
	finished = 1;
	//Cerrar los recursos solicitados.
	fclose(stdin);
}
