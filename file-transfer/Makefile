CC     = gcc
CFLAGS = -Wall -Wextra

all: server/ft_server client/ft_client

server/ft_server: server/ft_server.c
	$(CC) $(CFLAGS) -o server/ft_server server/ft_server.c

client/ft_client: client/ft_client.c
	$(CC) $(CFLAGS) -o client/ft_client client/ft_client.c

clean:
	rm -f server/ft_server client/ft_client
