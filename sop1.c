#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <sched.h>
#include "utils.h"
#include "Ldse.h"
#include "Biblioteca.h"

/*
Autores: Bruno Corrêa Elibio
         Raphael Pennacchi
*/

pthread_mutex_t mutex;
pthread_barrier_t mybarrier;
LDSE *listaOfertas;
int roda = 1;



//Funcao que "pega" os itens na listaOfertas
int pega(Loja *args){
    if(!buscaEModifica(listaOfertas,(void*) &(args->qtd), args->nome, compara, retira)) return 1;
    return 0;
}

void *my_func(void *args){
    //Converte o ponteiro de void para ponteiro da minha estrutura de argumentos, abre o arquivo
    //referente a thread e cria uma lista simplesmente encadeada da estrutura loja
    My_args *args_ = (My_args *) args;
    Loja *head = NULL;
    Loja *foo = NULL;
	Loja *holder = malloc(sizeof(Loja));
    while(fscanf(args_->arquivo,"%s %d", holder->nome, &(holder->qtd)) != EOF ){
		if(foo == NULL) {
			foo = malloc(sizeof(Loja));
			head = foo;
		}
		else {
			foo->prox = malloc(sizeof(Loja));
			foo = foo->prox;
			foo->prox = NULL;
		}
        foo->desejado = holder->qtd;
        foo->qtd = holder->qtd;
        strcpy(foo->nome , holder->nome);

    }

    //Barreira para sincronizar todas as threads apos a criacao de suas estruturas
    pthread_barrier_wait(&mybarrier);

    //Roda e a flag que diz quando ainda ha ofertas a serem carregadas na listaOfertas,
    //utilizamos mutex para trancar o ponto critico e apos a funcao pega conseguir pegar
    //algo da lista ela retorna 1 assim desbloqueando o mutex e dando yield.
    while(roda){
        foo = head;
        while(foo != NULL){
            pthread_mutex_lock(&mutex);
            if(pega(foo)){
                pthread_mutex_unlock(&mutex);
                sched_yield();
            }
            else pthread_mutex_unlock(&mutex);
            foo = foo->prox;
        }
    }
	free(holder);

    //Retorno das estruturas alocadas
    pthread_exit((void*) head);
}

//Funcao que adiciona as ofertas na listaOfertas
int disponibilizaOferta(Loja *args){
    if(!buscaEModifica(listaOfertas,(void*) &(args->qtd), args->nome, compara, modifica)) return 2;
    else{
        insereNoFim(listaOfertas, (void*) args);
        return 1;
    }
    return 0;
}

/*
//Funcao para imprimir a listaOfertas, nao utilizada na versao final do codigo
void imprime(){
    NoLDSE *noAux;
    if(listaOfertas->lista != NULL){
        noAux = listaOfertas->lista;
        while(noAux != NULL){
            printf("%s %u %u\n", ((Loja*)(noAux->dados))->nome ,((Loja*)(noAux->dados))->qtd,((Loja*)(noAux->dados))->desejado);
            noAux = noAux->prox;
        }
    }
}
*/

