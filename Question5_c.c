#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define DATA_PACKET_SIZE 512  // Adjust this size as needed

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc < 4) {
        write(STDOUT_FILENO, "Error: Insufficient arguments. Usage: %s <server> <file> <port>\n", 66);
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
        write(STDOUT_FILENO, "Connection established successfully!", 35);
        write(STDOUT_FILENO, "\n", 1);    

    }

    // Reservation of a connection socket to the server
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sock == -1) {
        write(STDERR_FILENO, "Connection error!\n", 19);
        return 1; // Return an error code indicating failure
    } else {
        write(STDERR_FILENO, "Socket reserved", 16);
        write(STDOUT_FILENO, "\n", 1);    
    }

    // Open the file for reading
    FILE* inputFile = fopen(file, "rb");
    if (!inputFile) {
        write(STDERR_FILENO, "Error opening file for reading\n", 31);
        return 1;
    }

    // Initialize block number
    uint16_t blockNum = 1;

    // Continue sending data packets until the end of the file is reached
    while (1) {
        // Read data from the file
        char buffer_send[DATA_PACKET_SIZE + 4];  // Buffer to store the data packet

        // Fill the buffer with the opcode, block number, and data
        buffer_send[0] = 0;
        buffer_send[1] = 3;  // Data opcode
        buffer_send[2] = (blockNum >> 8) & 0xFF;
        buffer_send[3] = blockNum & 0xFF;

        // Read data from the file into the buffer
        size_t bytesRead = fread(&buffer_send[4], sizeof(char), DATA_PACKET_SIZE, inputFile);

        // Check if it's the last data packet
        if (bytesRead < DATA_PACKET_SIZE) {
            // Set bytesRead as the new packet size
            DATA_PACKET_SIZE = bytesRead + 4;
        }

        // Send the data packet to the server
        ssize_t sendSize = sendto(sock, buffer_send, DATA_PACKET_SIZE, 0, res->ai_addr, res->ai_addrlen);
        if (sendSize == -1) {
            write(STDERR_FILENO, "Error sending data packet\n", 26);
            return 1;
        }

        // Increment the block number
        blockNum++;

        // Buffer for receiving acknowledgment (ACK)
        char buffer_receive[4];
        
        // Receive the acknowledgment from the server
        ssize_t recvAckSize = recvfrom(sock, buffer_receive, sizeof(buffer_receive), 0, NULL, NULL);
        if (recvAckSize == -1) {
            write(STDERR_FILENO, "Error receiving ACK\n", 21);
            return 1;
        }

        // Extract opcode and block number from the received acknowledgment
        uint16_t opcode = (buffer_receive[0] << 8) | buffer_receive[1];
        uint16_t ackBlockNum = (buffer_receive[2] << 8) | buffer_receive[3];

        // Check if the received packet is an acknowledgment (ACK)
        if (opcode == 4 && ackBlockNum == (blockNum - 1)) {
            // Print a success message for receiving ACK
            write(STDOUT_FILENO, "ACK received successfully\n", 26);
        } else {
            // Print an error message for unexpected ACK block number
            write(STDERR_FILENO, "Unexpected ACK block number\n", 28);
            return 1;
        }

        // Check if it's the last data packet
        if (bytesRead < DATA_PACKET_SIZE - 4) {
            // End of transfer
            break;
        }
    }

    // Close the file
    fclose(inputFile);

    // Print a success message for completing the file transfer
    write(STDOUT_FILENO, "File transfer completed\n", 25);

    

    return 0;
}
