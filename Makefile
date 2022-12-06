CC =gcc
CFLAGS = -o
RM =rm
CFLAG = -c

all: umalloc.o
	$(CC) $(CFLAGS) mem memgrind.c umalloc.o

umalloc.o: umalloc.c
	$(CC) $(CFLAGS) umalloc.o $(CFLAG) umalloc.c

clean:
	@echo "Clean Success"
	$(RM) *.o ./mem