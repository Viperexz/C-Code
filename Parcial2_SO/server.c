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
#include "split/split.h"

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
	int arregloClientes;
	//Bandera Contadora clientes
	int varCountClientes = 0;
	char * archivosCola;
	int port = 0; //Este debe ingresar por consola
	struct sockaddr_in  addr;
	//IPv4
	arregloClientes = (int *) malloc(sizeof(int) * 20); //Numero maximo de conexiones. 
	archivosCola = char * malloc(sizeof(char)*20);
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

	//comprueba la existencia del directorio, en caso de no existir lo crea
	if(dir()==0) {
		printf("El directorio files existe");
	}
	else {
		printf("Se creo el directorio files");
	}

	//asigna el socket del servidor
	s = socket(PF_INET,SOCK_STREAM,0);

	//if() TODO Crear verificacion del socket 
	
	//Se asocia a una direccion de retorno
	memset(&addr,0,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY; //Direccion estandar 0.0.0.0. "Toma automaticamnente cualquier direccion."
	bind(s,(struct sockaddr *)&addr,sizeof(struct sockaddr_in));
	listen(s,10);
	finished = 0;
	while(!finished) {

		char linearecibida[BUFSIZ];
		if(varCountClientes < 20)
		{
			arregloClientes = accept(s,NULL,0); //Espera una conexion.
		}
		else
		{
			char msjErr[35]; //mensaje de error en caso que el arreglo de clientes este lleno
			int aux = accept(s,NULL,0);
			strcpy(msjErr,"Limite de conexiones alcanzado");
			send(aux,msjErr,BUFSIZ,0);
			close(aux);
		}
		if(arregloClientes[varCountClientes] != 0)
		{
			printf("Cliente %i Conectado", arregloClientes[varCountClientes]); 
			char msjErr[35];
			int aux = accept(s,NULL,0);
			strcpy(msjErr,"Cliente conectado");
			send(aux,msjErr,BUFSIZ,0);
			varCountClientes++;
		}
		recv(arregloClientes[varCountClientes-1],linearecibida,BUFSIZ,0); 		//Recv (socket cliente, donde guardar el mensaje, tamano,0)
		
		char * cadena = strtok(linearecibida," ");

		char comando[10];
		char nombre[50];

		strcpy(comando, cadena);
		strcpy(nombre,cadena);

		if(strcmp(comando,"get")==0) {
			char respuesta[20];
			recibir(nombre,arregloClientes[]);
			strcpy(respuesta,"Entregado... \n");
			send(c,respuesta,BUFSIZ,0);
			//echo("entregado\n");
		}
		else if(strcmp(comando,"put")==0)
		{
			char respuesta[20];
			enviar(nombre,arregloClientes[]);
			strcpy(respuesta,"Enviado... \n");
			send(c,respuesta,BUFSIZ,0);
			//echo("archivo recibido\n");
		}
		sleep(5);
		close(arregloClientes[]);
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

int enviar(char * nombre , int atrSocket) {
    FILE * fp1 ;
    struct stat st;
    stat(nombre,&st);
    int varTamanio,varBandera, varAux,varLeidos,varEnviados;
	long varLongitud,varBuffer;

	//Obtenemos el tamano
    fp1 = fopen(nombre, "r");
	fseek(fp1 , 0 , SEEK_END ); // Se posiciona al final del archivo
	varTamanio = ftell(fp1); // Devuelve el tamaño del archivo (en bytes)
	close(fileno(fp1));
	printf ("tamaño %i\n", varTamanio);
	varLongitud = htonl(varTamanio);

	//Enviamos el tamano del archivo
	if(send(atrSocket, (char *)&varLongitud,sizeof(varLongitud),0)<0)
	{
		printf("Error en envio del tamano del archivo: %s", nombre);
		return 1;
	}
	varBuffer = 1024 * 1024 * 4; //se asignan 4M 
	varLongitud = varTamanio;
	fp1 = fopen(nombre,"rb");

	//Se envia por partes el archivo
	if(fp1!=NULL)
	{
		varBandera = varLongitud;
		varAux = 0;
		while(varAux < varLongitud)
		{
			if(varBandera<varBuffer)
			{
				varBuffer = varBandera;
				char buffer[varBandera];
				char bufferAux[varBandera+1];
			}
			printf("Servidor: Tamano: %d - varBandera %d \n",varBuffer,varBandera);
			varLeidos = fread(buffer,sizeof(char),varBuffer, fp1);
			varEnviados = send(atrSocket, bufferAux,varLeidos,0);
			printf("Servidor:Envie %d bytes \n", varEnviados);
			varBandera = varBandera-varLeidos;
			varAux = varAux + varLeidos;
		}
	}
	else
	{
		printf("Problema con el archivo \n");
		close(fileno(fp1));
	}
	return 0;
}

int recibir(char * nombre , int atrSocket) {
 	FILE * fp1 ;
    struct stat st = {0};
    stat(nombre,&st);
    int varTamanio,varBandera, varAux,varEscritos,varEnviados,varNumeroBytes;
	long varLongitud,varBuffer;	
	varBuffer = 1024 * 1024 * 4;
	
	if (varBuffer >=0){
		fp1= fopen (nombre, "wb");
		varBandera = varLongitud;
		//en este ciclo se recibe el archivo en partes
		varAux = 0;
		while(varAux < varLongitud){
			if (varBandera<varBuffer){
				varBuffer = varBandera;
				char buffer[varBandera];
				char bufferAux[varBandera+1];
			}

			varNumeroBytes = recv(atrSocket, buffer, sizeof(buffer), 0 );
			printf("CLIENTE ---- Recibi %d bytes \n", varNumeroBytes);
			//printf("CLIENTE ---- Recibi %s \n", buffer);
			for (int j=0; j< varNumeroBytes; j++){
				bufferAux[j]= buffer[j];
			}
			bufferAux[varNumeroBytes+1]='\0';
			varEscritos= fwrite(bufferAux, sizeof(char), varNumeroBytes, fp1);
			printf("CLIENTE ---- Recibi %s \n", bufferAux);

			printf("CLIENTE ---- escritos %d\n", varEscritos);

			varBandera = varBandera-varNumeroBytes;
			varAux = varAux+varNumeroBytes;
			printf("CLIENTE ---- Hasta ahora recibi %d bytes \n", varAux);
			if (varNumeroBytes==0) varAux= varLongitud;
		}
	}
	close(fileno(fp1));
}