#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
    // Initialize character arrays to store host and file information
    char host[20], file[20];
    // Copy command line arguments to the host and file arrays
    strcpy(host, argv[1]);
    strcpy(file, argv[2]);

    // Display host information on the standard output
    write(STDOUT_FILENO, "Host : ", 7);
    write(STDOUT_FILENO, host, strlen(host));
    write(STDOUT_FILENO, "\n", 1);

    // Display file information on the standard output
    write(STDOUT_FILENO, "File : ", 7);
    write(STDOUT_FILENO, file, strlen(file));
    write(STDOUT_FILENO, "\n", 1);

    // Declare variables for address information
    int n_bytes;
    struct addrinfo hints;
    struct addrinfo *res;
    
    // Initialize the hints structure to zero
    memset(&hints, 0, sizeof(struct addrinfo));
    // Specify the socket type, address family, and protocol for address information
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;

    // Call getaddrinfo to obtain address information for the specified host and port
    int en = getaddrinfo(host, "1069", &hints, &res);
    // Check for errors in getaddrinfo
    if (en == -1) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Program execution success
    return 0;
}
