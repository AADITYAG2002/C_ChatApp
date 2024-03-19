#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080

void *connection_handler(void*);

int main(int argc, char** argv){
    int server_fd, client_fd, *client_thread;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    };

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)) < 0) {
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

    // if ((client_fd = accept(server_fd, (struct sockaddr * restrict)&addr, &addrlen)) < 0) {
    //     perror("accept");
    //     exit(EXIT_FAILURE);
    // }
    
    while((client_fd = accept(server_fd, (struct sockaddr * restrict)&addr, &addrlen))){
        puts("Connection accepted");

        pthread_t sniffer_thread;
        client_thread = malloc(1);
        *client_thread = client_fd;
        
        if(pthread_create(&sniffer_thread, NULL, connection_handler, (void*) client_thread) < 0){
            perror("thread not created");
            return EXIT_FAILURE;
        }
        puts("Handler assigned");
    }
    
    close(client_fd);
    close(server_fd);
    return EXIT_SUCCESS;
}

void *connection_handler(void* client_thread){
    int client_fd = *(int*) client_thread;
    int read_size;
    char buffer[1024] = {0};
    char* server_message = "message received";
    while(1){
        // read from client
        read_size = recv(client_fd, buffer, 1024 - 1, 0);
        buffer[read_size-1] = '\0'; // adding null terminator to message and fix buffer leak
        
        if (read_size > 0){
            printf("Client sent: %s\n", buffer);
            // send response to client
            send(client_fd, server_message, strlen(server_message), 0);
        }
        if (read_size == 0){
            puts("CLient disconnected");
            break;
        }
        else if (read_size < 0){
            perror("recv failed");
            break;
        }
    }
    free(client_thread);
    return EXIT_SUCCESS;
}