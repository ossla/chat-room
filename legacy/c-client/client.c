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

#pragma comment(lib, "ws2_32.lib")
  
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


int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <host> <port>", argv[0]);
        return 1;
    }
    #if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) { /* initializing Winsockz
                        the MAKEWORD marco allows us to request Winsock version 2.2 */
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
    #endif

    printf("Ready to use socket API.\n");
    struct addrinfo raw_address;
    memset(&raw_address, 0, sizeof(raw_address));
    raw_address.ai_socktype = SOCK_STREAM;
    struct addrinfo* peer_address;
    int getaddr = getaddrinfo(argv[1], argv[2], &raw_address, &peer_address);
    if (getaddr != 0) {
        fprintf(stderr, "getaddrinfo() failed: %s\n", GETSOCKETERRNO());
        return 1;
    }
    {
        printf("Remote address is: ");
        char address_buffer[100];
        char service_buffer[100];
        getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen
                    , address_buffer, sizeof(address_buffer)
                    , service_buffer, sizeof(service_buffer)
                    , NI_NUMERICHOST);
        printf("%s %s\n", address_buffer, service_buffer); 
    }

    printf("Creating socket\n");
    SOCKET socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype
                                , peer_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_peer)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Connecting...\n");
     /* This call to connect() is extremely similar to how we called bind(). Where bind() associates a socket with a local address, connect()
        associates a socket with a remote address and initiates the TCP connection */
    if (connect(socket_peer, peer_address->ai_addr
                , peer_address->ai_addrlen)) { // 
        fprintf(stderr, "connect() failed (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(peer_address);

    printf("Connected\n");
    printf("To send data, enter text followed by enter.\n");
    /* If new data comes from the terminal, we send it over the socket. 
       If new data is read from the socket, we print it out to the terminal. */


    while(1) {
        fd_set reads;
        FD_ZERO(&reads); // зануляем
        FD_SET(socket_peer, &reads);
#if !defined(_WIN32)
        FD_SET(0, &reads); // FD_SET(fileno(stdin), &reads) same effect
#endif
        struct timeval timeout;
        timeout.tv_sec;
        timeout.tv_usec = 100000;
        /* После отработки select() В reads останутся только те дескрипторы,
        на которых произошла активность (например, данные готовы для чтения).
        Все остальные удаляются.*/
        if (select(socket_peer+1, &reads, 0, 0, &timeout) < 0) { 
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        /* Эта проверка означает, что сокет socket_peer находится в reads, а это значит,
         что он готов для чтения — то есть, в нем есть данные, которые можно прочитать,
         либо соединение было закрыто (что тоже можно обнаружить при попытке чтения).*/
        if (FD_ISSET(socket_peer, &reads)) {
            char read[4096];
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if (bytes_received < 1) {
                printf("Connection closed by peer. \n");
                break;
            }
            printf("%.*s", bytes_received, read);
        }

#if defined(_WIN32)
        if (_kbhit()) { /* on Windows it indicates, whether console
                           input is waiting */
#else
        if (FD_ISSET(0, &reads)) {
#endif
            char read[4096];
            if (!fgets(read, 4096, stdin)) break;
            send(socket_peer, read, strlen(read), 0);
        }
    }// while(1)

    printf("Closing socket...\n");
    CLOSESOCKET(socket_peer);

    #if defined(_WIN32) // for windows, to clean up Winsock
        WSACleanup();
    #endif
    return 0;
}
