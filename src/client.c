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

#define BUF_SIZE 500

void client(int port);

int main(int argc, char **argv) {
    if(argc != 2){
        printf("Usage: client <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    client(port);
    return 0;
}

void client(int port) {
    printf("Client started on port %d\n", port);

    int status, socket_fd;
    struct addrinfo hints, *server_info, *rp;
    char buf[BUF_SIZE], port_str[4];
    size_t bytes_read;
    socklen_t addr_size;

    sprintf(port_str, "%d", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo("127.0.0.1", port_str, &hints, &server_info)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // create a socket
    // connect to it
    for(rp = server_info; rp != NULL; rp = rp->ai_next){
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_fd == -1)
            continue;

        if(connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(socket_fd);  
    }

    freeaddrinfo(server_info);         

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(1);
    }

    const char* msg = "Hello from client\n";
    int len = strlen(msg);
    for(;;) {
        if(send(socket_fd, msg, len, 0) == -1) {
            perror("send");
            exit(1);
        }

        bytes_read = recv(socket_fd, buf, BUF_SIZE, 0);
        if(bytes_read == -1) {
            perror("recv");
            exit(1);
        }

        buf[bytes_read] = '\0';
        printf("Received from server: %s\n", buf);
    }
}