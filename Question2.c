#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>



int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc < 4) {
        printf("Error: Insufficient arguments. Usage: %s <server> <file> <port>\n", argv[0]);
        return 1; // Return an error code indicating failure
    }

    // Declare character arrays to store host, file, and port information
    char host[20], file[20], port[20];

    // Copy command line arguments to the host, file, and port arrays
    strcpy(host, argv[1]);
    strcpy(file, argv[2]);
    strcpy(port, argv[3]);

    // Display host information on the standard output
    write(STDOUT_FILENO, "Host : ", 7);
    write(STDOUT_FILENO, host, strlen(host));
    write(STDOUT_FILENO, "\n", 1);

    // Display file information on the standard output
    write(STDOUT_FILENO, "File: ", 7);
    write(STDOUT_FILENO, file, strlen(file));
    write(STDOUT_FILENO, "\n", 1);

    // Display port information on the standard output
    write(STDOUT_FILENO, "Port: ", 7);
    write(STDOUT_FILENO, port, strlen(port));
    write(STDOUT_FILENO, "\n", 1);

    // Memory allocation to store hints
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;         // Choose Ipv = Ipv4
    hints.ai_socktype = SOCK_DGRAM;    // Choose socket type
    hints.ai_protocol = IPPROTO_UDP;   // Choose UDP protocol

    // Get server address information using getaddrinfo
    int add_serv = getaddrinfo(serv, port, &hints, &res);
    if (add_serv) {
        // Print an error message if getaddrinfo fails
        char* error_message = gai_strerror(add_serv);
        write(STDERR_FILENO, "addrinfo : ", 11);
        write(STDERR_FILENO, error_message, strlen(error_message));
        write(STDERR_FILENO, "\n", 1);
    } else {
        // Print a success message if getaddrinfo succeeds
        write(STDERR_FILENO, "Connection established successfully!\n", 36);
    }

    // Program execution success
    return 0;
}