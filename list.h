#include <stdlib.h>
#include <stdio.h>

typedef struct lista{
	
	FILE* file;
	int tamanho;
	char nome[64];

} Lista;


Lista* carregar_lista(char*);
char* nomear_lista(char**,int,char**);
void escrever_lista(Lista*);
FILE* abrir_lista(char*);
void editar_lista(Lista*);
int realizar_acao(Lista*, char);
void listar_acoes();
void criar_tarefa(Lista*);
void remover_tarefa(Lista*);
int selecionar_tarefa(Lista*);
void escrever_tarefas(Lista*, int);
int mover_cursor(char, int, int);
void fechar_lista(Lista*);
void adicionar_detalhe(Lista*);
void editar_tarefa(Lista*);
