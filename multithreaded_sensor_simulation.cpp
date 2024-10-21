#include <iostream>
#include <thread>
#include <random>    
#include <mutex>
#include <chrono>    
#include <fstream>   // For file output

std::mutex mtx;  // Mutex to synchronize output
std::ofstream logfile("sensor_data.log");  // Create a log file

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

void collectTemperatureData(int id, int interval) {
    for (int i = 0; i < 5; i++) {  
        std::lock_guard<std::mutex> lock(mtx);  
        int tempData = simulateTemperatureData();
        std::cout << "Sensor " << id << " - Temperature: " << tempData << "°C" << std::endl;
        logfile << "Sensor " << id << " - Temperature: " << tempData << "°C" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(interval));  
    }
}

void collectPressureData(int id, int interval) {
    for (int i = 0; i < 5; i++) {  
        std::lock_guard<std::mutex> lock(mtx);  
        int pressureData = simulatePressureData();
        std::cout << "Sensor " << id << " - Pressure: " << pressureData << " hPa" << std::endl;
        logfile << "Sensor " << id << " - Pressure: " << pressureData << " hPa" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(interval));  
    }
}

int main() {
    logfile << "Sensor Data Log\n";  // Log header

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

    logfile.close();  // Close the log file
    return 0;
}
