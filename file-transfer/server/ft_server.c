#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT    "9090"
#define BUFSIZE 4096

int sendall(int fd, const char *buf, int *len)
{
    int total = 0, bytesleft = *len, n;
    while (total < *len) {
        n = send(fd, buf + total, bytesleft, 0);
        if (n == -1) break;
        total += n;
        bytesleft -= n;
    }
    *len = total;
    return (n == -1) ? -1 : 0;
}

int recvall(int fd, char *buf, int len)
{
    int total = 0, n;
    while (total < len) {
        n = recv(fd, buf + total, len - total, 0);
        if (n <= 0) return n;
        total += n;
    }
    return total;
}

void handle_get(int client_fd, const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        uint32_t zero = htonl(0);
        send(client_fd, (char *)&zero, sizeof(zero), 0);
        return;
    }

    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint32_t filesize_net = htonl((uint32_t)filesize);
    send(client_fd, (char *)&filesize_net, sizeof(filesize_net), 0);

    char buf[BUFSIZE];
    int n;
    while ((n = fread(buf, 1, BUFSIZE, fp)) > 0) {
        int len = n;
        sendall(client_fd, buf, &len);
    }
    fclose(fp);
}

void handle_put(int client_fd, const char *filename)
{
    uint32_t filesize_net;
    recvall(client_fd, (char *)&filesize_net, sizeof(filesize_net));
    uint32_t filesize = ntohl(filesize_net);

    FILE *fp = fopen(filename, "wb");
    char buf[BUFSIZE];
    uint32_t remaining = filesize;
    int n;
    while (remaining > 0) {
        int to_recv = (remaining < BUFSIZE) ? (int)remaining : BUFSIZE;
        n = recv(client_fd, buf, to_recv, 0);
        if (n <= 0) break;
        fwrite(buf, 1, n, fp);
        remaining -= n;
    }
    fclose(fp);
    send(client_fd, "OK\n", 3, 0);
}

int main(void)
{
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;
    getaddrinfo(NULL, PORT, &hints, &servinfo);

    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    freeaddrinfo(servinfo);
    listen(sockfd, 5);

    while (1) {
        int client_fd = accept(sockfd, NULL, NULL);

        char command[512];
        int i = 0;
        char c;
        while (i < (int)sizeof(command) - 1) {
            int n = recv(client_fd, &c, 1, 0);
            if (n <= 0) break;
            command[i++] = c;
            if (c == '\n') break;
        }
        command[i] = '\0';

        char *nl = strchr(command, '\n');
        if (nl) *nl = '\0';

        char cmd[8], filename[256];
        sscanf(command, "%7s %255s", cmd, filename);

        if (strcmp(cmd, "GET") == 0)
            handle_get(client_fd, filename);
        else if (strcmp(cmd, "PUT") == 0)
            handle_put(client_fd, filename);

        close(client_fd);
    }

    return 0;
}
