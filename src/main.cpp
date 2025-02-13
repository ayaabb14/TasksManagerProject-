#include "TaskUI.h"
#include <iostream>

// Entry point of the application
int main() {
    try {
        // Initialize the UI and necessary resources
        initializeUI();

        // Render the user interface (enters the main UI loop)
        renderUI();

        // Cleanup resources after the application exits
        cleanupUI();
    }
    catch (const std::exception& e) {
        // Handle any exceptions and log the error
        std::cerr << "Error: " << e.what() << std::endl;
        return -1; // Return a non-zero value to indicate an error
    }

    // Return 0 to indicate successful execution
    return 0;
}
