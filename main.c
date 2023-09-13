#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#define SERVER_FIFO "/tmp/server_fifo"
#define MAX_BUF_SIZE 256

int main() {
    char client_fifo[MAX_BUF_SIZE];
    pid_t pid = getpid();

    snprintf(client_fifo, sizeof(client_fifo), "/tmp/client_fifo_%d", pid);
    umask(0);
    mkfifo(client_fifo, S_IFIFO | 0666);

    int server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1) {
        perror("Failed to open server FIFO");
        exit(EXIT_FAILURE);
    }

    write(server_fd, client_fifo, strlen(client_fifo));

    int client_fd = open(client_fifo, O_RDONLY);
    if (client_fd == -1) {
        perror("Failed to open client FIFO");
        exit(EXIT_FAILURE);
    }

    char buf[MAX_BUF_SIZE];
    int bytes_read = 0;
    struct timeval start_time, current_time;
    gettimeofday(&start_time, NULL);

    while (1) {
        gettimeofday(&current_time, NULL);
        int elapsed_time = current_time.tv_sec - start_time.tv_sec;

        if (elapsed_time >= 30) {
            break;
        }

        bytes_read += read(client_fd, buf, sizeof(buf));
    }

    printf("Received %d characters from the server.\n", bytes_read);

    close(client_fd);
    unlink(client_fifo);  // Remove the client FIFO
    return 0;
}
