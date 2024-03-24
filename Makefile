NPROCS=4
LIBS=-lm
CFLAGS=-O2 -Wall -Wno-maybe-uninitialized
# As máquinas do laboratório precisam de --oversubscribe após o mpirun para funcionar

run: main
	mpirun -n $(NPROCS) $?

main: main.o
	mpicc -o $@ $? $(LIBS)

main.o: main.c
	mpicc $(CFLAGS) -c -o $@ $?

clean:
	rm -f main main.o
