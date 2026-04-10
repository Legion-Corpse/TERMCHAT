this is a basic file transfer system built in C using TCP sockets. it is basically a simple FTP where you can upload and download files between two terminals.

this is the first task of a larger project i am building which will eventually be a group chat system. the goal right now was just to learn how socket programming works, specifically how a client connects to a server and how data moves between them.

the code is based entirely on beej's guide to network programming. all the socket functions used like getaddrinfo, socket, bind, listen, accept, connect, send, recv and close come directly from that book.

structure

file-transfer/
    server/ft_server.c - the server. it waits for a client to connect, reads the command (GET or PUT), and either sends or receives a file.
    client/ft_client.c - the client. you run this with the server ip, type get or put commands, and it handles the file transfer.
    Makefile - builds both the server and client.

why it is split into server and client folders

i separated them because in a real setup the server and client run on different machines. keeping them in separate folders makes it clear which code belongs to which side. this same idea will carry over into the group chat project where the server handles all the clients and each client just connects to it.

how to run it

go into the file-transfer folder and run make. then open two terminals. in one run ./server/ft_server and in the other run ./client/ft_client followed by the server ip. then type get or put with a filename.
