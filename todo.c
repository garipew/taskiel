#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>


int main(int argc, char* argv[]){
	
	initscr();

	Lista* lista;
	char* listname;

	lista = carregar_lista(nomear_lista(&listname,argc,argv));
	refresh();
	
	editar_lista(lista);
	fechar_lista(lista);
	endwin();

	return 0;

}
