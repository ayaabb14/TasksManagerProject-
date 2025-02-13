#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <string>
#include <vector>
#include "Task.h"

void loadTasksFromServer();
void saveTaskToServer(const Task& task);
void deleteTaskFromServer(int id);
void updateTaskInServer(const Task& task); // Add this declaration

std::vector<Task> getAllTasks();

#endif // TASK_MANAGER_H
