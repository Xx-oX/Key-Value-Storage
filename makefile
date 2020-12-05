dir_name = storage
optimal = -O3
gdb = 
warning = -w

all: main
main: main.o api.o rbt.o
	gcc $(gdb) $(optimal) $(warning) -o main main.o api.o rbt.o
main.o: main.c types.h api.h
	gcc $(gdb) $(optimal) $(warning) -c main.c 
api.o: api.c api.h types.h rbt.h
	gcc $(gdb) $(optimal) $(warning) -c api.c
rbt.o: rbt.c rbt.h types.h
	gcc $(gdb) $(optimal) $(warning) -c rbt.c

clean:
	rm -f main *.o *.output -r ${dir_name}/

clroutput:
	rm -f *.output -r ${dir_name}/