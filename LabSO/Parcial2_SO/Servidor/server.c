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
#include <pthread.h>


#define DIR_ARCHIVOS ".files"
#define MAX_MEM 1024 * 1024 * 4 // Capacidad de 4M
unsigned char buffer[MAX_MEM];
unsigned char bufferAux[MAX_MEM];

int finished;
int dir();
int enviar(char * nombre , int atrSocket);
int recibir(char * nombre , int atrSocket);

int sendA(char * nombre, int atrSocket);
int recvA(char * nombre, int atrSocket);

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
		/* establecer conexion
		 * parametros:
		 * int * arregloClientes[20]
		 * int * s;
		 * int * varCountClientes
		 */
		char linearecibida[BUFSIZ];
		if(varCountClientes < 10)
		{
			arregloClientes[varCountClientes] = accept(s,NULL,0); //Espera una conexion.
			printf("Cliente %i Conectado", arregloClientes[varCountClientes]); 
			char msjErr[40];
			strcpy(msjErr,"SERVER: Cliente conectado \n ");
			send(arregloClientes[varCountClientes],msjErr,BUFSIZ,0);
			varCountClientes++;
		}
		else
		{
			char msjErr[35]; //mensaje de error en caso que el arreglo de clientes este lleno
			int aux = accept(s,NULL,0);
			strcpy(msjErr,"Limite");
			send(aux,msjErr,BUFSIZ,0);
			close(aux);
		}

		char comando[10];
		recv(arregloClientes[varCountClientes],comando,BUFSIZ,0); 		//Recv (socket cliente, donde guardar el mensaje, tamano,0)
		//En esta zona se reciben  Comando/peso-archivo/Informacion archivo
		//En caso que el cliente ingrese un put el servidor llamara al metodo "Recibir", escuchando el tamano y el contenido para escribirlo.
		//En caso que el cliente ingrese un get el servidor llamara al medoto "Evniar", enviara el tamano y el contenido.
		char nombre[BUFSIZ];
		if(strcmp(comando,"put")==0) {


			/*char respuesta[20];
			recv(arregloClientes[varCountClientes],nombre,BUFSIZ,0); 
			recibir(nombre,arregloClientes[varCountClientes]);
			strcpy(respuesta,"Entregado: ");
			strcpy(respuesta,nombre);
			send(arregloClientes[varCountClientes],respuesta,BUFSIZ,0);
			//echo("entregado\n");*/


		}
		else if(strcmp(comando,"get")==0)
		{


			/*recv(arregloClientes[varCountClientes],nombre,BUFSIZ,0); 
			char respuesta[20];
			enviar(nombre,arregloClientes[varCountClientes]);
			strcpy(respuesta,"Recibido: ");
			strcpy(respuesta,nombre);
			send(arregloClientes[varCountClientes],respuesta,BUFSIZ,0);
			//echo("archivo recibido\n");*/
		}
		sleep(5);
		close(arregloClientes[varCountClientes-1]);
	}
	exit(EXIT_SUCCESS);
}

//revisa si existe una carpeta .files de no ser así la crea
int dir() {
    struct stat s;
    if(stat(".files", &s)>0 || S_ISDIR(s.st_mode))
    {
        return 0;
    }
    else
    {
        mkdir(DIR_ARCHIVOS,S_IRWXU);
        return 1;  
    }
    return 0;
}

int enviar(char * nombre , int atrSocket) {
    FILE * fp1 ;
    struct stat st;
	char * dir_archivo;
	dir_archivo = malloc( 65 + (strlen(DIR_ARCHIVOS)) + strlen("/") );
    strcat(dir_archivo,DIR_ARCHIVOS);
    strcat(dir_archivo,"/");
    strcat(dir_archivo,nombre);
	if(stat(dir_archivo, &st)<0 || !S_ISREG(st.st_mode)) {
        perror("stat");
    }
    int varTamanio,varBandera, varAux,varLeidos,varEnviados;
	long varLongitud,varBuffer;

	//Obtenemos el tamano
    fp1 = fopen(dir_archivo, "r");
	fseek(fp1 , 0 , SEEK_END ); // Se posiciona al final del archivo
	varTamanio = ftell(fp1); // Devuelve el tamaño del archivo (en bytes)
	close(fileno(fp1));
	printf ("tamaño %i\n", varTamanio);
	varLongitud = htonl(varTamanio);

	//Enviamos el tamano del archivo
	if(send(atrSocket, (char *)&varLongitud,sizeof(varLongitud),0)<0) {
		printf("Error en envio del tamano del archivo: %s\n", dir_archivo);
		return 1;
	}
	varBuffer = 1024 * 1024 * 4; //se asignan 4M 
	varLongitud = varTamanio;
	fp1 = fopen(dir_archivo,"rb");

	//Se envia por partes el archivo
	if(fp1!=NULL) {
		varBandera = varLongitud;
		varAux = 0;
		while(varAux < varLongitud)	{
			if(varBandera<varBuffer) {
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
	else {
		printf("Problema con el archivo \n");
		close(fileno(fp1));
	}
	return 0;
}

int recibir(char * nombre , int atrSocket) {
 	FILE * fp1 ;
    struct stat st = {0};
	char * dir_archivo;
    
	dir_archivo = malloc( 65 + (strlen(DIR_ARCHIVOS)) + strlen("/") );
    strcat(dir_archivo,DIR_ARCHIVOS);
    strcat(dir_archivo,"/");
    strcat(dir_archivo,nombre);
	stat(dir_archivo,&st);
    int varTamanio,varBandera, varAux,varEscritos,varEnviados,varNumeroBytes;
	long varLongitud,varBuffer;
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


int sendA(char * nombre, int atrSocket)
{
	FILE * file;
	//Verificar si el archivo ingresado existe 
	struct stat st;
	
	if(stat(nombre,&st))
	{
		perror("stat");
		return 1;
	}
	file = fopen(nombre, "rb");
    if(file == NULL)
    {
        return 0;
    }

	//Recupera el tamano del archivo
	int varTamano; 
	varTamano = st.st_size;
	char tempTamano =(char)varTamano;

	send(atrSocket,varTamano,20,0);
	char * buffer[varTamano];
	int varRta= fread(buffer,1,varTamano,file);

	send(atrSocket,buffer,varTamano,0);
}


int recvA(char * nombre, int atrSocket)
{
	//Recibimos el tamano
	char tempTamano[20];
	int varTamano; 
	recv(atrSocket,tempTamano,20,0);
	varTamano = atoi(tempTamano);

	//Recibimos el contenido
	char * buffer[varTamano];
	recv(atrSocket,buffer,varTamano,0);

	FILE * file;
	//Verificar si el archivo ingresado existe 
	struct stat st;
	
	if(stat(nombre,&st))
	{
		perror("stat");
		return 1;
	}
	file = fopen(nombre, "wb");
    if(file == NULL)
    {
        return 0;
    }
	//Recupera el tamano del archivo
	varTamano = st.st_size;
	fwrite(buffer,1,varTamano,file);
}