int main(int argc, char const *argv[]) {
    //Inicializacao de variaveis
    FILE *arquivo;
    int i = 1;
    int nthreads = atoi(argv[1]);

    //Tentantiva de abrir o arquivo passado no argumento
    if((arquivo = fopen(argv[2], "r")) == NULL){
        printf("Nome de arquivo incorreto, tente novamente.\n");
        exit(0);
    }

    //Alocacao do vetor de ponteiro de arquivos das threads
    FILE **compras;
    if((compras = malloc(sizeof(FILE *) * nthreads)) == NULL){
        printf("Falha ao alocar compras\n");
        exit(0);
    }

    //Abrindo os arquivos das threads, faz a verificacao se foi aberto corretamente
    //e para na primeira falha setando o numero de arquivo abertos anteriormente a
    //falha como o numero de threads
    for(i = 0; i <= nthreads; i++){
        char *nomeArquivo = nArq(argv[2], i+1);
        if((compras[i] = fopen(nomeArquivo, "r")) == NULL){
            printf("Numero de threads incorreto, novo numero de threads = %d\n", i);
            break;
        }
        free(nomeArquivo);
    }

    //Novo numero de threads a partir da quantidade de arquivos abertos com sucesso
    nthreads = i;

    //Realocamos o tamanho do vetor a partir do novo numero de threads
    if((compras = (FILE**) realloc(compras, sizeof(FILE*) * nthreads)) == NULL){
        printf("Realloc nao funcionou que pena!\n");
        return 0;
    }

    //iniciamos a barreira e o mutex
    int rc;
    rc = pthread_barrier_init(&mybarrier, NULL, nthreads+1);
    assert(rc == 0);
    rc = pthread_mutex_init(&mutex, NULL);
    assert(rc == 0);

    //Cria as threads e os argumentos
    pthread_t *threads;
    My_args *args;
    if((threads = malloc(sizeof(pthread_t) * nthreads)) == NULL) exit(0);
    if((args = malloc(sizeof(My_args) * nthreads)) == NULL) exit(0);


    //Defino o ponteiro do arquivo na minha estrutura de argumentos igual
    //o ponteiro do arquivo aberto
    for (i = 0; i < nthreads; i++) {
        args[i].arquivo = compras[i];
        if(pthread_create(&threads[i], NULL, my_func , (void *) &args[i])) exit(1);
    }

    //Cria a lista de ofertas
    assert(cria(&listaOfertas, sizeof(Loja))==0);

    //Ativa a barreira para sincronizar as threads neste ponto antes de comecarmos
    //a lancar as ofertas na lista
    pthread_barrier_wait(&mybarrier);

    //Cria uma variavel local como holder
    Loja atual;
    while((fscanf(arquivo,"%s %u", atual.nome, &atual.qtd))!=EOF){
		atual.desejado = atual.qtd;
        if(strcmp(atual.nome,"#") == 0){
                msleep(atual.qtd);
            }
        else{
            pthread_mutex_lock(&mutex);
            disponibilizaOferta(&atual);
            pthread_mutex_unlock(&mutex);
            }
        }


    //printf("%s %u", ((Loja*)(listaOfertas->lista->dados))->nome ,((Loja*)(listaOfertas->lista->dados))->qtd);
    roda = 0;

    //Join e retorna os dados das threads
    Loja **retorno = (Loja**) malloc(sizeof(Loja*)*nthreads);
    for (i = 0; i < nthreads; i++) pthread_join(threads[i],(void**) (&retorno[i]));

    //Cria o arquivo do portfolio e o saldo de itens
    FILE *saida;
    char *nomeArquivo = nArq("saida" , nthreads);
    saida = fopen(nomeArquivo, "w");
    free(nomeArquivo);

    //Portfolio
    for(i = 1; i <= nthreads; i++){
        fprintf(saida, "Thread %d - Portfolio de itens:\n", i);
		fprintf(saida, "Item               Quantidade  Demanda\n");
		Loja *foo = retorno[i-1];
		while(foo != NULL){
			fprintf(saida, fmtport, foo->nome, foo->desejado - foo->qtd, foo->desejado);
			foo = foo->prox;
		}
		fprintf(saida, "\n");
    }

    //Saldo
	fprintf(saida, "Saldo de itens:\n");
	fprintf(saida, "Item               Quantidade  Ofertado\n");
	NoLDSE *_foo = listaOfertas->lista;
	while(_foo!=NULL){
		Loja *foo = (Loja*) _foo->dados;
		fprintf(saida, fmtsaldo, foo->nome, foo->qtd, foo->desejado);
		_foo = _foo->prox;
	}

    //printf("Arquivo - %s : criado com sucesso!\n", );

    //Destroi a barrier e o mutex
    pthread_barrier_destroy(&mybarrier);
    pthread_mutex_destroy(&mutex);


    //Fecha os arquivos
    fclose(arquivo);
    for(i = 0; i < nthreads; i++) fclose(compras[i]);
    fclose(saida);

    //Libera a memoria do que foi alocado
    for (i = 0; i < nthreads; i++){
        Loja *foo = retorno[i];
        Loja *hold;
        while(foo != NULL){
            hold = foo;
            foo = foo->prox;
            free(hold);
        }
    }
    reinicia(listaOfertas);
    destroi(&listaOfertas);
    free(compras);
    free(threads);
    free(args);

    return 0;
}
