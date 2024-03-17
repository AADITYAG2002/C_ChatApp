// Client side C program to demonstrate Socket
// programming
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
 
int main(int argc, char const* argv[])
{
    int status, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
 
    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1){
        // send to server
        printf("Say: "); fgets(buffer, 1024, stdin);
        if(send(client_fd, buffer, strlen(buffer), 0) < 0){
            perror("Send failed");
            return EXIT_FAILURE;
        }
        
        // receive from server
        int read_size = recv(client_fd, buffer, 1024 - 1, 0);
        buffer[read_size - 1] = '\0'; // add null terminator and fix buffer leak

        if(read_size < 0){ // subtract 1 for the null terminator at the end
            perror("recv failed");
            break;
        }
        
        printf("server: %s\n", buffer);
    }
 
    // closing the connected socket
    close(client_fd);
    return 0;
}