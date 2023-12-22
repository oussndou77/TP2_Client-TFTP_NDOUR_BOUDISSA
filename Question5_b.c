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
        write(STDERR_FILENO, "Error: Insufficient arguments. Usage: %s <server> <file> <port>\n", 71);
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
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;         // Choose Ipv = Ipv4
    hints.ai_socktype = SOCK_DGRAM;    // Choose socket type
    hints.ai_protocol = IPPROTO_UDP;   // Choose UDP protocol

    // Get server address information using getaddrinfo
    int add_serv = getaddrinfo(host, port, &hints, &res);
    if (add_serv) {
        // Print an error message if getaddrinfo fails
        char* error_message = gai_strerror(add_serv);
        write(STDERR_FILENO, "addrinfo : ", 11);
        write(STDERR_FILENO, error_message, strlen(error_message));
        write(STDERR_FILENO, "\n", 1);
        return 1; // Return an error code indicating failure
    } else {
        // Print a success message if getaddrinfo succeeds
        write(STDERR_FILENO, "Connection established successfully!\n", 36);
    }

    // Reservation of a connection socket to the server
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sock == -1) {
        write(STDERR_FILENO, "Connection error!\n", 19);
        return 1; // Return an error code indicating failure
    } else {
        write(STDERR_FILENO, "Connection established!\n", 24);
    }

    // Open the file for reading
    FILE* inputFile = fopen(file, "rb");
    if (!inputFile) {
        write(STDERR_FILENO, "Error opening file for reading\n", 31);
        return 1;
    }

    // Read the file content into a buffer
    char buffer_send[516];  // Assuming the buffer size needed for a data packet
    size_t bytesRead = fread(&buffer_send[4], sizeof(char), sizeof(buffer_send) - 4, inputFile);
    if (bytesRead == 0) {
        write(STDERR_FILENO, "Error reading from file\n", 25);
        fclose(inputFile);
        return 1;
    }

    // Construct Data (DAT) packet
    buffer_send[0] = 0;
    buffer_send[1] = 3;  // DAT opcode
    buffer_send[2] = 0;  // Block number high byte
    buffer_send[3] = 1;  // Block number low byte

    // Send the DAT packet to the server
    ssize_t sendSize = sendto(sock, buffer_send, bytesRead + 4, 0, res->ai_addr, res->ai_addrlen);
    if (sendSize == -1) {
        write(STDERR_FILENO, "Error sending DAT packet\n", 25);
        fclose(inputFile);
        return 1;
    }

    // Buffer for receiving acknowledgment (ACK)
    char buffer_receive[516];  // Assuming the buffer size needed for an ACK packet

    // Receive the ACK packet from the server
    ssize_t recvSize = recvfrom(sock, buffer_receive, sizeof(buffer_receive), 0, NULL, NULL);
    if(recvSize == -1){
        // Print an error message if receiving fails
        write(STDERR_FILENO, "Error receiving ACK\n", 21);
        fclose(inputFile);
        return 1; // Return an error code indicating failure
    }

    // Process the received ACK packet
    uint16_t opcode = (buffer_receive[0] << 8) | buffer_receive[1];
    uint16_t blockNum = (buffer_receive[2] << 8) | buffer_receive[3];

    // Check if the received packet is an ACK packet
    if (opcode == 4 && blockNum == 1) {
        // Print a success message for receiving ACK
        write(STDERR_FILENO, "ACK received successfully\n", 26);
    } else {
        // Print an error message for unexpected ACK
        write(STDERR_FILENO, "Unexpected ACK\n", 16);
        fclose(inputFile);
        return 1; // Return an error code indicating failure
    }

    // Close the file
    fclose(inputFile);

    // Program execution success
    return 0;
}
