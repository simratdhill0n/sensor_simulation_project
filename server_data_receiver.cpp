#include <iostream>
#include <winsock2.h>      // Windows-specific socket header
#include <ws2tcpip.h>      // For additional socket functions
#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Winsock initialization failed. Error Code: " << WSAGetLastError() << std::endl;
        return -1;
    }

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Binding the socket to the port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    while (true) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_fd);
            WSACleanup();
            return -1;
        }

        // Read data from the client
        int bytes_read = recv(new_socket, buffer, sizeof(buffer), 0);
        if (bytes_read > 0) {
            std::cout << "Data received: " << buffer << std::endl;
        } else {
            std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
        }

        closesocket(new_socket);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
