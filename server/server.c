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

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address; // stores info about connecting client
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen,
                            (struct sockaddr*) &client_address, &client_len);
    if (!ISVALIDSOCKET(socket_client)) {
        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    FILE* log_file;
    if (!(log_file = fopen("./log.txt", "a"))) {
        printf("fopen file for logging connections failed\n");
    } else {
        /* logging a client address to the log.txt: */
        char address_buffer[100];
        getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, 
                    sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
        fprintf(log_file, "%s\n", address_buffer);
        fclose(log_file);
    }

    printf("Reading message...\n");
    char message[1024];
    int bytes_received = recv(socket_client, message, 1024, 0);
    printf("Received %d chars. message: %.*s", bytes_received, bytes_received, message);

    printf("Sending status...\n");
    const char *status =
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Message received, ";
    send(socket_client, status, strlen(status), 0);
    const char* meeting = "Privet";
    send(socket_client, meeting, strlen(meeting), 0);

    printf("Closing connection...\n");
    CLOSESOCKET(socket_client);

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);


    #if defined(_WIN32) // for windows, to clean up Winsock
        WSACleanup();
    #endif
    return 0;
}