#ifndef ARCHIVOS_H
#define ArCHIVOS_H

#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define  DIR_ARCHIVOS ".files"
#define MAX_MEM 1024 * 1024 * 4 // Capacidad de 4M
unsigned char buffer[MAX_MEM];
unsigned char bufferAux[MAX_MEM];

int enviar(char * nombre , int atrSocket);
int recibir(char * nombre , int atrSocket);

#endif
