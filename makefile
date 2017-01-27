CFLAGS=-c -Wall -O2
LIBS = -lm -lpthread

all: fb

fb: main.o
	$(CC) main.o $(LIBS) -o fb

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *o fb

