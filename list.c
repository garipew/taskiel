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
	int tasks;

	fscanf(lista->file, "%s %d %d", titulo, &tasks, &tamanho); 
	lista->tamanho = tamanho;
	lista->tasks = tasks;
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
		fprintf(list, "%s %d %d\n", listname, 0, 0);
		rewind(list);
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
			remover_item(list);
			break;
		case 'a':
			adicionar_detalhe(list);
			break;
		case 'e':
			editar_tarefa(list);
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
	printw("[a] Adicionar detalhe.\n");
	printw("[e] Editar tarefa.\n");
	printw("[q] Retornar à edição.\n");
	refresh();
	do{
		noecho();
		close = getch();	
	}while(close != 'q');
	echo();
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

	list->tasks++;
	list->tamanho++;
	fprintf(list->file, "%s %d %d\n", list->nome, list->tasks, list->tamanho); 
	rewind(list->file);

}


void append_on_file(char str[], char file_name[]){

	FILE* appended = fopen("append.swp", "w");
	FILE* append = fopen(file_name, "r");

	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	fprintf(appended, "%s\n", str);

	while((read = getline(&line, &len, append)) != -1){
		fprintf(appended, "%s", line);
	}

	fclose(appended);
	fclose(append);
	remove(file_name);
	rename("append.swp", file_name);
}


void remover_item(Lista* list){
	
	int opcao = selecionar_item(list);
	if(opcao == -1){
		return;
	}
	FILE* out = fopen("out.swp", "w");
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int item = -1;
	int tarefa = 0;

	int in_task = 0;
	int in_detail = 0;
	char selected_type = 'a';

	char cabecalho[100];
	
	getline(&line, &len, list->file);	

	while((read = getline(&line, &len, list->file)) != -1){
		in_task = (line[0] == '\t' && line[1] == '[');
		
		item++; //nao tem linhas vazias

		if(item == opcao){
			list->tamanho--;
			list->tasks -= in_task;
			selected_type = in_task ? 't' : 'd';
			continue;
		}
	
		selected_type = (selected_type == 't' && in_task) ? 'a' : selected_type;

		if(selected_type == 't'){
			list->tamanho--;
			continue;	
		}

		fprintf(out, "%s", line);
	}

	fclose(list->file);
	fclose(out);
	snprintf(cabecalho, sizeof(cabecalho), "%s %d %d", list->nome, list->tasks, list->tamanho);
	append_on_file(cabecalho, "out.swp");
	remove(list->nome);
	rename("out.swp", list->nome);
	list->file = fopen(list->nome, "r+");

}


int selecionar_tarefa(Lista* list){
	
	char acao;
	int opcao = 0;
	char close_message;

	clear();
	if(list->tasks == 0){
		printw("Não há tarefas disponíveis, experimente criar uma.\n");
		refresh();
		do{ close_message = getch(); } while(close_message != 'q');
		return -1;
	}

	printw("Selecione a tarefa e pressione [ENTER].\n");
	escrever_tarefas(list, opcao, 0);
	noecho();
	do{	
		acao = getch();
		clear();
		printw("Selecione a tarefa e pressione [ENTER].\n");
		if(acao == 'q'){
			break;
		}
		opcao = mover_cursor(acao, opcao, list->tasks);
		escrever_tarefas(list, opcao, 0);
	} while(acao != '\n');
	echo();
	refresh();
	
	return (acao == '\n' ? opcao : -1);
}


int selecionar_item(Lista* list){

	char acao;
	int opcao = 0;
	char close_message;

	clear();
	if(list->tasks == 0){
		printw("Não há tarefas disponíveis, experimente criar uma.\n");
		refresh();
		do{ close_message = getch(); } while(close_message != 'q');
		return -1;
	}

	printw("Selecione o item e pressione [ENTER].\n");
	escrever_tarefas(list, opcao, 1);
	noecho();
	do{	
		acao = getch();
		clear();
		printw("Selecione o item e pressione [ENTER].\n");
		if(acao == 'q'){
			break;
		}
		opcao = mover_cursor(acao, opcao, list->tamanho);
		escrever_tarefas(list, opcao, 1);
	} while(acao != '\n');
	echo();
	refresh();
	
	return (acao == '\n' ? opcao : -1);
}


void escrever_tarefa(Lista* list, int number){

	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int current_task = -1;
	int details = 0;

	read = getline(&line, &len, list->file); // lê o titulo
	while((read = getline(&line, &len, list->file)) != -1){
		if(line[0] == '\t' && line[1] == '['){
			current_task++;
			if(current_task > number){
				break;
			}
		}
		if(current_task == number){
			if(details == 0){
				printw("%s", line);
				details++;
				continue;
			}
			line[0] = details + '0';
			line[1] = ' ';
			printw("\t\t%s", line);
			details++;
		}
	}
	rewind(list->file);
}


