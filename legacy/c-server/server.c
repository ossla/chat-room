#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
/* WIN */
#ifndef _WIN32_WINNT /* defined for the Winsock headers to provide all the functions we neeed. */
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") /* this tells the Microsoft Visual C compiler to 
                                link your program against the Winsock library, ws2_32.lib 
                                
                                IF using MinGW on Windows, use -lws2_32 flag with gcc 
                                (to link library ws2_32.lib) */
#else
/* UNIX */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

#if defined (_WIN32)
#define CLOSESOCKET(s) closesocket(s)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define CLOSESOCKET(s) close(s)
#define ISVALIDSOCKET(s) ((s) >= 0)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

int main() {
    #if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) { /* initializing Winsock g
                        the MAKEWORD marco allows us to request Winsock version 2.2 */
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
    #endif



    printf("Configuring local address...\n");
    struct addrinfo raw_addrinfo;
    memset(&raw_addrinfo, 0, sizeof(raw_addrinfo)); 
    raw_addrinfo.ai_family = AF_INET; // IPv4               (AF_INET6 для IPv6)
    raw_addrinfo.ai_flags = AI_PASSIVE; // адрес будет использоваться для привязки сокета
    raw_addrinfo.ai_socktype = SOCK_STREAM; // TCP          (SOCK_DGRAM для UDP)
    
    struct addrinfo* bind_addr;
    int result = getaddrinfo(0, "8080", &raw_addrinfo, &bind_addr);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(result));
        return 1;
    }
    
    printf("Configuring socket...\n");
    SOCKET socket_listen = socket(bind_addr->ai_family
                                , bind_addr->ai_socktype
                                , bind_addr->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "error with making socket %d\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Binding socket to local address\n");
    if(bind(socket_listen, bind_addr->ai_addr, bind_addr->ai_addrlen)) {
        fprintf(stderr, "error with binding socket %d\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_addr);

    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connection...\n");
    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }
        int is_set = 0;

        SOCKET curr_sock = 0;
        for (;curr_sock <= max_socket; ++curr_sock) {
            if (FD_ISSET(curr_sock, &reads)) {
                if (curr_sock == socket_listen) {
                    is_set = 1;
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen
                                            , (struct sockaddr*) &client_address
                                            , &client_len);
                    if (!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[100];
                    getnameinfo((struct sockaddr*) &client_address
                                , client_len, address_buffer
                                , sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);
                    break;
                } else {
                    is_set = 1;
                    char read[1024];
                    int bytes_received = recv(curr_sock, read, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(curr_sock, &master);
                        CLOSESOCKET(curr_sock);
                        break;
                    }
                    SOCKET j;
                    for (j = 1; j <= max_socket; ++j) {
                        if (FD_ISSET(j, &master)) {
                            if (j == socket_listen || j == curr_sock)
                                continue;
                            else
                                send(j, read, bytes_received, 0);
                        }
                    }
                    int bytes_sent = send(curr_sock, read, bytes_received, 0);
                    break;
                }
            }// FD_ISSET()
        } // for (SOCKET curr_sock = ...);

        if (!is_set) printf("socket wasn't found");
    } // while(1)

    CLOSESOCKET(socket_listen);
    #if defined(_WIN32) // for windows, to clean up Winsock
        WSACleanup();
    #endif
    return 0;
}
