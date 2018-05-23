#ifndef Biblioteca_h
#define Biblioteca_h
#include <stdio.h>

typedef struct loja{
  char nome[255];
  unsigned int qtd;
  unsigned int desejado;
  struct loja *prox;
}Loja;
 
typedef struct my_args{
  FILE *arquivo;
}My_args;


void retira(void *_reg, void *_num);
void modifica(void *_reg, void *_num);
int compara(void *_reg, void *_lista);
char* itoa(int val, int base);
char *nArq(const char *src, int i);


#endif