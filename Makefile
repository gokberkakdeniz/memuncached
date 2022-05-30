CC=gcc
CFLAGS=-std=gnu11 -ggdb3 -lcrypt -lpthread

all: server lib client

# ============================================ SERVER ============================================

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c -o vector.o

utils.o: utils.h
	$(CC) $(CFLAGS) -c utils.h -o utils.o

hash_table.o: hash_table.c hash_table.h
	$(CC) $(CFLAGS) -c hash_table.c -o hash_table.o

fnv.o: fnv.c fnv.h
	$(CC) $(CFLAGS) -c fnv.c -o fnv.o

connection.o: connection.c connection.h
	$(CC) $(CFLAGS) -c connection.c -o connection.o

memuncached.o: vector.o connection.o fnv.o hash_table.o server.c
	gcc $(CFLAGS) vector.o connection.o fnv.o hash_table.o server.c -o memuncached.o

main.o: vector.o connection.o fnv.o hash_table.o main.c
	gcc $(CFLAGS) vector.o connection.o fnv.o hash_table.o main.c -o main.o

server: memuncached.o ;

# ============================================ CLIENT ============================================

libmemuncached.o: libmemuncached.c libmemuncached.h
	$(CC) $(CFLAGS) -c libmemuncached.c -o libmemuncached.o

client.o: libmemuncached.o logger.h client.c
	$(CC) $(CFLAGS) libmemuncached.c client.c -o client.o


lib: libmemuncached.o ;

client: client.o ;


# ========================================= MISCELLANEOUS ========================================


clean:
	rm *.o