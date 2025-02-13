#include "Server.h"
#include "httplib.h"
#include "json.hpp"
#include <fstream>
#include <unordered_map>
#include <iostream>

// Using the nlohmann::json library for JSON handling
using json = nlohmann::json;

// A global map to store tasks with unique task IDs as keys
std::unordered_map<int, json> tasks;
// Counter to generate unique task IDs
int nextTaskId = 1;

// Function to save all tasks to a JSON file
void saveTasksToFile() {
    // Define the file path to save tasks
    std::string path = "C:\\Users\\chouse\\Desktop\\UNI\\fourth year\\C++\\TaskManagerProject\\server\\tasks.json";
    std::ofstream file(path);

    if (file.is_open()) {
        // Convert the tasks map to JSON format
        json jTasks;
        for (const auto& [id, task] : tasks) {
            jTasks[std::to_string(id)] = task;
        }
        // Write JSON to the file with pretty formatting
        file << jTasks.dump(4);
        std::cout << "Tasks saved to file successfully at: " << path << std::endl;
    }
    else {
        // Log an error if the file could not be opened
        std::cerr << "Error: Could not open tasks.json for writing at: " << path << std::endl;
    }
}

// Function to load tasks from a JSON file
void loadTasksFromFile() {
    // Use the same path as in saveTasksToFile()
    std::string path = "C:\\Users\\chouse\\Desktop\\UNI\\fourth year\\C++\\TaskManagerProject\\server\\tasks.json";
    std::ifstream file(path);

    if (file.is_open()) {
        try {
            json jTasks;
            // Parse the JSON file and populate the tasks map
            file >> jTasks;
            for (const auto& [key, value] : jTasks.items()) {
                int id = std::stoi(key); // Convert the string key to an integer
                tasks[id] = value;      // Add the task to the map
                // Ensure the nextTaskId is updated to avoid ID conflicts
                nextTaskId = std::max(nextTaskId, id + 1);
            }
            std::cout << "Tasks loaded from file successfully!" << std::endl;
        }
        catch (...) {
            // Handle any errors during JSON parsing
            std::cerr << "Error: Failed to parse tasks.json. Starting with an empty task list." << std::endl;
        }
    }
    else {
        // If the file is not found, create a new empty file
        std::cerr << "tasks.json not found. Creating a new one." << std::endl;
        saveTasksToFile();
    }
}

// Function to handle updating a specific task by ID
void handleUpdateTask(const httplib::Request& req, httplib::Response& res) {
    // Extract the task ID from the URL
    std::string taskIdStr = req.matches[1];
    int taskId = std::stoi(taskIdStr);

    std::cout << "📥 Received update request for Task ID: " << taskId << std::endl;

    // Parse the JSON body of the request
    json taskJson = json::parse(req.body);
    std::cout << "📜 Received JSON: " << taskJson.dump() << std::endl;

    // Check if the task exists in the map
    if (tasks.find(taskId) != tasks.end()) {
        // Update the task description
        tasks[taskId]["description"] = taskJson["description"];
        // Save the updated tasks to the file
        saveTasksToFile();
        // Respond to the client with success message
        res.set_content("Task updated successfully", "text/plain");
        std::cout << "✅ Task updated: " << tasks[taskId].dump() << std::endl;
    }
    else {
        // Respond with a 404 error if the task is not found
        res.status = 404;
        res.set_content("Task not found", "text/plain");
        std::cout << "❌ Task ID " << taskId << " not found!" << std::endl;
    }
}

// Function to start the HTTP server and define routes
void startServer() {
    httplib::Server server;

    // Load tasks from file at server startup
    loadTasksFromFile();

    // Define API routes

    // GET /tasks - Retrieve all tasks in JSON format
    server.Get("/tasks", [](const httplib::Request& req, httplib::Response& res) {
        json jTasks;
        // Convert the tasks map to JSON
        for (const auto& [id, task] : tasks) {
            jTasks[std::to_string(id)] = task;
        }
        // Send the JSON response to the client
        res.set_content(jTasks.dump(), "application/json");
        });

    // POST /tasks - Add a new task to the list
    server.Post("/tasks", [](const httplib::Request& req, httplib::Response& res) {
        try {
            // Parse the JSON body of the request
            auto jTask = json::parse(req.body);
            // Assign a unique ID to the new task
            jTask["id"] = nextTaskId++;
            // Add the task to the map
            tasks[jTask["id"]] = jTask;
            // Save the updated tasks to the file
            saveTasksToFile();
            // Respond to the client with success message
            res.set_content("Task added successfully!", "text/plain");
        }
        catch (...) {
            // Respond with a 400 error for invalid JSON
            res.status = 400;
            res.set_content("Invalid JSON format", "text/plain");
        }
        });

    // PUT /tasks/{id} - Update an existing task
    server.Put(R"(/tasks/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        // Delegate to the handleUpdateTask function
        handleUpdateTask(req, res);
        });

    // DELETE /tasks/{id} - Delete a task by ID
    server.Delete(R"(/tasks/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        // Extract the task ID from the URL
        int id = std::stoi(req.matches[1]);
        // Check if the task exists and erase it
        if (tasks.erase(id)) {
            // Save the updated tasks to the file
            saveTasksToFile();
            // Respond with success message
            res.set_content("Task deleted successfully!", "text/plain");
        }
        else {
            // Respond with a 404 error if the task is not found
            res.status = 404;
            res.set_content("Task not found", "text/plain");
        }
        });

    // Log the server startup information
    std::cout << "Server running on http://localhost:8080" << std::endl;
    // Start listening on localhost:8080
    server.listen("localhost", 8080);
}

int main() {
    try {
        // Start the server
        startServer();
    }
    catch (const std::exception& e) {
        // Log any exceptions thrown during server execution
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
