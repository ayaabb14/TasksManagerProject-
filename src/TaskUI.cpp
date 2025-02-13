#include "TaskUI.h"
#include "TaskManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include "Task.h"
#include <iostream>

// Global variable to store the main application window
static GLFWwindow* window = nullptr;

// Map to store edit buffers for each task, used for updating task descriptions
static std::unordered_map<int, std::string> editBuffers;

// Function to initialize the UI
void initializeUI() {
    std::cout << "Initializing GLFW..." << std::endl;
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
    std::cout << "GLFW initialized." << std::endl;

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window for the application
    window = glfwCreateWindow(1280, 720, "Task Manager", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    std::cout << "Window created successfully." << std::endl;

    // Set the current OpenGL context and enable vsync
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight(); // Use light theme for the UI
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::cout << "UI initialized successfully." << std::endl;

    // Load tasks from the server at startup
    try {
        loadTasksFromServer();
        std::cout << "Tasks loaded successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading tasks: " << e.what() << std::endl;
    }
}

// Function to render the UI in a loop
void renderUI() {
    while (!glfwWindowShouldClose(window)) {
        // Poll events and start a new ImGui frame
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Set the background color
        glClearColor(0.482f, 0.565f, 0.678f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Configure and display the main window
        ImGui::SetNextWindowSize(ImVec2(800, 600));
        ImGui::SetNextWindowPos(ImVec2((1280 - 800) / 2, (720 - 600) / 2));
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        // Application title
        ImGui::SetCursorPosX((800 - ImGui::CalcTextSize("Task Manager").x) * 0.5f);
        ImGui::SetCursorPosY(10);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Task Manager");
        ImGui::PopFont();
        ImGui::Separator();

        // Section to add a new task
        static char taskDescription[256] = "";
        ImGui::InputText("##TaskDescription", taskDescription, IM_ARRAYSIZE(taskDescription));
        ImGui::SameLine();
        if (ImGui::Button("Add Task", ImVec2(100, 30))) {
            std::string trimmedTask = std::string(taskDescription);
            if (!trimmedTask.empty()) {
                Task newTask = { 0, trimmedTask };
                saveTaskToServer(newTask); // Save the task to the server
                memset(taskDescription, 0, sizeof(taskDescription)); // Clear the input field
                loadTasksFromServer(); // Reload tasks to update the UI
            }
        }

        ImGui::Separator();

        // Display the current tasks
        auto tasks = getAllTasks();
        for (auto& task : tasks) {
            ImGui::PushID(task.id);
            ImGui::BeginChildFrame(task.id, ImVec2(ImGui::GetContentRegionAvail().x - 20, 100), ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::TextColored(ImVec4(0, 0, 0, 1), "Task %d: %s", task.id, task.description.c_str());
            ImGui::Spacing();

            // Ensure there is an edit buffer for each task
            if (editBuffers.find(task.id) == editBuffers.end()) {
                editBuffers[task.id] = task.description;
            }

            // Use the edit buffer in the input field
            char buffer[256];
            strncpy(buffer, editBuffers[task.id].c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination

            if (ImGui::InputText(("##EditTask" + std::to_string(task.id)).c_str(), buffer, sizeof(buffer))) {
                editBuffers[task.id] = std::string(buffer);
                std::cout << "✍️ Edit Buffer Updated for Task " << task.id << ": " << editBuffers[task.id] << std::endl;
            }

            ImGui::Spacing();

            // Delete button
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 160);
            if (ImGui::Button("Delete", ImVec2(70, 30))) {
                deleteTaskFromServer(task.id); // Delete the task on the server
                loadTasksFromServer(); // Reload tasks to update the UI
            }

            // Update button
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 80);
            if (ImGui::Button("Update", ImVec2(70, 30))) {
                std::string newDescription = editBuffers[task.id];

                if (newDescription != task.description) {
                    std::cout << "🔄 Updating task " << task.id << " from '" << task.description << "' to '" << newDescription << "'" << std::endl;

                    task.description = newDescription;  // Update the task locally
                    updateTaskInServer(task);          // Send the update to the server
                    loadTasksFromServer();            // Reload tasks to refresh the UI
                }
                else {
                    std::cout << "⚠️ No changes detected. Skipping update." << std::endl;
                }
            }

            ImGui::EndChildFrame();
            ImGui::PopID();
            ImGui::Spacing();
        }

        ImGui::End();

        // Render the UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

// Function to clean up resources when the UI is closed
void cleanupUI() {
    std::cout << "Cleaning up UI..." << std::endl;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "UI cleanup complete." << std::endl;
}
