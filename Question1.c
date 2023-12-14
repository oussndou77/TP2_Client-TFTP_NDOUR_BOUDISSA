#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Declare character arrays to store host and file information
    char host[20], file[20];
    
    // Copy command line arguments to the host and file arrays
    strcpy(host, argv[1]);
    strcpy(file, argv[2]);

    // Display host information on the standard output
    write(STDOUT_FILENO, "Host : ", 7);
    write(STDOUT_FILENO, host, strlen(host));
    write(STDOUT_FILENO, "\n", 1);

    // Display file information on the standard output
    write(STDOUT_FILENO, "File: ", 7);
    write(STDOUT_FILENO, file, strlen(file));
    write(STDOUT_FILENO, "\n", 1);

    // Program execution success
    return 0;
}

