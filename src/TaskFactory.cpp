#include "TaskFactory.hpp"

#include "../include/helpers/DateTimeUtils.hpp"
#include "tasks/ComputeTask.hpp"
#include "tasks/DownloadTask.hpp"
#include "tasks/FileTask.hpp"

#include <iostream>
#include <sstream>

std::string TaskFactory::generateTaskId(const std::string &fileName) {
  int nextId = taskId.fetch_add(1);
  return std::to_string(nextId) + "_" + fileName;
}
TaskFactory::TaskFactory(int defaultMaxRetries)
    : defaultMaxRetries(defaultMaxRetries) {}

std::unique_ptr<Task> TaskFactory::createTaskFromLine(const std::string& line) {
    if (line.empty() || line[0] == '#') {
        return nullptr;
    }

    std::stringstream ss(line);
    std::string taskName;
    int taskType, taskPriority;

    if (!(ss >> taskName >> taskType >> taskPriority)) {
        return nullptr;
    }

    int maxRetries = defaultMaxRetries;
    TaskMode taskMode = TaskMode::ONCE;
    std::optional<std::chrono::system_clock::time_point> startTime;
    std::optional<std::chrono::milliseconds> cycleInterval;

    std::string nextToken;
    std::string mode;

    if (ss >> nextToken) {
        if (std::isdigit(nextToken[0])) {
            maxRetries = std::stoi(nextToken);
            ss >> mode;
        } else {
            mode = nextToken;
        }

        if (mode == "H") {
            taskMode = TaskMode::SCHEDULED;
            std::string startTimeStr;
            ss >> startTimeStr;
            startTime = DateTimeUtils::parseDateTime(startTimeStr);
        } else if (mode == "C") {
            taskMode = TaskMode::CYCLIC;
            std::string startTimeStr, cycleIntervalStr;
            ss >> startTimeStr >> cycleIntervalStr;
            startTime = DateTimeUtils::parseDateTime(startTimeStr);
            cycleInterval = std::chrono::milliseconds(std::stoi(cycleIntervalStr));
        }
    }

    std::unique_ptr<Task> newTask;
    std::string newTaskId = generateTaskId(taskName);

    switch (taskType) {
        case 1:
            newTask = std::make_unique<ComputeTask>(newTaskId, taskName, taskPriority,
                                                       maxRetries, taskMode, startTime, cycleInterval);
            break;
        case 2:
            newTask = std::make_unique<FileTask>(newTaskId, taskName, taskPriority,
                                                    maxRetries, taskMode, startTime, cycleInterval);
            break;
        case 3:
            newTask = std::make_unique<DownloadTask>(newTaskId, taskName, taskPriority,
                                                        maxRetries, taskMode, startTime, cycleInterval);
            break;
        default:
            std::cerr << "[TaskFactory] Unknown task type: " << taskType << " for file: " << taskName << "\n";
            return nullptr;
    }

    return newTask;
}