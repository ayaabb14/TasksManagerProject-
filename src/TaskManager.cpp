#include "TaskManager.h"
#include "httplib.h"
#include "json.hpp"
#include <unordered_map>
#include <mutex>
#include <iostream>

// Using an unordered map to store tasks with their IDs as keys
std::unordered_map<int, Task> tasks;
// Mutex to ensure thread-safe access to the tasks map
std::mutex tasksMutex;
// Server URL for connecting to the backend
const std::string SERVER_URL = "http://localhost:8080";

// Function to load tasks from the server
void loadTasksFromServer() {
    // Create an HTTP client pointing to the server
    httplib::Client cli(SERVER_URL.c_str());
    // Send a GET request to fetch all tasks
    auto res = cli.Get("/tasks");
    if (res && res->status == 200) {
        // Parse the JSON response
        auto jTasks = nlohmann::json::parse(res->body);
        // Lock the tasks map for thread-safe access
        std::lock_guard<std::mutex> lock(tasksMutex);
        tasks.clear(); // Clear the existing tasks before loading new ones
        // Iterate through the JSON object and populate the tasks map
        for (const auto& [id, taskJson] : jTasks.items()) {
            Task task = Task::from_json(taskJson); // Convert JSON to Task object
            tasks[std::stoi(id)] = task;          // Add the task to the map
        }
        std::cout << "Tasks loaded from server successfully!" << std::endl;
    }
    else {
        // Log an error if the request failed
        std::cerr << "Error: Could not load tasks from server." << std::endl;
    }
}

// Function to save a task to the server
void saveTaskToServer(const Task& task) {
    // Create an HTTP client pointing to the server
    httplib::Client cli(SERVER_URL.c_str());
    // Convert the Task object to JSON
    nlohmann::json jTask = task.to_json();
    // Send a POST request with the task JSON
    auto res = cli.Post("/tasks", jTask.dump(), "application/json");
    if (res && res->status == 200) {
        std::cout << "Task saved to server successfully!" << std::endl;
    }
    else {
        // Log an error if the request failed
        std::cerr << "Error: Could not save task to server." << std::endl;
    }
}

// Function to delete a task from the server
void deleteTaskFromServer(int id) {
    // Create an HTTP client pointing to the server
    httplib::Client cli(SERVER_URL.c_str());
    // Build the DELETE request URL with the task ID
    std::string url = "/tasks/" + std::to_string(id);
    // Send a DELETE request to the server
    auto res = cli.Delete(url.c_str());
    if (res && res->status == 200) {
        std::cout << "Task deleted from server successfully!" << std::endl;
    }
    else {
        // Log an error if the request failed
        std::cerr << "Error: Could not delete task from server." << std::endl;
    }
}

// Function to update a task on the server
void updateTaskInServer(const Task& task) {
    // Create an HTTP client pointing to the server
    httplib::Client cli(SERVER_URL.c_str());
    // Build the PUT request URL with the task ID
    std::string url = "/tasks/" + std::to_string(task.id);
    // Convert the Task object to JSON
    nlohmann::json jTask = task.to_json();

    // Log the update request being sent
    std::cout << "📤 Sending Update Request: " << jTask.dump() << std::endl;

    // Send a PUT request with the task JSON
    auto res = cli.Put(url.c_str(), jTask.dump(), "application/json");

    if (res && res->status == 200) {
        std::cout << "✅ Task updated successfully on server!" << std::endl;
    }
    else {
        // Log an error if the request failed and include server response (if any)
        std::cerr << "❌ Error updating task! Response: " << (res ? res->body : "No Response") << std::endl;
    }
}

// Function to retrieve all tasks as a vector
std::vector<Task> getAllTasks() {
    // Lock the tasks map for thread-safe access
    std::lock_guard<std::mutex> lock(tasksMutex);
    std::vector<Task> taskList;
    // Iterate through the map and add all tasks to the vector
    for (const auto& [id, task] : tasks) {
        taskList.push_back(task);
    }
    return taskList; // Return the list of tasks
}
