this is a basic file transfer system built in C using TCP sockets. it is basically a simple FTP where you can upload and download files between two terminals.

**STRUCTURE**

file-transfer/server/ft_server.c - the server. it waits for a client to connect, reads the command (GET or PUT), and either sends or receives a file.

file-transfer/client/ft_client.c - the client. you run this with the server ip, type get or put commands, and it handles the file transfer.

i separated them because in a real setup the server and client run on different machines. keeping them in separate folders makes it clear which code belongs to which side. this same idea will carry over into the group chat project where the server handles all the clients and each client just connects to it.

**how to run it**

go into the file-transfer folder and run make. then open two terminals. in one run ./server/ft_server and in the other run ./client/ft_client followed by the server ip. then type get or put with a filename.
