#include <iostream>
#include <thread>
#include <random>
#include <mutex>
#include <chrono>
#include <fstream>
#include <winsock2.h>  // Windows-specific socket header
#include <ws2tcpip.h>  // Windows-specific for additional functions
#include <sstream>     // For to_string workaround

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

std::mutex mtx;  // Mutex to synchronize output

template <typename T>
std::string to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

int simulateTemperatureData() {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distr(0, 100);  
    return distr(gen);  
}

int simulatePressureData() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(900, 1050);  
    return distr(gen);
}

void sendDataToServer(const std::string& data) {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Winsock initialization failed. Error Code: " << WSAGetLastError() << std::endl;
        return;
    }

    // Create a socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Set up the server address and port
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    // Connect to remote server
    if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection error" << std::endl;
        closesocket(s);
        WSACleanup();
        return;
    }

    // Send data
    if (send(s, data.c_str(), data.size(), 0) < 0) {
        std::cerr << "Send failed" << std::endl;
        closesocket(s);
        WSACleanup();
        return;
    }

    std::cout << "Data sent to server: " << data << std::endl;

    // Close the socket and cleanup
    closesocket(s);
    WSACleanup();
}

void collectTemperatureData(int id, int interval) {
    for (int i = 0; i < 5; i++) {
        std::lock_guard<std::mutex> lock(mtx);
        int tempData = simulateTemperatureData();
        std::string data = "Sensor " + to_string(id) + " - Temperature: " + to_string(tempData) + "°C";
        std::cout << data << std::endl;
        sendDataToServer(data);
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

void collectPressureData(int id, int interval) {
    for (int i = 0; i < 5; i++) {
        std::lock_guard<std::mutex> lock(mtx);
        int pressureData = simulatePressureData();
        std::string data = "Sensor " + to_string(id) + " - Pressure: " + to_string(pressureData) + " hPa";
        std::cout << data << std::endl;
        sendDataToServer(data);
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

int main() {
    // Create threads for each sensor type with varying intervals
    std::thread tempThread1(collectTemperatureData, 1, 1);
    std::thread tempThread2(collectTemperatureData, 2, 2);
    std::thread pressureThread1(collectPressureData, 3, 1);
    std::thread pressureThread2(collectPressureData, 4, 3);

    // Join the threads (wait for them to finish)
    tempThread1.join();
    tempThread2.join();
    pressureThread1.join();
    pressureThread2.join();

    return 0;
}
