#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdint.h>

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

    // Construction of a read request (RRQ)
    // Establishing RRQ protocol based on RFC1350
    char buffer_send[516];  // Assuming the buffer size needed for RRQ
    buffer_send[0] = 0;
    buffer_send[1] = 1;
    strcpy(&buffer_send[2], file);
    buffer_send[2 + strlen(file)] = 0;
    strcpy(&buffer_send[2 + strlen(file) + 1], "octet");
    buffer_send[2 + strlen(file) + strlen("octet") + 2] = 0;
    int size = strlen(file) + strlen("octet") + 4;

    // Send RRQ message to the server
    ssize_t sendSize = sendto(sock, buffer_send, size, 0, res->ai_addr, res->ai_addrlen);
    if(sendSize == -1){
        write(STDERR_FILENO, "Error sending packet\n", 21);
        return 1; // Return an error code indicating failure
    }



    // Continue receiving and processing data packets until the transfer is complete
    while (1) {
        // Buffer for receiving data
        char buffer_receive[516];  // Assuming the buffer size needed for a data packet

        // Receive the data packet from the server
        ssize_t recvSize = recvfrom(sock, buffer_receive, sizeof(buffer_receive), 0, NULL, NULL);
        if (recvSize == -1) {
            // Print an error message if receiving fails
            write(STDERR_FILENO, "Error receiving data packet\n", 28);
            return 1; // Return an error code indicating failure
        }

        // Process the received data packet
        // Extract opcode, block number, and data from the received packet
        uint16_t opcode = (buffer_receive[0] << 8) | buffer_receive[1];
        uint16_t blockNum = (buffer_receive[2] << 8) | buffer_receive[3];

        // Check if the received packet is an ACK packet
        if (opcode == 4) {
            // Process the ACK packet
            // Extract block number from the ACK packet
            uint16_t ackBlockNum = (buffer_receive[2] << 8) | buffer_receive[3];

            // Check if the ACK corresponds to the expected block number
            if (ackBlockNum == expectedBlockNum) {
                // Move to the next block number
                expectedBlockNum++;

                // Print a success message for receiving ACK
                write(STDERR_FILENO, "ACK received successfully\n", 26);
            } else {
                // Print an error message for unexpected ACK block number
                write(STDERR_FILENO, "Unexpected ACK block number\n", 28);
                return 1; // Return an error code indicating failure
            }
        } else if (opcode == 3) {
            // Process the data packet
            // Write the data portion of the packet to a file, assuming data starts at index 4
            fwrite(&buffer_receive[4], sizeof(char), recvSize - 4, outputFile);

            // Send acknowledgment (ACK) for the received data packet
            char ackPacket[4];
            ackPacket[0] = 0;
            ackPacket[1] = 4;
            ackPacket[2] = (blockNum >> 8) & 0xFF;
            ackPacket[3] = blockNum & 0xFF;

            // Send the ACK packet to the server
            ssize_t sendAckSize = sendto(sock, ackPacket, sizeof(ackPacket), 0, res->ai_addr, res->ai_addrlen);
            if (sendAckSize == -1) {
                // Print an error message if sending ACK fails
                write(STDERR_FILENO, "Error sending ACK\n", 19);
                return 1; // Return an error code indicating failure
            }

            // Check if the received packet is the last data packet
            if (recvSize < sizeof(buffer_receive)) {
                // End of transfer
                break;
            }
        } else {
            // Print an error message for unexpected opcode
            write(STDERR_FILENO, "Unexpected opcode\n", 19);
            return 1; // Return an error code indicating failure
        }
    }

    // Close the output file
    fclose(outputFile);

    // Print a success message for completing the file transfer
    write(STDERR_FILENO, "File transfer completed\n", 25);

    // Program execution success
    return 0;
}
