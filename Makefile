CC := gcc
EXE := sort

all: heap.o
	$(CC) -o $(EXE) main.c heap.o
	$(CC) -o generator int32_generator.c

debug: heap.o
	$(CC) -g -o $(EXE) main.c heap.o

heap.o: heap.c
	$(CC) -c heap.c

sort:
	$(CC) -o $(EXE) main.c

gen:
	$(CC) -o generator int32_generator.c

clean:
	rm -f $(EXE) generator *.o output.txt
