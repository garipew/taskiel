
todo: todo.o
	gcc todo.o -o todo

todo.o: todo.c
	gcc -c todo.c

clean:
	rm -rf *.o todo
