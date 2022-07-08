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



/*
Para multiples clientes
https://poesiabinaria.net/2011/02/creando-un-servidor-que-acepte-multiples-clientes-simultaneos-en-c/ 
*/


#define DIRECTORIO_ARCHIVOS ""
#define MAX_MEM 1024 * 1024 * 4 // Capacidad de 4M
unsigned char buffer[MAX_MEM];
unsigned char bufferAux[MAX_MEM];

char * archivo(char * old_nombre );
int enviar(char * nombre , int atrSocket);
int recibir(char * nombre , int atrSocket);
int finished;

/*
TODO: Integrar los hilos y semaforos.
Recordar que se debe validar que los clientes no intenten subir el mismo archivo se presntara una condicion de carrera.
Al conectarse o desconectarse los mismos

Se debe cambiar la firma de los metodos para que sea de tipo void * "Funciones de hilos".

Verificar las secciones criticas:
1. Conexion de un usuario.
2. Acceso a un archivo.
3. Desconexion y liberacion de recursos
*/

/**
 * @brief 
 * Argc [2]
 * argv = Ejecutable
 * argv[2]=Puerto
 */
int main(int argc,char *  argv[]) {
	
	//VARIABLES
	int finished = 0;
	int s;
	
	//Cliente
	int c;
	int port = 0;
	char * ip = "127.0.0.1";
	
	//Socket del servidor
	struct sockaddr_in  addr;

	//Mensajes
	char comando[BUFSIZ];
	char respuesta[BUFSIZ];

	//Metodos
	if(argc>=2 ) {
		port = atoi(argv[1]);
		printf("Puerto en proceso: %i \n",port );
		if(port <=0) {
			exit(EXIT_FAILURE);
		}
	} 
	else if(port == 0) {
		do{
		printf("Debe ingresar un puerto: ");
		scanf("%i",&port);
		}while(port==0);
	
	}

	printf("Puerto configurado: %i \n",port );
	s = socket(PF_INET,SOCK_STREAM,0);

	if(s == -1 ) {
		exit(EXIT_FAILURE);
	}

	//Se asocia a una direccion de retorno
	memset(&addr,0,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY; //Direccion estandar 0.0.0.0. "Toma automaticamnente cualquier direccion."

	//IPv4
	inet_aton(ip,&addr.sin_addr);
	
	if(connect(s,(struct sockaddr *)&addr,sizeof(struct sockaddr_in))<0) {
		printf("Error al conectar.");
		exit(EXIT_FAILURE);
	}

	printf("Conectando a %s:%i ... \n",ip,port);
	recv(s,respuesta,BUFSIZ,0);

	if(strcmp(respuesta,"Limite de conexiones alcanzado")==0) {	
		printf("%s",respuesta);
		exit(EXIT_FAILURE);
	}

	printf("%s",respuesta);

	while(!finished) {

		//TODO: Crear una funcion con este metodo:
		//Esta area escucha los comandos ingresados por consola .
		//Recuerda si el usuario ingresa:
		//get : El cliente debe llamar al metodo  recibir para hacer la transferencias de archivos
		//put : El cliente debe llamar al metodo enviar para empezar la transferencia de informacion.
		memset(comando,0,BUFSIZ);
		printf("Ingrese el comando. (help =  ayuda)\n");
		printf(">");
		//permite leer una cadena de texto con espacios y agrega un salto de linea
		fgets(comando,BUFSIZ,stdin);
		//elimina el salto de linea con este metodo para no usar la funcion peligrosa "gets"
		comando[strcspn(comando, "\n")] = 0;
		
		//variable usada para separar la cadena de texto con la funcion strtok
		char * token = strtok(comando," ");
		//vector de cadenas usado para almacenar los tokens o partes de la cadena separada
		char * tokens[3];
		//auxiliar contador para llenar la variable tokens
		int i = 0;

		//ciclo while para el llenado del vector tokens con las partes de la cadena
		while(token!=NULL){
			tokens[i++] = token;
			token = strtok(NULL," ");
		}

		/*
		 * tokens[0]: contiene la instruccion
		 * tokens[1]: contiene el nombre del archivo a recibir o enviar
		 */
		
		//en caso de que la instruccion sea "help" imprime un menu de ayuda
		if(strcmp(tokens[0],"help")==0) {
			printf("=============================== HELP =============================== \n");
			printf("Puedes seleccionar uno de los siguientes comandos: \n");
			printf("get: Para recuperar los archivos del servidor. \n");
			printf("puts:  Para enviar archivos al servidor. \n");
			printf("exit: Finaliza el proceso cliente. \n");
		}
		//en caso de que se haya ingresado un nombre y la instruccion sea "get" o "put"...
		else if(tokens[1] != NULL && (strcmp(tokens[0],"get")==0 || strcmp(tokens[0],"put")==0)) {

			printf("Ejecutando: (%s) ...\n", tokens[0]);
			//Cambiar comando por archivo("Nombre del archivo");
			//Para que le entregue un puntero con toda la informacion del archivo
			int value = send(s,comando,BUFSIZ,0);

			if(value ==-1) {
				printf("Error en el envio."); 
			}

			printf("Se enviaron %i bytes \n",value);

			if(strcmp(tokens[0],"get")==0) {
				recibir(tokens[1],s);
			} else if(strcmp(tokens[0],"get")==0) {
				enviar(tokens[1],s);
			}

			recv(s,respuesta,BUFSIZ,0);
			printf("%s",respuesta);
		}
		else if(strcmp(tokens[0],"exit")==0) {

			finished = 1;
			close(s);
			printf("Desconectado \n");
			
		}
		else {
			printf("Comando no disponible \n");
		}
	}
	exit(EXIT_SUCCESS);
}

//TODO: Dividir estos metodos en .h para  mejorar la simplicidad del trabajo.
//Para probarlos se puede solicitar las dos tokens separadas. 

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
