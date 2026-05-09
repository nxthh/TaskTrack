#include "../include/System.h" // Point to the include folder
#include <windows.h>

/**
 * Entry point for the Task Tracking System.
 */
int main() {
    // Set terminal title (Windows only)
    SetConsoleTitleA("Task Track - Secure Management System");

    // Create system object
    System taskSystem;

    // Start application
    taskSystem.run();

    return 0;
}