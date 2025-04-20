CC=gcc
CFLAGS=-Wall -Wextra -O2
SERVER=servidor
CLIENT=logica_clientes

all: $(SERVER) $(CLIENT)

$(SERVER): servidor.o
	$(CC) $(CFLAGS) -o $(SERVER) servidor.o

$(CLIENT): logica_clientes.o
	$(CC) $(CFLAGS) -o $(CLIENT) logica_clientes.o

servidor.o: servidor.c servidor.h
	$(CC) $(CFLAGS) -c servidor.c

logica_clientes.o: logica_clientes.c logica_clientes.h
	$(CC) $(CFLAGS) -c logica_clientes.c

clean:
	rm -f $(SERVER) $(CLIENT) servidor.o logica_clientes.o
