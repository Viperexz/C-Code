//SERVIDOR
int s, s1; //transfiere por s y escucha por s1
char *buf;
struct sockaddr_in server;
struct sockaddr_in cliente;
struct hostent *server_host;
int tam;
long funcion;
long longList;
int i,j;
unsigned char buffer[512];
unsigned char bufferAux[513];
FILE * flista;
int leidos;
long falta;
long tam_buffer;
int enviados;
buf = malloc(max);
tam = sizeof(struct sockaddr_in);
/*********************CONECTA CON SERVIDOR*************************/
if ((s = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
printf("No se pudo abrir el socket\n");
exit(1);
}
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons((short)NumeroPuerto);
bzero(&(server.sin_zero),8);
if ((bind(s,(struct sockaddr*)&server,tam)) < 0){
printf("Error en bind\n");
exit(1);
}
if (listen(s, 2) < 0){
printf("Error en listen\n");
exit(1);
}
while(1){
if((s1=accept(s,(struct sockaddr *)&cliente,&tam)) < 0){
printf("Error en accept\n");
exit(1);
}
/*********************ENVIAR ARCHIVO*************************/
FILE * fd = fopen(listaarchs2 , "r" );
fseek( fd , 0 , SEEK_END ); // Se posiciona al final del archivo
int tamanio = ftell(fd); // Devuelve el tamaño del archivo (en bytes)
close(fd);
printf ("tamaño %i\n", tamanio);
longList = htonl(tamanio); //asi envia la longitud del archivo
if((send(s1, (char*)&longList, sizeof(longList), 0)) < 0){
printf("Error en envio\n");
exit(1);
}
tam_buffer=512;
longList= tamanio; //tamaño del archivo

flista= fopen ("listaarchs2.txt", "rb");
if (flista!=NULL){
falta = longList;
// Se envia el archivo en partes
i = 0;
while(i < longList){
if (falta<tam_buffer) {
tam_buffer=falta;
char buffer[falta];
char bufferAux[falta+1];
}
printf("SERVIDOR ---- tam_buffer: %d - falta: %d\n", tam_buffer, falta);

leidos = fread(buffer, sizeof(char),tam_buffer,flista);

enviados= send(s1, bufferAux, leidos, 0);
printf("SERVIDOR ---- Envie %d bytes \n", enviados);

falta = falta-leidos;
i = i + leidos;
}
}
else printf("SERVIDOR ---- No abrio el archivo \n");
close(flista);
close(s1);
}
close(s);

//CLIENTE
struct sockaddr_in server; // Direccion del peer donde se conectara
struct hostent *server_host;
int tam;
char *buf;
int s;
long longlista;
int i,j;
char ch;
unsigned char buffer[512];
unsigned char bufferAux[513];
FILE *flista;
int numbytes;
long falta;
long tam_buffer;
int escritos;
long longlista; //tamaño del archivo a transferir

/*********************CONECTA CON SERVIDOR*************************/
tam= sizeof(struct sockaddr_in);
buf = malloc(max);
if ((server_host = gethostbyname(ip)) == 0){
printf("No se encuentra el host\n");
}
server.sin_family= AF_INET;
bcopy(server_host->h_addr,&server.sin_addr,server_host->h_length);
server.sin_port= htons((short)40121);
bzero(&(server.sin_zero),8);
if ((s = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
printf("No se pudo abrir el socket\n");
exit(1);
}
if ((connect(s,(struct sockaddr*)&server,tam)) < 0){
//printf("Error al intentar conectarnos al servidor.\n");
//exit(1);
}
recv(s, (char *)&buf, max, 0); // asi recibe la long de la lista de archivos
longlista= ntohl(buf);

/*********************RECIBIR ARCHIVO*************************/
tam_buffer= 512;
if (longlista >=0){
flista= fopen ("listaarchs.txt", "wb");
falta = longlista;
//en este ciclo se recibe el archivo en partes
i = 0;
while(i < longlista){
if (falta<tam_buffer){
tam_buffer = falta;
char buffer[falta];
char bufferAux[falta+1];
}

numbytes = recv(s, buffer, sizeof(buffer), 0 );
printf("CLIENTE ---- Recibi %d bytes \n", numbytes);
//printf("CLIENTE ---- Recibi %s \n", buffer);
for (j=0; j< numbytes; j++){
bufferAux[j]= buffer[j];
}
bufferAux[numbytes+1]='\0';
escritos= fwrite(bufferAux, sizeof(char), numbytes, flista);
printf("CLIENTE ---- Recibi %s \n", bufferAux);

printf("CLIENTE ---- escritos %d\n", escritos);

falta = falta-numbytes;
i = i+numbytes;
printf("CLIENTE ---- Hasta ahora recibi %d bytes \n", i);
if (numbytes==0) i= longlista;
}
}
close(flista);
}


