#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ldse.h"

#define FRACASSO 1
#define SUCESSO 0


//typedef int(*_compara)(void*,void*);

/*************** CRIA ***************/
int cria(LDSE **pp, int tamInfo){
  int ret = FRACASSO;
    LDSE *desc = (LDSE*) malloc(sizeof(LDSE));

    if( !desc ) {
        ret = FRACASSO;
    }

    else {
        desc->lista = NULL;
        desc->tamInfo = tamInfo;
        ret = SUCESSO;
    }

    (*pp) = desc;

    return ret;
}
/*************** INSERE NO FINAL ***************/
int insereNoFim(LDSE *p, void *novo)
{
    NoLDSE *temp, *aux;
    int ret = FRACASSO;

    if( (temp = (NoLDSE*) malloc(sizeof(NoLDSE))) != NULL ) {
        if( (temp->dados = (void *) malloc(p->tamInfo)) != NULL ) {
            memcpy(temp->dados, novo, p->tamInfo);
            temp->prox = NULL;
            if( p->lista == NULL ) {
                p->lista=temp;
            }
            else {
                aux = p->lista;
                while( aux->prox != NULL ) {
                    aux = aux->prox;
                }
                aux->prox=temp;
            }
            ret = SUCESSO;
        }
        else {
            free(temp);
        }
    }

    return ret;
}
/*************** REMOVER DO FINAL ***************/
void removeUltimo(LDSE *pLDSE, void *reg){
	NoLDSE *aux1=NULL;
	NoLDSE *aux2=NULL;
	if(pLDSE != NULL){
		aux1 = pLDSE->lista;
			if(aux1->prox != NULL)
				aux2 = aux1->prox;
			if(aux2 == NULL){
				memcpy(reg,aux1->dados,pLDSE->tamInfo);
				free(aux1->dados);
				free(aux1);
				pLDSE->lista = NULL;
				//SUCESSO
			}else{
				while(aux2->prox != NULL){
					aux1 = aux2;
					aux2 = aux2->prox;
				}
				aux1->prox = NULL;
				memcpy(reg, aux2->dados, pLDSE->tamInfo);
				free(aux2->dados);
				free(aux2);
				//SUCESSO
			}
	}
}
/*************** BUSCA NA POSI��O L�GICA ***************/
int buscaNaPosLog(LDSE *p, void *reg, char *nome, int (*_compara)(void*, void*))
{
    int ret = FRACASSO;
    NoLDSE *pos;

    if(p->lista != NULL) {
        pos = p->lista;
        while(_compara(nome, pos->dados) && pos->prox != NULL) {
            pos = pos->prox;
        }
        if(!_compara(nome, pos->dados)){
            memcpy(reg, pos->dados, p->tamInfo);
            ret = SUCESSO;
        }
    }

    return ret;
}

int buscaEModifica(LDSE *p, void *reg, char *nome, int (*_compara)(void*, void*), void (*_modifica)(void*, void*))
{
    int ret = FRACASSO;
    NoLDSE *pos;

    if(p->lista != NULL) {
        pos = p->lista;
        while(_compara(nome, pos->dados) && pos->prox != NULL) {
            pos = pos->prox;
        }
        if(!_compara(nome, pos->dados)){
            _modifica(pos->dados, reg);
            ret = SUCESSO;
        }
    }

    return ret;
}

/*************** TAMANHO ***************/
int tamanho(LDSE *p)
{
    int tam = 0;
    NoLDSE *aux;
    aux = p->lista;

    if(aux == NULL) {
        tam = 0;
    }

    else {
        while(aux != NULL) {
            aux = aux->prox;
            tam++;
        }
    }

    return tam;
}

/*************** PURGA ***************/
void reinicia(LDSE *p)
{
    NoLDSE *aux = NULL;

    if(p->lista != NULL) {
        aux = p->lista->prox;
        while(aux != NULL) {
            free(p->lista->dados);
            free(p->lista);
            p->lista = aux;
            aux = aux->prox;
        }
        free(p->lista->dados);
        free(p->lista);
        p->lista = NULL;
    }
}

/*************** DESTROI ***************/
void destroi(LDSE **pp)
{
    reinicia(*pp);
    free(*pp);
    (*pp) = NULL;
}
