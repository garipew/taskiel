#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* nomear_lista(char** listname, int argc, char* argv[]){

	*listname = ".list";
	if(argc >= 2){
		*listname = (argv)[1];
	}

	return *listname;
	
}


int main(int argc, char* argv[]){
	
	char* listname;
	printf("%s\n", nomear_lista(&listname,argc,argv));
	
	FILE* list = fopen(listname, "w+");
	fclose(list);

	return 0;

}
