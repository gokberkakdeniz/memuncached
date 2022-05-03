build:
	gcc -lcrypt command.c fnv.c cache_table.c main.c  -std=c11 -ggdb3  -o memuncached.o
