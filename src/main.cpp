#include "../include/System.h"

/**
 * Entry point for the Task Tracking System.
 * Cross-platform: Windows, Linux, macOS.
 * Terminal title and UTF-8 setup are handled inside System::run().
 */
int main() {
    System taskSystem;
    taskSystem.run();
    return 0;
}