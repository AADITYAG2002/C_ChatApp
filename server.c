#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char** argv){
    int server_fd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int opt = 1;

    int client_fd;
    char buffer[256] = {0};
    char* server_message = "message received";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    };

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (const struct sockaddr *)&addr, addrlen) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((client_fd = accept(server_fd, (struct sockaddr * restrict)&addr, &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    int valread = read(client_fd, buffer, 256 - 1);
    printf("Client sent: %s\n", buffer);
    send(client_fd, server_message, strlen(server_message), 0);

    
    close(client_fd);
    close(server_fd);
    return EXIT_SUCCESS;
}