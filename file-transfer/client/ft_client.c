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

int connect_to_server(const char *host)
{
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host, PORT, &hints, &servinfo);

    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    freeaddrinfo(servinfo);
    return sockfd;
}

void do_get(const char *host, const char *filename)
{
    int sockfd = connect_to_server(host);

    char command[512];
    snprintf(command, sizeof(command), "GET %s\n", filename);
    int len = (int)strlen(command);
    sendall(sockfd, command, &len);

    uint32_t filesize_net;
    recvall(sockfd, (char *)&filesize_net, sizeof(filesize_net));
    uint32_t filesize = ntohl(filesize_net);

    if (filesize == 0) {
        printf("file not found\n");
        close(sockfd);
        return;
    }

    printf("downloading %s\n", filename);

    FILE *fp = fopen(filename, "wb");
    char buf[BUFSIZE];
    uint32_t remaining = filesize;
    int n;
    while (remaining > 0) {
        int to_recv = (remaining < BUFSIZE) ? (int)remaining : BUFSIZE;
        n = recv(sockfd, buf, to_recv, 0);
        if (n <= 0) break;
        fwrite(buf, 1, n, fp);
        remaining -= n;
    }
    fclose(fp);
    close(sockfd);
    printf("done\n");
}

void do_put(const char *host, const char *local_filename, const char *remote_filename)
{
    FILE *fp = fopen(local_filename, "rb");
    if (fp == NULL) { printf("file not found\n"); return; }

    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int sockfd = connect_to_server(host);

    char command[512];
    snprintf(command, sizeof(command), "PUT %s\n", remote_filename);
    int len = (int)strlen(command);
    sendall(sockfd, command, &len);

    uint32_t filesize_net = htonl((uint32_t)filesize);
    send(sockfd, (char *)&filesize_net, sizeof(filesize_net), 0);

    printf("uploading %s\n", local_filename);

    char buf[BUFSIZE];
    int n;
    while ((n = (int)fread(buf, 1, BUFSIZE, fp)) > 0) {
        int slen = n;
        sendall(sockfd, buf, &slen);
    }
    fclose(fp);

    char resp[16];
    recv(sockfd, resp, sizeof(resp) - 1, 0);
    close(sockfd);
    printf("done\n");
}

int main(int argc, char *argv[])
{
    (void)argc;
    const char *host = argv[1];
    char input[512];

    while (1) {
        printf("> ");
        fflush(stdout);

        fgets(input, sizeof(input), stdin);

        char *nl = strchr(input, '\n');
        if (nl) *nl = '\0';

        char cmd[16], arg1[256], arg2[256];
        int parts = sscanf(input, "%15s %255s %255s", cmd, arg1, arg2);

        if (strcmp(cmd, "quit") == 0) {
            break;
        } else if (strcmp(cmd, "get") == 0 && parts >= 2) {
            do_get(host, arg1);
        } else if (strcmp(cmd, "put") == 0 && parts >= 2) {
            do_put(host, arg1, parts == 3 ? arg2 : arg1);
        }
    }

    return 0;
}
