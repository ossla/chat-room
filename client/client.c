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


int main() {
    #if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) { /* initializing Winsockz
                        the MAKEWORD marco allows us to request Winsock version 2.2 */
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
    #endif
    printf("Ready to use socket API.\n");

    struct addrinfo raw_addr;
    getaddrinfo();


    #if defined(_WIN32) // for windows, to clean up Winsock
        WSACleanup();
    #endif
    return 0;
}
