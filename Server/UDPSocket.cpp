#include "UDPSocket.h"
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link Winsock library
#endif

// Initialize Winsock (Windows-specific)
int UDPSocket::WSAInit() {
    static unsigned int count = 0;
    if (count++) {
        return 0;
    }
#ifdef WIN32
    WSADATA wsa;

    printf("\nInitializing Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("failed. error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("initialized");
#endif
    return 0;
}

// Constructor
UDPSocket::UDPSocket() {
    UDPSocket::WSAInit();

    // Create the socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        printf("socket() failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

}

// Destructor
UDPSocket::~UDPSocket() {
#ifdef _WIN32
    closesocket(s);
    WSACleanup();
#else
    close(s);
#endif
}

// Send datagram to a specific address/port
int UDPSocket::SendDatagram(char* msg, unsigned int msglen, struct sockaddr* si_dest, unsigned int slen) {
    if (sendto(s, msg, (int)msglen, 0, si_dest, slen) == SOCKET_ERROR) {
        printf("sendto() failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    return 0;
}

//// Receive datagram from a client
//int UDPSocket::RecvDatagram(char* buf, unsigned int buflen, struct sockaddr* si_dest, int* slen) {
//    if (recvfrom(s, buf, BUFLEN, 0, si_dest, slen) == SOCKET_ERROR) 
//    {
//        printf("recvfrom() failed with error code : %d", WSAGetLastError());
//        exit(EXIT_FAILURE);
//    }
//    return 0;
//}
int UDPSocket::RecvDatagram(char* buf, unsigned int buflen, struct sockaddr* si_dest, int* slen) {
    int recv_len = recvfrom(s, buf, buflen, 0, si_dest, (int*)slen);

    if (recv_len == SOCKET_ERROR) {
        printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
        return SOCKET_ERROR;  // Return the error code if there was an issue
    }

    return recv_len;  // Return the number of bytes received
}
