#include "UDPServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>  // For InetPton (and other Winsock utilities)

#include <winsock2.h>  // Make sure winsock2.h is included for Windows-specific networking functions

// Constructor: Initializes the UDP server and binds to the port
UDPServer::UDPServer(unsigned short serverport) : UDPSocket(), mServerPort(serverport) {
    mServer.sin_family = AF_INET;
    mServer.sin_port = htons(mServerPort);  // Convert port to network byte order

    // Use InetPton to convert IP address from string to binary form
    if (inet_pton(AF_INET, SERVER, &mServer.sin_addr) != 1) {
        printf("Invalid address: %s\n", SERVER);
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the server address and port
    if (bind(s, (struct sockaddr*)&mServer, sizeof(mServer)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);  // Exit if bind fails
    }
    else {
        printf("Server started on port %d\n", mServerPort);
    }
}

// Destructor: Clean up resources (close socket)
UDPServer::~UDPServer() {
    // Perform any necessary cleanup, e.g., closing the socket
#ifdef _WIN32
    closesocket(s);  // Windows-specific
    WSACleanup();    // Clean up Winsock (only on Windows)
#else
    close(s);        // Unix/Linux-specific
#endif
    printf("Server socket closed.\n");
}
