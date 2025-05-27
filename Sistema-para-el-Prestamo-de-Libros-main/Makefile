CC = gcc
CFLAGS = -Wall -pthread
OBJS = bdlibros.o

all: solicitante receptor

solicitante: Solicitante.c $(OBJS)
	$(CC) $(CFLAGS) -o solicitante Solicitante.c $(OBJS)

receptor: Receptor.c $(OBJS)
	$(CC) $(CFLAGS) -o receptor Receptor.c $(OBJS)

bdlibros.o: SistemaDePrestamoDeLibros.c SistemaDePrestamoDeLibros.h
	$(CC) $(CFLAGS) -c SistemaDePrestamoDeLibros.c -o bdlibros.o

clean:
	rm -f *.o solicitante receptor