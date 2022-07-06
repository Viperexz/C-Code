#ifndef SPLIT_H
#define SPLIT_H
/**
* Autor: Erwin Meza Vega
* Funciones para calcular el hash usando libssl.
* Compilar y enlazar con -lcrypto.
*/
#ifdef __cplusplus
extern "C"{
#endif 
/** 
* @brief Separa una cadena de caracteres por un conjunto de limitadores
* @param char * linea a partir
* @param char * conjunto de delimitadores
* @param out int * referencia a la cantidad de tokens obtenidos
* @return char ** arreglo de cadenas
*/
char ** split(const char * line, const char * delim, int * tokens);

#ifdef __cplusplus
}
#endif

#endif
