
todo: todo.o list.o
	gcc todo.o list.o -o todo -lncurses

todo.o: todo.c 
	gcc -c todo.c -lncurses

list.o: list.c list.h
	gcc -c list.c -lncurses

clean:
	rm -rf *.o todo
