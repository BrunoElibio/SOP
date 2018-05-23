#include "Biblioteca.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void retira(void *_reg, void *_num){
    Loja *reg = (Loja*) _reg; 
    unsigned int *num = (unsigned int*) _num;
    if(reg->qtd >= *num) {reg->qtd -= *num; *num = 0;}
    else if(reg->qtd != 0){*num -= reg->qtd; reg->qtd -= reg->qtd;}
}

void modifica(void *_reg, void *_num){
    Loja *reg = (Loja*) _reg; 
    unsigned int *num = (unsigned int*) _num;
    reg->qtd += *num;
    reg->desejado += *num;
}

int compara(void *_reg, void *_lista){
    char *reg = (char *) _reg;
    Loja *lista = (Loja *) _lista;
    if(strcmp(reg, lista->nome) == 0)
        return 0;
    return 1; 
}

char* itoa(int val, int base){
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}

char *nArq(const char *src, int i){
    char *dest = malloc(sizeof(src));
    strcpy(dest, src);
    char *buffer = itoa(i, 10);
    strcat(dest, "-");
    strcat(dest, buffer);
    return dest;
}
