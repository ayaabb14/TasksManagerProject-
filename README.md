
Welcome to the Task Manager project!
This guide explains how to set up and run the project on Windows.

Before running the project, ensure your system meets the following requirements:
✔ Windows 10/11
✔ GCC/G++ (MinGW-w64) Installed
✔ CMake (Optional)
✔ Visual Studio Code or another IDE
✔ Required Libraries:

GLFW - for the graphical interface
OpenGL - for rendering
ImGui - for the UI
nlohmann/json - for JSON processing
httplib - for HTTP communication with the server


- Compile the Project Using g++
Open a PowerShell or CMD window, navigate to the project directory, and run:


bash:

g++ -o build/server.exe server/Server.cpp -Iserver -Isrc -I"C:/Users/Public/Downloads/TaskManagerProject/glfw/glfw-3.4.bin.WIN64/include" -L"C:/Users/Public/Downloads/TaskManagerProject/glfw/glfw-3.4.bin.WIN64/lib-mingw-w64" -lglfw3 -lws2_32

- Run the Server
Before launching the UI, make sure the server is running:

bash:

build/server.exe

If the server starts successfully, it will run on http://localhost:8080.


bash: 

g++ -o build/task_manager.exe src/main.cpp src/TaskUI.cpp src/TaskManager.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_demo.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp -Iimgui -Iimgui/backends -I"C:/Users/Public/Downloads/TaskManagerProject/glfw/glfw-3.4.bin.WIN64/include" -L"C:/Users/Public/Downloads/TaskManagerProject/glfw/glfw-3.4.bin.WIN64/lib-mingw-w64" -lglfw3 -lopengl32 -lgdi32 -lws2_32


- Run the Task Manager UI
Once the server is running, launch the Task Manager UI with:

bash:

build/task_manager.exe


 Good luck! 
