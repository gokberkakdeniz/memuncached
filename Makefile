build:
	gcc -lcrypt -lpthread vector.c connection.c fnv.c hash_table.c server.c  -std=gnu11 -ggdb3  -o memuncached.o
build-main:
	gcc -lcrypt vector.c connection.c fnv.c hash_table.c main.c  -std=c11 -ggdb3  -o memuncached.o
