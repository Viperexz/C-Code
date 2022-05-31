#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct{
    char nombre_archivo[BUFSIZ];
    char hash[65];
    char comentario[BUFSIZ];
}version;

#define DIRECTORIO_VERSIONS ".versions"
#define VERSIONS_FILE ".versions/versions.db"
#define MAX_MEM 1024 * 1024 * 4 // Capacidad de 4M

int add (char * nombre_archivo, char * comentario);
int list (char * nombre_archivo);
int dir();
char * obtener_hash(char * nombre_archivo);
int guardar(version * v);
int copiar(char * old_nombre ,char * new_name );

int main(int argc, char * argv[])
{
    dir();
    //Opciones a programar.
    //ADD -> Agregar "archivo" "comentario" 
    //Se obtiene el hash  del archivo y se copia a la carpeta con el nombre hash .versions 
    //Se guarda en la base de datos.
    //list -> list "archivo" 
    //Se obtiene un listado del las versiones de un archivo en especifico
    //hash = "nombre_archivo" "comentario"
    if(argc > 2)
    {
        if(argc == 4)
        {
            if(strcmp(argv[1],"add")==0)
            {
                add(argv[2],argv[3]);
            }
            else if(strcmp(argv[1],"list")==0)
            {
                list(argv[2]);
            }
        }
        if(argc == 3)
        {
            if ( argc == 1 &&strcmp(argv[1],"list")==0)
            {
                list(argv[2]);
            }else if(strcmp(argv[1],"add")==0){
                printf("Argumento faltante: ./versions add archivo comentario \n");
            }
        }
    }
    else if (argc == 2)
    {
        if(strcmp(argv[1],"add")==0)
        {
            printf("Argumento faltante: ./versions add archivo comentario \n");
        }
        else if(strcmp(argv[1],"list")==0)
        {
            printf("Argumento faltante: ./versions list archivo \n");
        }
        else{
            printf("Opcion inexistente");
        }
    }

}

int add (char * nombre_add, char * comentario_add)
{
    char * hash_add;
    char * dir_archivo;
    hash_add= obtener_hash(nombre_add);
    if(hash_add != NULL)
    {
        printf("Hash de %s: %s\n", nombre_add, hash_add);
        version * v;
        v = (version*)malloc(sizeof(version));
        strcpy(v->nombre_archivo,nombre_add);
        strcpy(v->hash,hash_add);
        strcpy(v->comentario, comentario_add);
        //Se crea la direccion del archivo para agregarlo a .versions
        dir_archivo = malloc( 65 + (strlen(DIRECTORIO_VERSIONS)) + strlen("/") );
        strcat(dir_archivo,DIRECTORIO_VERSIONS);
        strcat(dir_archivo,"/");
        strcat(dir_archivo,hash_add);
        copiar(nombre_add,dir_archivo);
        if(!guardar(v))
        {
            fprintf(stderr,"No se puede guardar. \n");
            return 1;
        }
        return 0;
    }
    else{
        fprintf(stderr,"No se puede calcular el hash %s \n", nombre_add);
    }
    if(hash_add != NULL)
    {
        free(hash_add);
    }
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


 
int list (char * nombre_archivo)
{
    //Hay que listar la informacion del archivo .db 
    printf("Salida 2 %s \n", nombre_archivo );
}

//Validar si el directorio .versions existe si no crearlo.
int dir()
{
    struct stat s;
    if(stat(".versions", &s)>0 || S_ISDIR(s.st_mode))
    {
        return 0;
    }
    else
    {
        mkdir(DIRECTORIO_VERSIONS,S_IRWXU);
        return 1;  
    }
    return 0;
}
//Copia la informacion del archivo 
//old_nombre archivo origen
//new_name archivo destino "Lo crea si no existe"
int copiar(char * old_nombre ,char * new_name )
{
    FILE * fp1;
    FILE * fp2;
    struct stat st = {0};
    stat(old_nombre,&st);
    int max;

    fp1 = fopen(old_nombre, "rb");

    if(fp1 == NULL)
    {
        return 0;
    }

    fp2 = fopen(new_name, "wb");

    if(fp2 == NULL)
    {
        return 0;
    }
    
    if(st.st_size<MAX_MEM)
    {
        max = st.st_size;
    }
    else
    {
        max= MAX_MEM;
    }
         char * buf = malloc (max); // Tan grande como el archivo, el espacio del montón se asigna tanto como sea posible
   
    while(1)
    {
           int rc = fread(buf,1,max,fp1);
           if(rc == 0)break;
           fwrite(buf,1,rc,fp2);
    }

    fclose(fp1);
    fclose(fp2);
    free(buf);
    printf("Copiado");
    return 0;
}


//TODO - Crear un recuperar versiones "Listar versiones y 
//leer el archivo guardado en .versions y copiarlos sobre el el nombre del existente"