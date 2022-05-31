#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct{
    char nombre_archivo[PATH_MAX];
    char hash[65];
    char comentario[BUFSIZ];
}version;

#define VERSIONS_FILE "versions.db"

int guardar(version * v);

int main(int argc, char * argv[])
{
    version * v;
    v = (version*)malloc(sizeof(version));
    strcpy(v->nombre_archivo,"hola.c");
    strcpy(v->hash,"ab91e52865ab07b20e71f282ed6a545227cab5b37648b06f61fe7504acbce77c");
    strcpy(v->comentario, "Primera Version");

    if(!guardar(v))
    {
        fprintf(stderr,"No se puede guardar. \n");
    }
    exit(EXIT_SUCCESS);
}

int guardar(version * v){
    FILE * fp;
    fp = fopen(VERSIONS_FILE, "a");
    if(fp == NULL)
    {
        return 0;
    }
    if(fwrite(v, sizeof(version),1,fp) != 1)
    {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}


