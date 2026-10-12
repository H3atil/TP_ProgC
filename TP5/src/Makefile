CC ?= gcc

CFLAGS ?= -Wall -Wextra -g

COBJS ?= client.o
SOBJS ?= serveur.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client

.PHONY: all clean

all: $(SERVER) $(CLIENT)

$(SERVER): $(SOBJS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SOBJS)

$(CLIENT): $(COBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(COBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER) $(CLIENT) *.o
