#ifndef Ldse_h
#define Ldse_h

typedef struct noLDSE {
    void *dados;
    struct noLDSE *prox;
} NoLDSE;

typedef struct LDSE {
    int tamInfo;
    NoLDSE *lista;
} LDSE;

//typedef int(*_compara)(void*, void*);

int cria(LDSE **pp, int tamInfo);
int insereNoFim(LDSE *p, void *novo);
int buscaNaPosLog(LDSE *p, void *reg, char *nome, int (*_compara) (void*, void*));
int buscaEModifica(LDSE *p, void *reg, char *nome, int (*_compara) (void*, void*), void (*_modifica)(void*, void*));
int tamanho(LDSE *p);
void reinicia(LDSE *p);
void destroi(LDSE **pp);
#endif