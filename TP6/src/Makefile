CC ?= gcc
CFLAGS ?= -Wall -Wextra -g
LDFLAGS ?=
LDLIBS ?= -lm

COBJS ?= client.o bmp.o couleur.o
SOBJS ?= serveur.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client

.PHONY: all clean

all: $(SERVER) $(CLIENT)

$(SERVER): $(SOBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(SERVER) $(SOBJS) $(LDLIBS)

$(CLIENT): $(COBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(CLIENT) $(COBJS) $(LDLIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER) $(CLIENT) *.o
