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

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: ipshow <host>");
        exit(1);
    }

    struct addrinfo hints, *server_info, *rf;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;


    char *host = argv[1];
    if(getaddrinfo(host, NULL, &hints, &server_info) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    for(rf = server_info; rf != NULL; rf = rf->ai_next) {
        void *addr;
        
        if (rf->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rf->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rf->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        
        inet_ntop(rf->ai_family, addr, ipstr, sizeof(ipstr));
        printf("Host ip: %s\n", ipstr);
    }

    freeaddrinfo(server_info);
}