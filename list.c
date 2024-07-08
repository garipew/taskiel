#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>


Lista* carregar_lista(char* listname){

	Lista* lista = malloc(sizeof(*lista));
	strcpy(lista->nome, listname);
	lista->file = abrir_lista(listname);	
	
	char titulo[64];
	int tamanho;

	fscanf(lista->file, "%s %d", &titulo, &tamanho); 
	lista->tamanho = tamanho;
	rewind(lista->file);
	return lista;
}


char* nomear_lista(char** listname, int argc, char* argv[]){

	*listname = ".list";
	if(argc >= 2){
		*listname = (argv)[1];
	}

	return *listname;
	
}


void escrever_lista(Lista* list){

	char next_char;
	
	do{
		next_char = fgetc(list->file);
		addch(next_char);
	} while(next_char != EOF);

	rewind(list->file);
	refresh();
}


FILE* abrir_lista(char* listname){

	FILE* list = fopen(listname, "r+");
	if(list == NULL){
		printw("Creating %s...\n", listname);
		list = fopen(listname, "w+");
		fprintf(list, "%s %d\n", listname, 0);
	}

	return list;
}


void editar_lista(Lista* list){
	
	char action;
	int status = 0;

	do{
		escrever_lista(list);
		printw("Digite aqui sua próxima ação: ");
		refresh();
		action = getch();
		addch('\n');
		refresh();
		status = realizar_acao(list, action);
		clear();
	}while(status == 0);
}


int realizar_acao(Lista* list, char action){

	switch(action){
		case 'Q':
			return 1;
		case 'q':
			return 0;
		case 'c':
			criar_tarefa(list);
			break;
		case 'r':
			remover_tarefa(list);
			break;
		default:
			listar_acoes();
	}

	return 0;
}


void listar_acoes(){
	char close;
	printw("[Q] Fechar a lista.\n");
	printw("[c] Criar nova tarefa.\n");
	printw("[r] Remover tarefa.\n");
	printw("[q] Retornar à edição.\n");
	refresh();
	do{
		noecho();
		close = getch();	
	}while(close != 'q');
	echo();
}


void criar_tarefa(Lista* list){

	char nova_tarefa[70] = "[=>] ";
	char titulo[64];
	
	clear();
	printw("Digite o título da nova tarefa:\n");
	refresh();
	getstr(titulo);
	if(strcmp(titulo, "q") == 0){
		clear();
		return;
	}
	strcat(nova_tarefa, titulo);
	fseek(list->file, 0, SEEK_END);
	fprintf(list->file, "\t%s\n", nova_tarefa);
	rewind(list->file);
	clear();

	list->tamanho++;
	fprintf(list->file, "%s %d\n", list->nome, list->tamanho); 
	rewind(list->file);

}


void remover(Lista* list, int opcao){
	
	FILE* out = fopen("out.swp", "w");
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	
	int tarefa = -1;
	getline(&line, &len, list->file);	
	fprintf(out, "%s %d\n", list->nome, --list->tamanho);

	while((read = getline(&line, &len, list->file)) != -1){
		tarefa++;
		if(tarefa == opcao){
			continue;
		}
		fprintf(out, line);
	}

	fclose(list->file);
	fclose(out);
	remove(list->nome);
	rename("out.swp", list->nome);
	list->file = fopen(list->nome, "r+");

}


void remover_tarefa(Lista* list){
	
	char acao;
	int opcao = 0;

	clear();
	printw("Selecione a tarefa que deseja remover e pressione [ENTER].\n");
	escrever_tarefas(list, opcao);
	noecho();
	do{	
		acao = getch();
		clear();
		printw("Selecione a tarefa que deseja remover e pressione [ENTER].\n");
		if(acao == 'q'){
			break;
		}
		opcao = mover_cursor(acao, opcao, list->tamanho);
		opcao =	escrever_tarefas(list, opcao);
	} while(acao != '\n');
	echo();
	refresh();

	if(acao == '\n'){
		remover(list, opcao);
	}
}


int escrever_tarefas(Lista* list, int selected){

	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int current_line = 0;
	int x,y;

	read = getline(&line, &len, list->file); // lê o titulo
	while((read = getline(&line, &len, list->file)) != -1){
		printw(line);	
		if(current_line == selected){
			getyx(stdscr, y, x);
			mvaddch(y-1, 73, '<');
			addch('\n');
		}
		current_line++;
	}
	if(selected > current_line){
		selected = current_line;
	}
	rewind(list->file);
	return selected;
}


int mover_cursor(char acao, int opcao, int tamanho){
	if(acao == 'k'){
		return (opcao == 0 ? 0 : opcao - 1);
	}
	if(acao == 'j'){
		return (opcao == tamanho - 1 ? opcao : opcao + 1);
	}

	return opcao;

}


void fechar_lista(Lista* list){

	fclose(list->file);
	free(list);

}
