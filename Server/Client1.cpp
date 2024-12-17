#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>  // For InetPton
#include <thread>
#include <atomic>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define BUFLEN 1024

std::atomic<bool> keep_running{ true };  // Flag for thread control

void sendAndReceiveMessages(SOCKET client_socket, struct sockaddr_in server_addr) {
    char message[BUFLEN];
    while (keep_running) {
        // Get input from user
        std::cout << "Enter message: ";
        std::cin.getline(message, BUFLEN);

        // Check for exit condition
        if (strcmp(message, "stop") == 0) {
            std::cout << "Exiting...\n";
            keep_running = false;
            break;
        }

        // Send message to server
        int send_len = sendto(client_socket, message, strlen(message), 0,
            (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (send_len == SOCKET_ERROR) {
            std::cerr << "Send failed! Error: " << WSAGetLastError() << std::endl;
            keep_running = false;
            break;
        }

        // Receive response from server
        char buffer[BUFLEN];
        int server_len = sizeof(server_addr);
        int recv_len = recvfrom(client_socket, buffer, BUFLEN, 0,
            (struct sockaddr*)&server_addr, &server_len);
        if (recv_len == SOCKET_ERROR) {
            std::cerr << "Receive failed! Error: " << WSAGetLastError() << std::endl;
            keep_running = false;
            break;
        }

        buffer[recv_len] = '\0';  // Null-terminate the received string
        std::cout << "Received from server: " << buffer << std::endl;
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        return -1;
    }

    // Create the UDP socket
    SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    // Setup server address
    struct sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    const char* server_ip = "127.0.0.1";

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1) {
        std::cerr << "Invalid address!" << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }

    // Launch the thread to handle both sending and receiving
    std::thread communication_thread(sendAndReceiveMessages, client_socket, server_addr);

    // Wait for the thread to finish
    communication_thread.join();

    // Cleanup
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
