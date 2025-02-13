#ifndef TASK_H
#define TASK_H

#include <string>
#include "json.hpp"

struct Task {
    int id;
    std::string description;

    nlohmann::json to_json() const {
        return { {"id", id}, {"description", description} };
    }

    static Task from_json(const nlohmann::json& j) {
        if (!j.contains("id") || !j.contains("description") || j["description"].get<std::string>().empty()) {
            throw std::invalid_argument("Invalid task format");
        }
        return { j.at("id").get<int>(), j.at("description").get<std::string>() };
    }
};

#endif // TASK_H
