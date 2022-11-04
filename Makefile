.phony all:
all: p2

p2: main.c queue.c
	gcc -w -pthread main.c queue.c -o p2

.PHONY clean:
clean:
	-rm -rf *.o *.exe
