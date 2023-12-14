#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>


int main(int argc, char *argv[]) {

    char host[20], file[20];
    strcpy(host, argv[1]);
    strcpy(file, argv[2]);

    write(STDOUT_FILENO, "Host : ", 7);
    write(STDOUT_FILENO, host, strlen(host));
    write(STDOUT_FILENO, "\n", 1);

    write(STDOUT_FILENO, "File : ", 7);
    write(STDOUT_FILENO, file, strlen(file));
    write(STDOUT_FILENO, "\n", 1);

    int n_bytes;
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(struct addrinfo)); // Initialise à 0
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;

    int en = getaddrinfo(host, "1069", &hints, &res);
    if (en == -1) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    return 0;
}