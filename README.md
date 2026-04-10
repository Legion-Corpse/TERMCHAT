This is a basic file transfer system built in C using TCP sockets. It is basically a simple FTP where you can upload and download files between two terminals.

**STRUCTURE**

file-transfer/server/ft_server.c - the server. It waits for a client to connect, reads the command (GET or PUT), and either sends or receives a file.

file-transfer/client/ft_client.c - the client. You run this with the server IP, type get or put commands, and it handles the file transfer.

I separated them because in a real setup, the server and client run on different machines. Keeping them in separate folders makes it clear which code belongs to which side. Also, it allows viewing the files downloading and uploading properly. 

**how to run it**

Go into the file-transfer folder and run make. Then open two terminals. in one run ./server/ft_server and in the other run ./client/ft_client followed by the server ip. Then type get or put with a filename.
