#ifndef SERVER_H
#define SERVER_H
#include "httplib.h"

void startServer();
void loadTasksFromFile();
void saveTasksToFile();
void handleUpdateTask(const httplib::Request& req, httplib::Response& res);

#endif // SERVER_H
