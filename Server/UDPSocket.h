#pragma once
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
using SOCKET = int;
#define WSAGetLastError() 1
#else
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link winsock library
#endif

#define SERVER "127.0.0.1"
#define BUFLEN 1024
#define PORT 8888

class UDPSocket {
protected:
    SOCKET s;
public:
    static int WSAInit();
    UDPSocket();
    virtual ~UDPSocket();

    int SendDatagram(char* msg, unsigned int msglen, struct sockaddr* si_dest, unsigned int slen);
    int RecvDatagram(char* buf, unsigned int buflen, struct sockaddr* si_dest, int* slen);
};
