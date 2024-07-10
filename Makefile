
taskiel: taskiel.o list.o
	gcc taskiel.o list.o -o taskiel -lncurses

taskiel.o: taskiel.c 
	gcc -c taskiel.c -lncurses

list.o: list.c list.h
	gcc -c list.c -lncurses

clean:
	rm -rf *.o taskiel
