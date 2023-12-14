#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    char host[20], file[20];
    strcpy(host, argv[1]);
    strcpy(file, argv[2]);

    write(STDOUT_FILENO, "Host : ", 7);
    write(STDOUT_FILENO, host, strlen(host));
    write(STDOUT_FILENO, "\n", 1);

    write(STDOUT_FILENO, "File: ", 7);
    write(STDOUT_FILENO, file, strlen(file));
    write(STDOUT_FILENO, "\n", 1);

    return 0;
}
