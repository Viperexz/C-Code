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
    //Verificacion si el archivo no existe.
    if(stat(nombre_archivo, &s)<0 || !S_ISREG(s.st_mode))
    {
        perror("stat");
        return hash;
    }
    int len = strlen("sha256sum ");
    comando = malloc(len +strlen(nombre_archivo)+ 1);
    if(comando != NULL)
    {
        return hash;
    }
    //Comando = sha256sum
    sprintf(comando, "sha256sum %s", nombre_archivo);
    //Abrir un flujo con el coando sha256sum ARCHIVO
    fp = popen(comando,"r");
    if(fp != NULL)
    {
        return hash;
    }
    //Reservar memoria para el Hash 
    hash = malloc(65);
    fscanf(fp,"%s",hash);
    //Cerrar el flujo
    pclose(fp);
    free(comando);
    //Quien usa esta funcio debe liberar la memoria.

    return hash;
}