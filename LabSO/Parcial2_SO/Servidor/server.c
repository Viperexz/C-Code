#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "archivos.h"

#define DIRECTORIO_ARCHIVOS ".files"
#define MAX_MEM 1024 * 1024 * 4 // Capacidad de 4M
unsigned char buffer[MAX_MEM];
unsigned char bufferAux[MAX_MEM];

int finished;
int dir();
int enviar(char * nombre , int atrSocket);
int recibir(char * nombre , int atrSocket);

int main(int argc,char *  argv[])
{
	//Socket del servidor
	int s;
	//Cliente
	int arregloClientes[20];
	//Bandera Contadora clientes
	int varCountClientes = 0;
	char * archivosCola;
	int port = 0; //Este debe ingresar por consola
	struct sockaddr_in  addr;
	//Se Crea una cola de archivos para verificar que archvivos estan siendo manipulados.
	archivosCola = (char *) malloc(sizeof(char)*20);
	/*
	Esta area solo se dedica para recibir el puerto.
	De no ingresar por parametros, se le pregunta a el usuario.
	*/
	if(argc>=2 )
	{
		port = atoi(argv[1]);
		printf("Puerto en proceso: %i \n",port );
		if(port <=0)
		{
			exit(EXIT_FAILURE);
		}
	}
	else if(port == 0){
		do{
		printf("Debe ingresar un puerto: ");
		scanf("%i",&port);
		}while(port==0);
	}

	//Comprueba la existencia del directorio ".files", en caso de no existir lo crea
	if(dir()==0) {
		printf("El directorio files existe\n");
	}
	else {
		printf("Se creo el directorio files\n");
	}

	//Asigna el socket del servidor
	s = socket(PF_INET,SOCK_STREAM,0);

	//if() TODO Crear verificacion del socket 
	
	//Se asocia a una direccion de retorno
	memset(&addr,0,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY; //Direccion estandar 0.0.0.0. "Toma automaticamnente cualquier direccion."
	bind(s,(struct sockaddr *)&addr,sizeof(struct sockaddr_in));
	//Se deja el servidor en modo escucha.
	listen(s,10);
	//Se establece la bandera finished en 0 para que se repita hasta la senal de finalizacion.
	finished = 0;


	while(!finished) {

		char linearecibida[BUFSIZ];
		if(varCountClientes < 2)
		{
			arregloClientes[varCountClientes] = accept(s,NULL,0); //Espera una conexion.
			printf("Cliente %i Conectado", arregloClientes[varCountClientes]); 
			char msjErr[35];
			strcpy(msjErr,"SERVER: Cliente conectado \n ");
			send(arregloClientes[varCountClientes],msjErr,BUFSIZ,0);
			varCountClientes++;
		}
		else
		{
			char msjErr[35]; //mensaje de error en caso que el arreglo de clientes este lleno
			int aux = accept(s,NULL,0);
			strcpy(msjErr,"Limite de conexiones alcanzado\n");
			send(aux,msjErr,BUFSIZ,0);
			close(aux);
		}
		recv(arregloClientes[varCountClientes-1],linearecibida,BUFSIZ,0); 		//Recv (socket cliente, donde guardar el mensaje, tamano,0)
		//En esta zona se reciben  Comando/peso-archivo/Informacion archivo
		//En caso que el cliente ingrese un put el servidor llamara al metodo "Recibir", escuchando el tamano y el contenido para escribirlo.
		//En caso que el cliente ingrese un get el servidor llamara al medoto "Evniar", enviara el tamano y el contenido.
		char * cadena = strtok(linearecibida," ");
		char comando[10];
		char nombre[50];
		strcpy(comando, cadena);
		strcpy(nombre,cadena);
		if(strcmp(comando,"put")==0) {
			char respuesta[20];
			recibir(nombre,arregloClientes[varCountClientes-1]);
			strcpy(respuesta,"Entregado... \n");
			send(arregloClientes[varCountClientes-1],respuesta,BUFSIZ,0);
			//echo("entregado\n");
		}
		else if(strcmp(comando,"get")==0)
		{
			char respuesta[20];
			enviar(nombre,arregloClientes[varCountClientes-1]);
			strcpy(respuesta,"Enviado... \n");
			send(arregloClientes[varCountClientes-1],respuesta,BUFSIZ,0);
			//echo("archivo recibido\n");
		}
		sleep(5);
		close(arregloClientes[varCountClientes-1]);
	}
	exit(EXIT_SUCCESS);
}

//revisa si existe una carpeta .files de no ser así la crea
int dir()
{
    struct stat s;
    if(stat(".files", &s)>0 || S_ISDIR(s.st_mode))
    {
        return 0;
    }
    else
    {
        mkdir(DIRECTORIO_ARCHIVOS,S_IRWXU);
        return 1;  
    }
    return 0;
}
