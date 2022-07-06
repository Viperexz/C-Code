#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split.h"

int main(int argc, char * argv[])
{
	int finished = 0;
	char comando[BUFSIZ];
	printf("Tamano Bufsize: %i ",BUFSIZ);
	while(!finished)
	{
		printf("Ingrese el comando. (help =  ayuda)\n");
		printf(">");
		memset(comando,0,BUFSIZ);
		fgets(comando,0,BUFSIZ);//Lee maximo BUFSIZ-1 Caracteres de la entrada estandar
		printf("Leido: %s\n",comando);
		
		split_list * l; 
		l = split(comando, "/n/r/t");
		if(l->parts[0], "exit")
		{
			continue;
		}
		if(strcmp(l->parts[0],"exit")==0)
		{
			finished =1;
		}
	}


	exit(EXIT_SUCCESS);
}
