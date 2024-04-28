#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// networking
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 500

void server(int port);
void *handle_connection(void* p_client_fd);

int main(int argc, char **argv) {
    int port = 8080;
    printf("Usage: server <port-optional>\n");

    if(argc == 2){
        port = atoi(argv[1]);
    }

    server(port);
    return 0;
}

void server(int port) {
    printf("Server started on port %d\n", port);

    int status, socket_fd;
    struct sockaddr_in server_info;

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = AF_UNSPEC;
    server_info.sin_port = htons(port);
    server_info.sin_addr.s_addr = htonl(INADDR_ANY);

    // try to bind the socket to a port
    if(bind(socket_fd, (struct sockaddr *)&server_info, sizeof(server_info)) == -1){
        perror("bind");
        exit(1);
    }

    if(listen(socket_fd, 10) == -1){
        perror("listen");
        exit(1);
    }

    // accept connections
    for(;;){
        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
        if(client_fd == -1){
            perror("Could not accept connection\n");
            continue;
        }

        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ipstr, sizeof(ipstr));
        printf("Got connection from %s\n", ipstr);
        
        int *p_client_fd = malloc(sizeof(int));
        *p_client_fd = client_fd;

        pthread_t thread;
        if(pthread_create(&thread, NULL, handle_connection, p_client_fd) != 0){
            perror("Could not create thread");
            free(p_client_fd);
            close(client_fd);
        }
    }

    close(socket_fd);
};

void *handle_connection(void* p_client_fd) {
    int client_fd = *((int*)p_client_fd);
    free(p_client_fd);
    char buf[BUF_SIZE];
    int bytes_read, bytes_sent;

    while((bytes_read = recv(client_fd, buf, BUF_SIZE - 1, 0)) > 0) {
        if(buf[bytes_read - 1] == '\n' && buf[bytes_read - 2] == '\r'
            && buf[bytes_read - 3] == '\n' && buf[bytes_read - 4] == '\r') {
            break;
        }
    }
    if(bytes_read == -1){
        perror("Receive error");
        close(client_fd);
        return NULL;
    }
    
    FILE *fp = fopen("../index.html", "r");
    if(fp == NULL){
        perror("Could not open file");
        return NULL;
    }

    char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    send(client_fd, header, strlen(header), 0);

    while(fgets(buf, BUF_SIZE, fp) != NULL){
        int len = strlen(buf);
        bytes_sent = send(client_fd, buf, len, 0);
        if(bytes_sent != len){
            perror("Send error");
            return NULL;
        }
    }

    close(client_fd);
    fclose(fp);
    return NULL;
};


