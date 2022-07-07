#include <stdlib.h>
#include <string.h>
#include "split.h" 

/** @brief Delimitadores por defecto */
const char * split_default_delimiters = " \t\r\n";

/** @brief Lista de tokens */
struct token {
  /** @brief Caracteres que conforman el token */
  char * chars;
  /** @brief Apuntador al siguiente token */
  struct token * next;
};

/** 
* @brief Separa una cadena de caracteres por un conjunto de limitadores
* @param char * linea a partir
* @param char * conjunto de delimitadores
* @param out int * referencia a la cantidad de tokens obtenidos
* @return char ** arreglo de cadenas
*/
char ** split(const char * line, const char * delim, int * tokens) {
  const char * l_ptr;
  const char * aux;
  const char * d_ptr;
  int nchars;
  int n;
  int token_count;
  char * token;
  char ** ret;
  int i;
  char * str;

  struct token * t_head;
  struct token * t_tail;
  struct token * tok;

  //Validar cadenas nulas en la entrada
  if (line == NULL) {
    *tokens = 0;
    return NULL;
  }

  if (delim == NULL || *delim == 0) {
    delim = (char*)split_default_delimiters;
  }

  //Contar los tokens

  l_ptr = line;
  d_ptr = delim;
  n = 0;

  t_head = NULL;
  t_tail = NULL;

  while (*l_ptr != 0) {
    //Descartar los delimitadores al inicio de la cadena
    while (*l_ptr != 0 && *d_ptr != 0) {
      if (*l_ptr == *d_ptr) { //Es delimitador
        l_ptr++; //Ignorar delimitador
        d_ptr = delim; //Reiniciar delimitador
      }else {
        d_ptr++; //Probar con el siguiente delimitador
      }
    }
    //Validar fin de la cadena
    if (*l_ptr == 0){ 
      break;
    }
    //Buscar fin de token (proximo delimitador o NULL)
    aux = l_ptr;
    d_ptr = delim;
    while (*aux != 0) {
      if (*aux == *d_ptr) { //Es delimitador
        break;
      }else { 
        d_ptr++; //Probar con el siguiente delimitador
      }
      if (*d_ptr == 0) { //Fin de los delimitadores
        aux++; //Caracter no es delimitador
        d_ptr = delim; //Reiniciar delimitador
      }
    }
    if (aux > l_ptr) {
      //Crear nuevo token
      tok = (struct token *)malloc(sizeof(struct token));
      //Crear buffer con los caracteres
      str = (char*)malloc(aux - l_ptr + 1);
      //Copiar los caracteres al buffer
      strncpy(str, l_ptr, aux - l_ptr);
      //Terminar correctamente el buffer
      str[aux - l_ptr] = 0;

      tok->chars = str; //Almacenar el buffer en el token
      tok->next = NULL;

      //Adicionar el token a la lista
      if (t_head == NULL) { //Primero es cabeza de la lista
        t_head = tok;
        t_tail = t_head;
      }else { //Adicionar al final 
        t_tail->next = tok;
        t_tail = tok;
      }
      n++;
    }
    l_ptr = aux; //Avanzar en la cadena
  }

  /* Crear arreglo de cadenas, n + 1 posiciones  */
  ret  = (char **)malloc((n +1) * sizeof(char*));

  /* Recorrer la lista y guardar los datos de cada token en el arreglo*/
  tok = t_head;
  for (i = 0; i <n; i++) {
    ret[i] = tok->chars;
    t_head = tok;
    tok = tok->next;
    free(t_head); //Liberar el token (no sus caracteres)
  }

  //Agregar entrada centinela
  ret[i] = 0;

  *tokens = n;
  return ret;
}
