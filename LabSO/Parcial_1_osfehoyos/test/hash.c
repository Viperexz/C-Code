#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

char * obtener_hash(char * nombre_archivo);

int main(int argc, char * argv[])
{
    char * hash;
    if (argc != 2)
    {
        fprintf(stderr,"Debe especificar el nombre del archivoo \n");
        exit(EXIT_FAILURE);
    }
    
    hash = obtener_hash(argv[1]);
     if(hash != NULL)
    {
    printf("Hash de %s: %s\n", argv[1], hash);
    }
    else{
        fprintf(stderr,"No se puede calcular el hash %s \n", argv[1]);
    }
    if(hash != NULL)
    {
        free(hash);
    }
    exit(EXIT_SUCCESS);
}

char * obtener_hash(char * nombre_archivo)
{
    char * hash = NULL;
    char * comando;
    FILE * fp;
    struct stat s;

    //Verificacion si el archivo existe y es obtenible.
    if(stat(nombre_archivo, &s)<0 || !S_ISREG(s.st_mode))
    {
        perror("stat");
        return hash;
    }

    int len = strlen("sha256sum ");

    //Comando Sha256sum + nombre del archivo
    comando = malloc(len +strlen(nombre_archivo)+ 1);
    //Salida Estandar 
    sprintf(comando,"sha256sum %s", nombre_archivo);
    
    //Abrimos un flujo de datos
    fp = popen(comando, "r");
 
    //Abrir un flujo con el coando sha256sum ARCHIVO
    
    if(fp == NULL)
    {
        return hash;
    }

    //Reservar memoria para el Hash 
    hash = malloc(65);

    fgets(hash,65,fp);
    if(strlen(hash) == 64)
    {
        pclose(fp);
        free(comando);
        return hash;
    }
    else
    {
        pclose(fp);
        free(hash);
        return NULL;
    }
}