#include "archivos.h"

int enviar(char * nombre , int atrSocket) {
    	FILE * fp1 ;
	char * dir_archivo;
    	//Se crea la direccion del archivo para agregarlo a .versions
        dir_archivo = malloc( 65 + (strlen(DIR_ARCHIVOS)) + strlen("/") );
        strcat(dir_archivo,DIR_ARCHIVOS);
        strcat(dir_archivo,"/");
        strcat(dir_archivo,nombre);
	struct stat st;
	stat(dir_archivo,&st);
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
	char * dir_archivo;
       //Se crea la direccion del archivo para agregarlo a .versions
        dir_archivo = malloc( 65 + (strlen(DIR_ARCHIVOS)) + strlen("/") );
        strcat(dir_archivo,DIR_ARCHIVOS);
        strcat(dir_archivo,"/");
        strcat(dir_archivo,nombre);
	int varTamanio,varBandera, varAux,varEscritos,varEnviados,varNumeroBytes;
	long varLongitud,varBuffer;	
	varBuffer = 1024 * 1024 * 4;

	if (varBuffer >=0){
		fp1= fopen (dir_archivo, "wb");
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