void escrever_tarefas(Lista* list, int selected, int include_details){

	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int current_item = -1;
	int written = 0;
	int x,y;

	read = getline(&line, &len, list->file); // lê o titulo
	while((read = getline(&line, &len, list->file)) != -1){
		if(line[0] == '\t' && ((line[1] == '[') || (line[1] == '\t' && include_details))){
			current_item++;
		}
		printw("%s", line);	
		if((current_item == selected) && !written){
			getyx(stdscr, y, x);
			mvaddch(y-1, 65, '<');
			addch('\n');
			written = 1;
		}
	}
	rewind(list->file);
}


void adicionar_detalhe(Lista* list){
	
	int opcao = selecionar_tarefa(list);
	if(opcao == -1){
		return;
	}
	
	FILE* out = fopen("out.swp", "w");

	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int current_task = -1;
	
	char detail[200] = "* ";
	char detail_str[198];

	int written = 0;
	
	clear();
	printw("Digite o detalhe para a tarefa:\n");
	refresh();
	getstr(detail_str);
	strcat(detail, detail_str);
	read = getline(&line, &len, list->file); //titulo
	list->tamanho++;
	fprintf(out, "%s %d %d\n", list->nome, list->tasks, list->tamanho);
	while((read = getline(&line, &len, list->file)) != -1){
		if(line[0] == '\t' && line[1] == '['){
			current_task++;
		}
		fprintf(out, "%s", line);
		if(current_task == opcao && !written){
			fprintf(out, "\t\t%s\n", detail);
			written = 1;
		}
	}
	
	fclose(list->file);
	fclose(out);
	remove(list->nome);
	rename("out.swp", list->nome);
	list->file = fopen(list->nome, "r+");

}


void editar_titulo(Lista* list, int opcao){

	FILE* out = fopen("out.swp", "w");

	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int current_task = -1;

	char title[70] = "[=>] ";
	char title_str[64];
	int written = 0;

	printw("Digite o novo titulo para tarefa:\n");
	refresh();
	getstr(title_str);
	strcat(title, title_str);
	while((read = getline(&line, &len, list->file)) != -1){
		if(line[0] == '\t' && line[1] == '['){
			current_task++;
		}
		if(current_task == opcao && !written){
			fprintf(out, "\t%s\n", title);
			written = 1;
			continue;
		}
		fprintf(out, "%s", line);
	}
	
	fclose(list->file);
	fclose(out);
	remove(list->nome);
	rename("out.swp", list->nome);
	list->file = fopen(list->nome, "r+");
}


void editar_detalhe(Lista* list, int opcao){

	FILE* out = fopen("out.swp", "w");

	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int current_task = -1;
	int current_desc = -2;

	char detail[200] = "* ";
	char detail_str[198];
	char desc[4];

	int descricao = 0;
	int written = 0;

	escrever_tarefa(list, opcao);
	printw("Qual o numero da descricao que deseja alterar? ");
	getstr(desc);
	for(int i=0;i<3;i++){
		if(desc[i] != '\0'){
			descricao*=10;
			descricao += desc[i] - '0';
		}
	}
	descricao--;
	printw("Digite a nova descricao para a tarefa:\n");
	refresh();
	getstr(detail_str);
	strcat(detail, detail_str);
	while((read = getline(&line, &len, list->file)) != -1){
		if(line[0] == '\t' && line[1] == '['){
			current_task++;
			current_desc = -2;
		}

		current_desc++;

		if(current_desc == descricao && current_task == opcao && !written){
			fprintf(out, "\t\t%s\n", detail);
			written = 1;
			continue;
		}
		fprintf(out, "%s", line);
		
	}
	
	fclose(list->file);
	fclose(out);
	remove(list->nome);
	rename("out.swp", list->nome);
	list->file = fopen(list->nome, "r+");
}


void editar_tarefa(Lista* list){

	int opcao = selecionar_tarefa(list);
	if(opcao == -1){
		return;
	}
	
	char acao;
	clear();
	printw("Selecione o que deseja editar:\n");
	printw("[t] Editar titulo\n[d] Editar descricao\n");
	refresh();
	noecho();
	
	do{
		acao = getch();
		if(acao == 't'){
			echo();
			editar_titulo(list, opcao); 
			break;
		}
		if(acao == 'd'){
			echo();
			editar_detalhe(list, opcao);
			break;
		}
	} while(acao != 'q');
	echo();

}


