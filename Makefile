build:
	gcc -lcrypt -pthread connection.c fnv.c cache_table.c server.c  -std=c11 -ggdb3  -o memuncached.o
build-main:
	gcc -lcrypt connection.c fnv.c cache_table.c main.c  -std=c11 -ggdb3  -o memuncached.o
