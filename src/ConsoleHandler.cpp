#include "ConsoleHandler.hpp"

#include "helpers/DateTimeUtils.hpp"
#include "helpers/EnumUtils.hpp"

#include <fstream>
#include <iostream>

void ConsoleHandler::getTasks() {
    isRunning = true;
    std::cout<<"Provide tasks: 1 - from file, 2 - manually\n";

    std::string mode;
    if (!std::getline(std::cin, mode)) {
        isRunning = false;
        return;
    }

    while (true) {
        if (mode == "1") {
            loadTasksFromFile();
            return;
        } else if (mode == "2") {
            parseInput();
            return;
        } else {
            std::cout << "Invalid choice! Please type 1 or 2.\n\n";
        }
    }
}

bool ConsoleHandler::addTaskFromLine(const std::string& line) const {
    if (auto task = taskFactory.createTaskFromLine(line)) {
        std::string id = task->getId();
        scheduler.addTask(std::move(task));
        // std::cout << "Task added: " << id << "\n";
        return true;
    }
    std::cout << "Invalid task format, skipped: " << line << "\n";
    return false;
}

void printTaskTableHeader() {
    std::cout << std::left
               << std::setw(28) << "TASK ID"
               << std::setw(12) << "STATUS"
               << std::setw(6)  << "PRIO"
               << std::setw(5)  << "MODE"
               << std::setw(18) << "EXEC TIME"
               << std::setw(10) << "CYCLE(ms)"
               << "\n";
    std::cout << std::string(79, '-') << "\n";
}

void printTaskRow(const TaskInfo& info) {
    std::string execTimeStr = "-";
    if (info.mode != TaskMode::ONCE) {
        execTimeStr = DateTimeUtils::formatDateTime(info.nextExecutionTime);
    }

    std::string cycleStr = "-";
    if (info.mode == TaskMode::CYCLIC) {
        cycleStr = std::to_string(info.cycleInterval.count());
    }

    std::cout << std::left
               << std::setw(28) << info.id
               << std::setw(12) << taskStatusToString(info.status)
               << std::setw(6)  << info.priority
               << std::setw(5)  << taskModeToShortString(info.mode)
               << std::setw(18) << execTimeStr
               << std::setw(10) << cycleStr
               << "\n";
}

void ConsoleHandler::printAllStatuses() const {
    auto tasks = scheduler.getAllTasksStatus();
    if (tasks.empty()) {
        std::cout << "No tasks in the system.\n";
        return;
    }
    printTaskTableHeader();
    for (const auto& info : tasks) {
        printTaskRow(info);
    }
}

void ConsoleHandler::printStatusesByStatus(const std::string& statusStr) const {
    auto parsed = stringToTaskStatus(statusStr);
    if (!parsed) {
        std::cout << "Unknown status: " << statusStr << "\n";
        return;
    }
    auto tasks = scheduler.getTasksByStatus(*parsed);
    if (tasks.empty()) {
        std::cout << "No tasks with status " << statusStr << ".\n";
        return;
    }
    printTaskTableHeader();
    for (const auto& info : tasks) {
        printTaskRow(info);
    }
}

void ConsoleHandler::printStatusForTask(const std::string & taskName) const {
    auto status = scheduler.getTaskStatus(taskName);
    if (!status) {
        std::cout << "Task not found: " << taskName << "\n";
        return;
    }
    std::cout << taskName << ": " << taskStatusToString(*status) << "\n";
}

void ConsoleHandler::processCommand(const std::string& command) {
    std::istringstream ss(command);
    std::string cmd;
    ss >> cmd;
    if (cmd == "q") {
        isRunning = false;
        std::cout << "Shutting down...\n";
        return;
    }
    if (cmd == "task") {
        std::string rest;
        std::getline(ss, rest);
        if (!rest.empty() && rest[0] == ' ') rest.erase(0, 1);
        if (rest.empty()) {
            std::cout << "Usage: task <filename> <type> <priority> [retries] [C/H] [time...]\n";
            return;
        }
        addTaskFromLine(rest);
        return;
    }

    if (cmd == "status") {
        std::string arg;
        if (!(ss >> arg)) {
            printAllStatuses();
        } else if (arg == "-s") {
            std::string statusStr;
            ss >> statusStr;
            printStatusesByStatus(statusStr);
        } else {
            printStatusForTask(arg);
        }
        return;
    }

    if (cmd == "cancel") {
        std::string name;
        if (ss >> name) {
            bool ok = scheduler.cancelTask(name);
            std::cout << (ok ? "Cancelled task nr: " + name + "\n"
                              : "Task not found: " + name + "\n");
        } else {
            std::cout << "Usage: cancel <name>\n";
        }
        return;
    }

    if (cmd == "report") {
        std::string name;
        if (ss >> name) {
            logger.exportReport(name, name + "_report.txt");
            std::cout << "Report saved to " << name << "_report.txt\n";
        } else {
            std::cout << "Usage: report <name>\n";
        }
        return;
    }

    std::cout << "Unknown command: " << cmd << "\n";
}

void ConsoleHandler::loadTasksFromFile() {
    std::string filepath;
    std::cout << "Provide filename/filepath please (txt): ";
    std::getline(std::cin, filepath);

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file: " << filepath << ". Starting with empty queue.\n";
        return;
    }

    std::string line;
    int successCount = 0;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty() || line[0] == '#') continue;

        if (auto task = taskFactory.createTaskFromLine(line)) {
            std::string id = task->getId();
            scheduler.addTask(std::move(task));
            // std::cout << "Task added: " << id << "\n";
            successCount++;
        }
    }
    std::cout << "Successfully loaded " << successCount << " tasks from " << filepath << "\n";
}

void ConsoleHandler::parseInput() {
    std::cout<<"Input tasks:\n "
               "filename type prio num_retries(o) C/H(o) time(o)\n"
               "type: 1 - computational, 2 - file operations, 3 - data downloading\n";
    std::string taskLine;
    while (std::getline(std::cin, taskLine)) {
        std::cout<<"Got task!\n";
        //validate strcuture
        //create Task using TAskFactory
    }
}

ConsoleHandler::ConsoleHandler(Scheduler &sched, TaskFactory &factory, Logger& logger)
    : scheduler(sched), taskFactory(factory), logger(logger) {}

void ConsoleHandler::start() {
    isRunning = true;
    getTasks();
    std::cout << "\nReady. Type a command (status / cancel name / report name / q):\n";

    std::string command;
    while (isRunning && std::getline(std::cin, command)) {
        if (command.empty()) continue;
        processCommand(command);
    }
};