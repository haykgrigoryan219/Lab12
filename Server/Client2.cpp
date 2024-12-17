#include "UDPServer.h"
#include <iostream>
#include <cstring>
#include <thread>
#include <atomic>

#define PORT2 8888   // Port the server will bind to
#define BUFLEN 1024  // Buffer size for messages

std::atomic<bool> keep_running{ true };  // Flag to control the server loop

void serverCommunication(UDPServer& server_sock) {
    struct sockaddr_in client_addr;
    char buf[BUFLEN];
    int slen = sizeof(client_addr);

    while (keep_running) {
        // Receive a message from a client
        int recv_len = server_sock.RecvDatagram(buf, BUFLEN, (struct sockaddr*)&client_addr, &slen);
        if (recv_len == SOCKET_ERROR) {
            std::cerr << "recvfrom() failed with error code " << WSAGetLastError() << std::endl;
            continue;
        }

        // Null-terminate the received message for safety
        buf[recv_len] = '\0';
        std::cout << "Received message: \"" << buf << "\"" << std::endl;

        // Exit condition (if the client sends "stop")
        if (strcmp(buf, "stop") == 0) {
            std::cout << "Stop command received. Shutting down server..." << std::endl;
            keep_running = false;
            break;
        }

        // Send a response back to the client
        const char* response = "Message received by server!";
        int send_len = server_sock.SendDatagram((char*)response, (unsigned int)strlen(response),
            (struct sockaddr*)&client_addr, slen);
        if (send_len == SOCKET_ERROR) {
            std::cerr << "Send failed with error code " << WSAGetLastError() << std::endl;
        }
        else {
            std::cout << "Response sent to client: " << response << std::endl;
        }
    }
}

int main() {
    // Initialize server socket
    UDPServer server_sock(PORT2);
    std::cout << "Server started on port " << PORT2 << std::endl;

    // Launch the communication thread
    std::thread communication_thread(serverCommunication, std::ref(server_sock));

    // Wait for the communication thread to finish
    communication_thread.join();

    std::cout << "Server shutting down..." << std::endl;
    return 0;
}
